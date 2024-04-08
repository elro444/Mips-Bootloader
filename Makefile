CC = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-gcc
LD = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-ld
AS = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-as
OBJCOPY = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-objcopy
OBJDUMP = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-objdump
CCFLAGS = -mxgot -fno-pic -ffunction-sections -fdata-sections -Iinclude
LDFLAGS = -nostdlib -T kernel-ld.txt # -Map=mapfile

all: clean build

run: bootloader.bin
	@qemu-system-mips -M mipssim -bios $< -serial stdio 2>/dev/null

build: bootloader.bin

bootloader.bin: kernel.elf
	$(OBJCOPY) -O binary $< $@

debug: kernel.elf
	$(OBJDUMP) -z -d $< > disasm
	./hexlify_offsets.py disasm

kernel.elf: start.o main.o serial.o
	$(LD) $(LDFLAGS) $^ -o $@

start.o: start.S
	$(AS) $< -o $@

serial.o: serial.c serial.h
	$(CC) $(CCFLAGS) -c $< -o $@

main.o:	main.c serial.h
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	-@rm bootloader.bin kernel.bin kernel.elf disasm *.o 2>/dev/null || true
