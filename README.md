# MIPS Bootloader

### What is this?

This is a simple implementation for a MIPS bootloader.  
It is built to run on `qemu` on the `mipssim` board.

I am writing this bootloader as an exercise to myself to learn about compilation and linking, as well as getting some low level initializations going.

### Building and running

To build the project just run `make` from the root directory.

To run - type `make run`, and it will run `qemu-system-mips` with the compiled bootloader as the bios.