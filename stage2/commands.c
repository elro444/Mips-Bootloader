#include "commands.h"

#include "libc.h"
#include "serial.h"

command_t CMDS[CMD_COUNT] = {
    {"memwl", 2, "Memory write long", cmd_mem_write_dword},
    {"memrl", 1, "TODO", cmd_mem_read_dword},
    {"memdump", 2, "TODO", cmd_mem_read_bytes},
    {"jump", 1, "TODO", cmd_jump},
    {"rflash", 3, "TODO", cmd_flash_read},
    {"wflash", 3, "TODO", cmd_flash_write},
    {"eflash", 2, "TODO", cmd_flash_erase},
    {"reboot", 0, "TODO", cmd_reboot},
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
    if (0 == memcmp((u8*)str, "0x", 2))
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
    return result;
}

void show_help(void)
{
    printf("Available commands:\n");
    for (u8 i = 0; i < CMD_COUNT; ++i) {
        if (!CMDS[i].name)
            continue;
        printf(" * %-10s - %s\n", CMDS[i].name, CMDS[i].description);
    }
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
        return;
    }

    for (unsigned i = 0; i < CMDS[cmd].arg_count; ++i) {
        char *arg = strtok(NULL, " ", &saveptr);
        if (NULL == arg) {
            printf("Error: Expected %u arguments, but got %u\n", CMDS[cmd].arg_count, i);
            return;
        }
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

void cmd_mem_write_dword(u32 addr, u32 value)
{
    // TODO: Implement memory write to address `addr` with value `value`
    printf("Memory write at 0x%08x with value 0x%08x\n", addr, value);
}

void cmd_mem_read_dword(u32 addr)
{
    // TODO: Implement memory read from address `addr` and print the result
    printf("Memory read at 0x%08x: 0x%08x\n", addr, 0x12345678);
}

void cmd_mem_read_bytes(u32 addr, u32 count)
{
    // TODO: Implement memory read from address `addr` for `count` bytes and print the result
    printf("Memory read at 0x%08x for %u bytes:\n", addr, count);
    for (u32 i = 0; i < count; ++i) {
        printf("%02x ", 0xAB);
    }
    printf("\n");
}

void cmd_jump(u32 target)
{
    ((void (*)(void))target)();
}

void cmd_flash_read(u32 flash_offset, u32 ram_addr, u32 count)
{
    // TODO: Implement flash read from `flash_offset` to RAM at `ram_addr` for `count` bytes
    printf("Flash read from 0x%08x to RAM 0x%08x for %u bytes\n", flash_offset, ram_addr, count);
    for (u32 i = 0; i < count; ++i) {
        printf("%02x ", 0xCD);
    }
    printf("\n");
}

void cmd_flash_write(u32 flash_offset, u32 ram_addr, u32 count)
{
    // TODO: Implement flash write from RAM at `ram_addr` to `flash_offset` for `count` bytes
    printf("Flash write from RAM 0x%08x to 0x%08x for %u bytes\n", ram_addr, flash_offset, count);
    for (u32 i = 0; i < count; ++i) {
        printf("%02x ", 0xAB);
    }
    printf("\n");
}

void cmd_flash_erase(u32 flash_offset, u32 sectors_count)
{
    // TODO: Implement flash erase for `flash_offset` for `sectors_count` sectors
    printf("Flash erase from 0x%08x for %u sectors\n", flash_offset, sectors_count);
}

void cmd_reboot(void)
{
    // TODO: Implement system reboot
    printf("System reboot\n");
}
