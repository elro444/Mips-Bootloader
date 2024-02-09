CC = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-gcc
LD = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-ld
AS = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-as
OBJCOPY = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-objcopy
OBJDUMP = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-objdump
CCFLAGS = -mxgot -fno-pic -ffunction-sections -fdata-sections
LDFLAGS = -nostdlib -T kernel-ld.txt # -Map=mapfile

all: clean build run

run: kernel.elf
	@qemu-system-mips -M mipssim -kernel $< -serial stdio 2>/dev/null

build: kernel.elf

disasm: kernel.elf
	$(OBJCOPY) -O binary $< $@
	$(OBJDUMP) -z -d $< > disasm

kernel.elf: start.o main.o serial.o
	$(LD) $(LDFLAGS) $^ -o $@

start.o: start.S
	$(AS) $< -o $@

serial.o: serial.c serial.h
	$(CC) $(CCFLAGS) -c $< -o $@

main.o:	main.c serial.h
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	@-rm kernel.bin
	@-rm kernel.elf
	@-rm *.o