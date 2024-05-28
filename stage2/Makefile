CC = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-gcc
LD = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-ld
AS = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-as
ROOTDIR = ..
OBJCOPY = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-objcopy
OBJDUMP = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-objdump
CCFLAGS = -nostdlib -mxgot -fno-pic -ffunction-sections -fdata-sections -I$(ROOTDIR)/include  -Wno-builtin-declaration-mismatch
LDFLAGS = -nostdlib -T stage2-ld.txt

OUTPUT = stage2.bin

all: clean build

build: $(OUTPUT)

$(OUTPUT): stage2.elf
	$(OBJCOPY) -O binary $< $@

stage2.elf: start.o main.o serial.o commands.o
	$(LD) $(LDFLAGS) $^ -o $@

start.o: start.S
	$(AS) $< -o $@

commands.o: commands.c commands.h
	$(CC) $(CCFLAGS) -c $< -o $@

serial.o: $(ROOTDIR)/src/serial.c $(ROOTDIR)/include/serial.h
	$(CC) $(CCFLAGS) -c $< -o $@

main.o:	main.c $(ROOTDIR)/include/serial.h
	$(CC) $(CCFLAGS) -c $< -o $@

soft-clean:
	-@rm stage2.elf *.o 2> /dev/null || true

clean: soft-clean
	-@rm stage2.bin 2> /dev/null || true