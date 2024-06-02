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

You can expect the following output (will update as we go)
```
╭─ ~/repos/bootloader on master
╰─❯ make
[+] Cleaning
[+] Building stage1
[+] Linking stage1
[+] Dumping stage1 binary
[+] Building stage2
[+] Linking stage2
[+] Dumping stage2 binary
[+] Arranging flash partitions
[+] Done
╭─ ~/repos/bootloader on master
╰─❯ make run
Copying stage 2 from flash to ram
Starting decompress
fastlz_decompress: 28969
Jumping to stage 2
Hello from stage 2!!
```