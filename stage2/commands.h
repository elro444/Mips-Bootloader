#ifndef COMMANDS_H
#define COMMANDS_H

#include <types.h>

typedef void* command_callback;

typedef struct {
    char *name;
    unsigned arg_count;
    char *description;
    command_callback callback;
} command_t;

enum Commands
{
    CMD_MEM_WRITE_DWORD,
    CMD_MEM_READ_DWORD,
    CMD_MEM_READ_BYTES,
    CMD_JUMP,
    CMD_FLASH_READ,
    CMD_FLASH_WRITE,
    CMD_FLASH_ERASE,
    CMD_REBOOT,
    CMD_NONE,

    /* Keep Last */
    CMD_COUNT
};

void run_command(char *input);

void cmd_mem_write_dword(u32 addr, u32 value);
void cmd_mem_read_dword(u32 addr);
void cmd_mem_read_bytes(u32 addr, u32 count);
void cmd_jump(u32 target);
void cmd_flash_read(u32 flash_offset, u32 ram_addr, u32 count);
void cmd_flash_write(u32 flash_offset, u32 ram_addr, u32 count);
void cmd_flash_erase(u32 flash_offset, u32 sectors_count);
void cmd_reboot(void);



#endif // COMMANDS_H
