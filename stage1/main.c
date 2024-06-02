#include "types.h"
#include "libc.h"
#include "serial.h"

#include "fastlz.h"

#include "nanoprintf.h"

#define FLASH_BASE (0xbfc00000)
#define STAGE2_FLASH_OFFSET (0x8000)
#define STAGE2_RAM_BASE (0x80008000)

extern u32 stack_top;

void copy_stage2_to_ram(u8 *src, u8* dst, u32 size)
{
    memcpy(dst, src, size);
}

void decompress_stage2_to_ram(u8 *src, u8* dst, u32 size)
{
    // We don't want to override our stack in the RAM
    u32 max_ram_size = 0x20000;

    int res = fastlz_decompress(src, size, dst, max_ram_size);
    printf("fastlz_decompress: %d\n", res);
}



int main(void) {
    printf("Copying stage 2 from flash to ram\n");
    u8 *src = (u8*)(FLASH_BASE + STAGE2_FLASH_OFFSET);
    u8 *dst = (u8*)STAGE2_RAM_BASE;
    u32 stage2_size = 0x8000;
    printf("Starting decompress\n");
    decompress_stage2_to_ram(src, dst, stage2_size);

    printf("Jumping to stage 2\n");
    ((void(*)(void))dst)();
    return 0;
}

