# Purpose

In this page, I will document the challenges I am facing, and the solutions to those challenges.

## Serial not working when running from flash

So last week I managed to get the commit `754c1d6` to work.
That meant I had a kernel correctly compiled and relocated to `0x80000000`, and I managed to get the UART 8250 to print things to the screen.  
This was a major breakthrough, because it meant I could now debug things :)  
However, I was still running qemu like so:

```bash
qemu-system-mips -M mipssim -kernel kernel.elf -serial stdio
```

This week, I tried to move my program to the flash, at `0xBFC00000`. In essence, I wanted to run qemu like this:

```bash
qemu-system-mips -M mipssim -bios bootloader.bin -serial stdio
```

This meant I to dump all the sections properly to a binary file, and relocate my program to the flash instead of the RAM.  
Luckily, I did most of this last week, so it was pretty straight forward.

But then I ran into a problem. For some reason, when running the program as the bios, I didn't get my serial output. Debugging was once again difficult.

I researched online about the 8250 UART, how it worked, etc.
After some research, I tried to look for differences between running as the kernel, and running as the bios, I thought of these options:

* I assumed my `serial_init` wasn't working properly, and I just got lucky when running as the kernel, because the default bios probably initialized it for me.  
* The UART code seemed to get stuck on `is_transmit_empty`, so I was wondering if the UART is sending some interrupts and something needs to clean the buffer. I didn't set up any interrupt handler, so I looked into this as well.

These two ideas took me about a day to look around, but the problem was something totally unrelated.

Initially, my linker script was:

```txt
...
SECTIONS {
    . = 0x80000000;
    .text : {
        *(_start)
        *(.text*)
        *(.data*)
        *(.bss*)
        *(.got)
    }
    .data : {
        *(.rodata)
    }
    . += 0x1000; /* 4kb stack */
    stack_top = .; // <- Note this

...
```

After relocating to the flash, I modified it to:

```txt
...
SECTIONS {
    . = 0x80000000;
    .text : {
...
```

What this meant, was that the stack was now on the flash.  
This caused silent errors, because the flash is probably ROM in qemu, and so I could not edit it, and my local variables (and return addresses) were not saved.  
While trying the ideas in the list above, I ran into this problem and fixed it by editing my `start.S`:

```asm
_start:
    lui $sp, 0x8000
    addiu $sp, 0x1000
    lui $t9, %hi(main)
    addiu $t9, %lo(main)
    jalr $t9
    nop
```

Only when I took a step back, and tested against my original code, I realized it was working this whole time, and while looking for the other ideas I actually broke my code for UART.

Overall, learned a lot, but exhausted.


## Adding LZMA support

I looked online for some libraries. I then found TinyLZMA online `git@github.com:WangXuan95/TinyLZMA.git`

There are some problems with it, as it used `malloc` in some situations, and I want to have a simple _inflate_/_deflate_ interface, so I created my own `lzma.h` and `lzma.c` files. I will copy the implementation from the library, with minimal changes.