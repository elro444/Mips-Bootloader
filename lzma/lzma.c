#include "lzma.h"
#include <stdlib.h>


#define   R_OK                           0
#define   R_ERR_MEMORY_RUNOUT            1
#define   R_ERR_UNSUPPORTED              2
#define   R_ERR_OUTPUT_OVERFLOW          3
#define   R_ERR_INPUT_OVERFLOW           4
#define   R_ERR_DATA                     5
#define   R_ERR_OUTPUT_LEN_MISMATCH      6


/* Original Code */
// the code only use these basic types :
//    int      : as return code
//    u8  : as compressed and uncompressed data, as LZMA state
//    u16 : as probabilities of range coder
//    u32 : as generic integers
//    u32   : as data length

#define RET_IF_ERROR(expression) \
    {                            \
        int res = (expression);  \
        if (res != R_OK)         \
            return res;          \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// common useful functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static u32 bitsReverse(u32 bits, u32 bit_count)
{
    u32 revbits = 0;
    for (; bit_count > 0; bit_count--)
    {
        revbits <<= 1;
        revbits |= (bits & 1);
        bits >>= 1;
    }
    return revbits;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Range Decoder
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define RANGE_CODE_NORMALIZE_THRESHOLD (1 << 24)
#define RANGE_CODE_MOVE_BITS 5
#define RANGE_CODE_N_BIT_MODEL_TOTAL_BITS 11
#define RANGE_CODE_BIT_MODEL_TOTAL (1 << RANGE_CODE_N_BIT_MODEL_TOTAL_BITS)
#define RANGE_CODE_HALF_PROBABILITY (RANGE_CODE_BIT_MODEL_TOTAL >> 1)

typedef struct
{
    u32 code;
    u32 range;
    const u8 *p_src;
    const u8 *p_src_limit;
    u8 overflow;
} RangeDecoder_t;

static void rangeDecodeNormalize(RangeDecoder_t *d)
{
    if (d->range < RANGE_CODE_NORMALIZE_THRESHOLD)
    {
        if (d->p_src != d->p_src_limit)
        {
            d->range <<= 8;
            d->code <<= 8;
            d->code |= (u32)(*(d->p_src));
            d->p_src++;
        }
        else
        {
            d->overflow = 1;
        }
    }
}

static RangeDecoder_t newRangeDecoder(const u8 *p_src, u32 src_len)
{
    RangeDecoder_t coder;
    coder.code = 0;
    coder.range = 0;
    coder.p_src = p_src;
    coder.p_src_limit = p_src + src_len;
    coder.overflow = 0;
    rangeDecodeNormalize(&coder);
    rangeDecodeNormalize(&coder);
    rangeDecodeNormalize(&coder);
    rangeDecodeNormalize(&coder);
    rangeDecodeNormalize(&coder);
    coder.range = 0xFFFFFFFF;
    return coder;
}

static u32 rangeDecodeIntByFixedProb(RangeDecoder_t *d, u32 bit_count)
{
    u32 val = 0, b;
    for (; bit_count > 0; bit_count--)
    {
        rangeDecodeNormalize(d);
        d->range >>= 1;
        d->code -= d->range;
        b = !(1 & (d->code >> 31));
        if (!b)
            d->code += d->range;
        val <<= 1;
        val |= b;
    }
    return val;
}

static u32 rangeDecodeBit(RangeDecoder_t *d, u16 *p_prob)
{
    u32 prob = *p_prob;
    u32 bound;
    rangeDecodeNormalize(d);
    bound = (d->range >> RANGE_CODE_N_BIT_MODEL_TOTAL_BITS) * prob;
    if (d->code < bound)
    {
        d->range = bound;
        *p_prob = (u16)(prob + ((RANGE_CODE_BIT_MODEL_TOTAL - prob) >> RANGE_CODE_MOVE_BITS));
        return 0;
    }
    else
    {
        d->range -= bound;
        d->code -= bound;
        *p_prob = (u16)(prob - (prob >> RANGE_CODE_MOVE_BITS));
        return 1;
    }
}

static u32 rangeDecodeInt(RangeDecoder_t *d, u16 *p_prob, u32 bit_count)
{
    u32 val = 1;
    u32 i;
    for (i = 0; i < bit_count; i++)
    {
        if (!rangeDecodeBit(d, p_prob + val - 1))
        { // get bit 0
            val <<= 1;
        }
        else
        { // get bit 1
            val <<= 1;
            val |= 1;
        }
    }
    return val & ((1 << bit_count) - 1);
}

static u32 rangeDecodeByteMatched(RangeDecoder_t *d, u16 *p_prob, u32 match_byte)
{
    u32 i, val = 1, off0 = 0x100, off1; // off0 and off1 can only be 0x000 or 0x100
    for (i = 0; i < 8; i++)
    {
        match_byte <<= 1;
        off1 = off0;
        off0 &= match_byte;
        if (!rangeDecodeBit(d, (p_prob + (off0 + off1 + val - 1))))
        { // get bit 0
            val <<= 1;
            off0 ^= off1;
        }
        else
        { // get bit 1
            val <<= 1;
            val |= 1;
        }
    }
    return val & 0xFF;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LZMA Decoder
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{ // packet_type
    PKT_LIT,
    PKT_MATCH,
    PKT_SHORTREP,
    PKT_REP0, // LONGREP0
    PKT_REP1, // LONGREP1
    PKT_REP2, // LONGREP2
    PKT_REP3  // LONGREP3
} PACKET_t;

static u8 stateTransition (u8 state, PACKET_t type) {
    switch (state) {
        case  0 : return (type==PKT_LIT) ?  0 : (type==PKT_MATCH) ?  7 : (type==PKT_SHORTREP) ?  9 :  8;
        case  1 : return (type==PKT_LIT) ?  0 : (type==PKT_MATCH) ?  7 : (type==PKT_SHORTREP) ?  9 :  8;
        case  2 : return (type==PKT_LIT) ?  0 : (type==PKT_MATCH) ?  7 : (type==PKT_SHORTREP) ?  9 :  8;
        case  3 : return (type==PKT_LIT) ?  0 : (type==PKT_MATCH) ?  7 : (type==PKT_SHORTREP) ?  9 :  8;
        case  4 : return (type==PKT_LIT) ?  1 : (type==PKT_MATCH) ?  7 : (type==PKT_SHORTREP) ?  9 :  8;
        case  5 : return (type==PKT_LIT) ?  2 : (type==PKT_MATCH) ?  7 : (type==PKT_SHORTREP) ?  9 :  8;
        case  6 : return (type==PKT_LIT) ?  3 : (type==PKT_MATCH) ?  7 : (type==PKT_SHORTREP) ?  9 :  8;
        case  7 : return (type==PKT_LIT) ?  4 : (type==PKT_MATCH) ? 10 : (type==PKT_SHORTREP) ? 11 : 11;
        case  8 : return (type==PKT_LIT) ?  5 : (type==PKT_MATCH) ? 10 : (type==PKT_SHORTREP) ? 11 : 11;
        case  9 : return (type==PKT_LIT) ?  6 : (type==PKT_MATCH) ? 10 : (type==PKT_SHORTREP) ? 11 : 11;
        case 10 : return (type==PKT_LIT) ?  4 : (type==PKT_MATCH) ? 10 : (type==PKT_SHORTREP) ? 11 : 11;
        case 11 : return (type==PKT_LIT) ?  5 : (type==PKT_MATCH) ? 10 : (type==PKT_SHORTREP) ? 11 : 11;
        default : return 0xFF;                                                                              // 0xFF is invalid state which will never appear
    }
}


#define N_STATES 12
#define N_LIT_STATES 7

#define MAX_LC 8 // max value of lc is 8, see LZMA specification
#define N_PREV_BYTE_LC_MSBS (1 << MAX_LC)

#define MAX_LP 4 // max value of lp is 4, see LZMA specification
#define N_LIT_POS_STATES (1 << MAX_LP)

#define MAX_PB 4 // max value of pb is 4, see LZMA specification
#define N_POS_STATES (1 << MAX_PB)

#define INIT_PROBS(probs)                           \
    {                                               \
        u16 *p = (u16 *)(probs);                    \
        u16 *q = p + (sizeof(probs) / sizeof(u16)); \
        for (; p < q; p++)                          \
            *p = RANGE_CODE_HALF_PROBABILITY;       \
    } // all probabilities are init to 50% (half probability)

#define INIT_PROBS_LITERAL(probs)                                             \
    {                                                                         \
        u16 *p = (u16 *)(probs);                                              \
        u16 *q = p + (N_PREV_BYTE_LC_MSBS * N_LIT_POS_STATES * 3 * (1 << 8)); \
        for (; p < q; p++)                                                    \
            *p = RANGE_CODE_HALF_PROBABILITY;                                 \
    } // all probabilities are init to 50% (half probability)

static int lzmaDecode(const u8 *p_src, u32 src_len, u8 *p_dst, u32 *p_dst_len, u8 lc, u8 lp, u8 pb)
{
    const u32 lc_shift = (8 - lc);
    const u32 lc_mask = (1 << lc) - 1;
    const u32 lp_mask = (1 << lp) - 1;
    const u32 pb_mask = (1 << pb) - 1;

    u8 state = 0; // valid value : 0~12
    u32 pos = 0;  // position of uncompressed data (p_dst)
    u32 rep0 = 1;
    u32 rep1 = 1;
    u32 rep2 = 1;
    u32 rep3 = 1;

    RangeDecoder_t coder = newRangeDecoder(p_src, src_len);

    // probability arrays ---------------------------------------
    u16 probs_is_match[N_STATES][N_POS_STATES];
    u16 probs_is_rep[N_STATES];
    u16 probs_is_rep0[N_STATES];
    u16 probs_is_rep0_long[N_STATES][N_POS_STATES];
    u16 probs_is_rep1[N_STATES];
    u16 probs_is_rep2[N_STATES];
    u16 probs_dist_slot[4][(1 << 6) - 1];
    u16 probs_dist_special[10][(1 << 5) - 1];
    u16 probs_dist_align[(1 << 4) - 1];
    u16 probs_len_choice[2];
    u16 probs_len_choice2[2];
    u16 probs_len_low[2][N_POS_STATES][(1 << 3) - 1];
    u16 probs_len_mid[2][N_POS_STATES][(1 << 3) - 1];
    u16 probs_len_high[2][(1 << 8) - 1];

    // u16 probs_literal   [N_PREV_BYTE_LC_MSBS] [N_LIT_POS_STATES] [3*(1<<8)];

    u16(*probs_literal)[N_LIT_POS_STATES][3 * (1 << 8)];

    probs_literal = (u16(*)[N_LIT_POS_STATES][3 * (1 << 8)]) malloc(sizeof(u16) * N_PREV_BYTE_LC_MSBS * N_LIT_POS_STATES * 3 * (1 << 8)); // since this array is quiet large (3145728 items, 6MB), we need to use malloc

    if (probs_literal == 0)
        return R_ERR_MEMORY_RUNOUT;

    INIT_PROBS(probs_is_match);
    INIT_PROBS(probs_is_rep);
    INIT_PROBS(probs_is_rep0);
    INIT_PROBS(probs_is_rep0_long);
    INIT_PROBS(probs_is_rep1);
    INIT_PROBS(probs_is_rep2);
    INIT_PROBS(probs_dist_slot);
    INIT_PROBS(probs_dist_special);
    INIT_PROBS(probs_dist_align);
    INIT_PROBS(probs_len_choice);
    INIT_PROBS(probs_len_choice2);
    INIT_PROBS(probs_len_low);
    INIT_PROBS(probs_len_mid);
    INIT_PROBS(probs_len_high);
    // INIT_PROBS(probs_literal);
    INIT_PROBS_LITERAL(probs_literal);

    while (pos < *p_dst_len)
    { // main loop
        const u32 pos_state = pb_mask & (u32)pos;

        if (coder.overflow)
            return R_ERR_INPUT_OVERFLOW;

        if (!rangeDecodeBit(&coder, &probs_is_match[state][pos_state]))
        { // decoded bit sequence = 0 (packet LIT)
            const u32 literal_pos_state = lp_mask & (u32)pos;
            u32 prev_byte_lc_msbs = 0;
            u32 curr_byte;

            if (pos > 0)
                prev_byte_lc_msbs = lc_mask & (p_dst[pos - 1] >> lc_shift);

            if (state < N_LIT_STATES)
            {
                curr_byte = rangeDecodeInt(&coder, probs_literal[prev_byte_lc_msbs][literal_pos_state], 8);
            }
            else
            {
                if (pos < (u32)rep0)
                    return R_ERR_DATA;
                curr_byte = rangeDecodeByteMatched(&coder, probs_literal[prev_byte_lc_msbs][literal_pos_state], p_dst[pos - rep0]);
            }

            // printf("[LZMAd]   @%08lx PKT_LIT   decoded_byte=%02x   state=%d\n", pos, curr_byte, state);

            if (pos == *p_dst_len)
                return R_ERR_OUTPUT_OVERFLOW;

            p_dst[pos] = (u8)curr_byte;
            pos++;

            state = stateTransition(state, PKT_LIT);
        }
        else
        { // decoded bit sequence = 1 (packet MATCH, SHORTREP, or LONGREP*)
            u32 dist = 0, len = 0;

            if (!rangeDecodeBit(&coder, &probs_is_rep[state]))
            { // decoded bit sequence = 10 (packet MATCH)
                rep3 = rep2;
                rep2 = rep1;
                rep1 = rep0;
                // printf("[LZMAd]   @%08lx PKT_MATCH   state=%d\n", pos, state);
                state = stateTransition(state, PKT_MATCH);
            }
            else if (!rangeDecodeBit(&coder, &probs_is_rep0[state]))
            { // decoded bit sequence = 110 (packet SHORTREP or LONGREP0)
                dist = rep0;
                if (!rangeDecodeBit(&coder, &probs_is_rep0_long[state][pos_state]))
                { // decoded bit sequence = 1100 (packet SHORTREP)
                    len = 1;
                    // printf("[LZMAd]   @%08lx PKT_SHORTREP   state=%d\n", pos, state);
                    state = stateTransition(state, PKT_SHORTREP);
                }
                else
                { // decoded bit sequence = 1101 (packet LONGREP0)
                    // printf("[LZMAd]   @%08lx PKT_REP0   state=%d\n", pos, state);
                    state = stateTransition(state, PKT_REP0);
                }
            }
            else if (!rangeDecodeBit(&coder, &probs_is_rep1[state]))
            { // decoded bit sequence = 1110 (packet LONGREP1)
                dist = rep1;
                rep1 = rep0;
                // printf("[LZMAd]   @%08lx PKT_REP1   state=%d\n", pos, state);
                state = stateTransition(state, PKT_REP1);
            }
            else if (!rangeDecodeBit(&coder, &probs_is_rep2[state]))
            { // decoded bit sequence = 11110 (packet LONGREP2)
                dist = rep2;
                rep2 = rep1;
                rep1 = rep0;
                // printf("[LZMAd]   @%08lx PKT_REP2   state=%d\n", pos, state);
                state = stateTransition(state, PKT_REP2);
            }
            else
            { // decoded bit sequence = 11111 (packet LONGREP3)
                dist = rep3;
                rep3 = rep2;
                rep2 = rep1;
                rep1 = rep0;
                // printf("[LZMAd]   @%08lx PKT_REP3   state=%d\n", pos, state);
                state = stateTransition(state, PKT_REP3);
            }

            if (len == 0)
            { // unknown length, need to decode
                if (!rangeDecodeBit(&coder, &probs_len_choice[dist == 0]))
                    len = 2 + rangeDecodeInt(&coder, probs_len_low[dist == 0][pos_state], 3); // len = 2~9
                else if (!rangeDecodeBit(&coder, &probs_len_choice2[dist == 0]))
                    len = 10 + rangeDecodeInt(&coder, probs_len_mid[dist == 0][pos_state], 3); // len = 10~17
                else
                    len = 18 + rangeDecodeInt(&coder, probs_len_high[dist == 0], 8); // len = 18~273
                // printf("[LZMAd]   len = %u\n", len);
            }

            if (dist == 0)
            { // unknown distance, need to decode
                const u32 len_min5_minus2 = (len > 5) ? 3 : (len - 2);
                u32 dist_slot;
                u32 mid_bcnt = 0;
                u32 low_bcnt = 0;
                u32 low_bits = 0;

                dist_slot = rangeDecodeInt(&coder, probs_dist_slot[len_min5_minus2], 6); // decode distance slot (0~63)

                if (dist_slot >= 14)
                {                                 // dist slot = 14~63
                    dist = (2 | (dist_slot & 1)); // high 2 bits of dist
                    mid_bcnt = (dist_slot >> 1) - 5;
                    low_bcnt = 4;
                }
                else if (dist_slot >= 4)
                {                                 // dist slot = 4~13
                    dist = (2 | (dist_slot & 1)); // high 2 bits of dist
                    low_bcnt = (dist_slot >> 1) - 1;
                }
                else
                { // dist slot = 0~3
                    dist = dist_slot;
                }

                dist <<= mid_bcnt;
                dist |= rangeDecodeIntByFixedProb(&coder, mid_bcnt);

                if (dist_slot >= 14) // dist slot = 14~63
                    low_bits = rangeDecodeInt(&coder, probs_dist_align, low_bcnt);
                else if (dist_slot >= 4) // dist slot = 4~13
                    low_bits = rangeDecodeInt(&coder, probs_dist_special[dist_slot - 4], low_bcnt);

                low_bits = bitsReverse(low_bits, low_bcnt);

                dist <<= low_bcnt;
                dist |= low_bits;

                if (dist == 0xFFFFFFFF)
                {
                    // printf("[LZMAd]   meet end marker\n");
                    break;
                }

                dist++;

                // printf("[LZMAd]   dist_slot = %u   dist = %u\n", dist_slot, dist);
            }

            rep0 = dist;

            if (dist > pos || dist == 0)
                return R_ERR_DATA;

            for (; len > 0; len--)
            {
                if (pos == *p_dst_len)
                    return R_ERR_OUTPUT_OVERFLOW;
                p_dst[pos] = p_dst[pos - dist];
                pos++;
            }
        }
    }

    free(probs_literal);

    *p_dst_len = pos;

    return R_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LZMA decompress (include parsing ".lzma" format's header)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LZMA_HEADER_LEN 13
#define LZMA_DIC_MIN (1 << 12)

static int parseLzmaHeader(const u8 *p_src, u8 *p_lc, u8 *p_lp, u8 *p_pb, u32 *p_dict_len, u32 *p_uncompressed_len, u32 *p_uncompressed_len_known)
{
    u8 byte0 = p_src[0];

    *p_dict_len = ((u32)p_src[1]) | ((u32)p_src[2] << 8) | ((u32)p_src[3] << 16) | ((u32)p_src[4] << 24);

    if (*p_dict_len < LZMA_DIC_MIN)
        *p_dict_len = LZMA_DIC_MIN;

    if (p_src[5] == 0xFF && p_src[6] == 0xFF && p_src[7] == 0xFF && p_src[8] == 0xFF && p_src[9] == 0xFF && p_src[10] == 0xFF && p_src[11] == 0xFF && p_src[12] == 0xFF)
    {
        *p_uncompressed_len_known = 0;
    }
    else
    {
        u32 i;
        *p_uncompressed_len_known = 1;
        *p_uncompressed_len = 0;
        for (i = 0; i < 8; i++)
        {
            if (i < sizeof(u32))
            {
                *p_uncompressed_len |= (((u32)p_src[5 + i]) << (i << 3)); // get (sizeof(u32)) bytes from p_src, and put it to (*p_uncompressed_len)
            }
            else if (p_src[5 + i] > 0)
            {
                return R_ERR_OUTPUT_OVERFLOW; // uncompressed length overflow from the machine's memory address limit
            }
        }
    }

    *p_lc = (u8)(byte0 % 9);
    byte0 /= 9;
    *p_lp = (u8)(byte0 % 5);
    *p_pb = (u8)(byte0 / 5);

    if (*p_lc > MAX_LC || *p_lp > MAX_LP || *p_pb > MAX_PB)
        return R_ERR_UNSUPPORTED;

    return R_OK;
}

/* Original Code End*/

int lzma_deflate(const u8 *data, u32 data_size, u8 *dst, u32 *dst_size)
{
    return 0;
}

int lzma_inflate(const u8 *data, u32 data_size, u8 *dst, u32 *dst_size)
{
    u8 lc, lp, pb; // lc=0~8   lp=0~4   pb=0~4
    u32 dict_len, uncompressed_len_known;
    u32 uncompressed_len = 0;

    if (data_size < LZMA_HEADER_LEN)
        return R_ERR_INPUT_OVERFLOW;

    RET_IF_ERROR(parseLzmaHeader(data, &lc, &lp, &pb, &dict_len, &uncompressed_len, &uncompressed_len_known))

    // printf("[LZMAd] lc=%d   lp=%d   pb=%d   dict_len=%u\n", lc, lp, pb, dict_len);

    if (uncompressed_len_known)
    {
        if (uncompressed_len > *dst_size)
            return R_ERR_OUTPUT_OVERFLOW;
        *dst_size = uncompressed_len;
        // printf("[LZMAd] uncompressed length = %lu (parsed from header)\n"                                  , *dst_size);
    }
    else
    {
        // printf("[LZMAd] uncompressed length is not in header, decoding using output buffer length = %lu\n" , *dst_size);
    }

    RET_IF_ERROR(lzmaDecode(data + LZMA_HEADER_LEN, data_size - LZMA_HEADER_LEN, dst, dst_size, lc, lp, pb));

    if (uncompressed_len_known && uncompressed_len != *dst_size)
        return R_ERR_OUTPUT_LEN_MISMATCH;

    return R_OK;
}
