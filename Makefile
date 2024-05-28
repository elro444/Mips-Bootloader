CC = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-gcc
LD = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-ld
AS = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-as
OBJCOPY = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-objcopy
OBJDUMP = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-objdump
CCFLAGS = -mxgot -fno-pic -ffunction-sections -fdata-sections -Iinclude -Wno-builtin-declaration-mismatch
LDFLAGS = -nostdlib -T src/kernel-ld.txt

OUTPUT = flash.bin

all: clean build

run: build soft-clean
	@qemu-system-mips -M mipssim -bios $(OUTPUT) -serial stdio 2> /dev/null

build: $(OUTPUT)

$(OUTPUT): stage1.bin stage2/stage2.bin
	utils/format_flash.py $@ \
		--partition stage1.bin:0 \
		--partition stage2/stage2.bin:0x1000

stage1.bin: kernel.elf
	$(OBJCOPY) -O binary $< $@

stage2/stage2.bin:
	make -C stage2

debug: kernel.elf
	$(OBJDUMP) -z -d $< > disasm
	./hexlify_offsets.py disasm

kernel.elf: start.o main.o serial.o
	$(LD) $(LDFLAGS) $^ -o $@

start.o: src/start.S
	$(AS) $< -o $@

serial.o: src/serial.c include/serial.h
	$(CC) $(CCFLAGS) -c $< -o $@

main.o:	src/main.c include/serial.h
	$(CC) $(CCFLAGS) -c $< -o $@

soft-clean:
	-@rm kernel.elf *.o || true

clean: soft-clean
	make -C stage2 clean
	-@rm bootloader.bin 2>/dev/null || true
