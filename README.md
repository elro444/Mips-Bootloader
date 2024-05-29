# MIPS Bootloader

### What is this?

This is a simple implementation for a MIPS bootloader.  
It is built to run on `qemu` on the `mipssim` board.

I am writing this bootloader as an exercise to myself to learn about compilation and linking, as well as getting some low level initializations going.

### Building and running

#### Cross compiler

The project assumes a cross compiler is preset at `/opt/cross/mips-linux-musl-cross`. If you have one placed somewhere else just update it in the root makefile.

#### Compiling

To build the project just run `make` from the root directory.

To run - type `make run`, and it will run `qemu-system-mips` with the compiled bootloader as the bios.