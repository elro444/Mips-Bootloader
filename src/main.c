#include "types.h"
#include "libc.h"
#include "serial.h"

#define FLASH_BASE (0xbfc00000)
#define STAGE2_FLASH_OFFSET (0x1000)
#define STAGE2_RAM_BASE (0x80008000)

int main(void) {
    serial_out("Copying flash from flash to ram\n");
    u8 *src = (u8*)(FLASH_BASE + STAGE2_FLASH_OFFSET);
    u8 *dst = (u8*)STAGE2_RAM_BASE;
    memcpy(src, dst, 0x4000);
    serial_out("Jumping to stage 2\n");
    ((void(*)(void))dst)();
    return 0;
}

