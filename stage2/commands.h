#ifndef COMMANDS_H
#define COMMANDS_H

typedef void (*command_callback)(char **argv);

typedef struct {
    char *name;
    unsigned arg_count;
    command_callback callback;
} command_t;

#endif // COMMANDS_H
