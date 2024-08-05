#include "commands.h"

#include "libc.h"

command_t CMDS[CMD_COUNT] = {
    {"memwl", 2, cmd_mem_write_dword},
    {"memrl", 1, cmd_mem_read_dword},
    {"memdump", 2, cmd_mem_read_bytes},
    {"jump", 1, cmd_jump},
    {"rflash", 3, cmd_flash_read},
    {"wflash", 3, cmd_flash_write},
    {"eflash", 2, cmd_flash_erase},
    {"reboot", 0, cmd_reboot},
};

int get_hex_value(char c)
{
    if ('0' <= c && c <= '9')
        return c - '0';
    if ('a' <= c && c <= 'f')
        return 10 + c - 'a';
    if ('A' <= c && c <= 'F')
        return 10 + c - 'A';
    return -1;
}

u32 parse_u32_hex(const char *str)
{
    if (0 == memcmp(str, "0x", 2))
        str += 2;

    u32 num_digits = strlen(str);
    u32 result = 0;
    u32 current_exponent = 1;

    for (unsigned i = 0; i < num_digits; ++i) {
        // First check that we only have digits
        int digit_value = get_hex_value(str[num_digits - 1 - i]);
        if (-1 == digit_value)
            return 0;

        result += current_exponent * digit_value;
        current_exponent *= 16;
    }
}

void show_help(void)
{
    // TODO
}

typedef void (*cmd_0_args)(void);
typedef void (*cmd_1_args)(u32);
typedef void (*cmd_2_args)(u32, u32);
typedef void (*cmd_3_args)(u32, u32, u32);
typedef void (*cmd_4_args)(u32, u32, u32, u32);

void run_command(char *input)
{
    char *saveptr = NULL;
    u32 args[4] = {0};

    char *command = strtok(input, " ", &saveptr);

    enum Commands cmd = CMD_NONE;

    for (u8 i = 0; i < CMD_COUNT; ++i) {
        if (!CMDS[i].name)
            continue;
        
        if (0 == strcmp(CMDS[i].name, command)) {
            cmd = i;
        }
    }

    if (CMD_NONE == cmd) {
        show_help();
    }

    for (unsigned i = 0; i < CMDS[cmd].arg_count; ++i) {
        char *arg = strtok(NULL, " ", &saveptr);
        if (NULL == arg)
            args[i] = 0;
        else
            args[i] = parse_u32_hex(arg);
    }

    switch (CMDS[cmd].arg_count) {
        case 0:
            ((cmd_0_args)CMDS[cmd].callback)();
            break;
        case 1:
            ((cmd_1_args)CMDS[cmd].callback)(args[0]);
            break;
        case 2:
            ((cmd_2_args)CMDS[cmd].callback)(args[0], args[1]);
            break;
        case 3:
            ((cmd_3_args)CMDS[cmd].callback)(args[0], args[1], args[2]);
            break;
        case 4:
            ((cmd_4_args)CMDS[cmd].callback)(args[0], args[1], args[2], args[3]);
            break;
    }
}

void cmd_mem_write_dword(u32 addr, u32 value);
void cmd_mem_read_dword(u32 addr);
void cmd_mem_read_bytes(u32 addr, u32 count);
void cmd_jump(u32 target);
void cmd_flash_read(u32 flash_offset, u32 ram_addr, u32 count);
void cmd_flash_write(u32 flash_offset, u32 ram_addr, u32 count);
void cmd_flash_erase(u32 flash_offset, u32 sectors_count);
void cmd_reboot(void);
