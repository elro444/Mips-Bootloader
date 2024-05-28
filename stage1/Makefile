CC = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-gcc
LD = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-ld
AS = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-as
ROOTDIR = ..
OBJCOPY = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-objcopy
OBJDUMP = /opt/cross/mips-linux-musl-cross/bin/mips-linux-musl-objdump
CCFLAGS =  -mno-abicalls -mxgot -fno-pic -ffunction-sections -fdata-sections -I$(ROOTDIR)/include -Wno-builtin-declaration-mismatch
LDFLAGS = -nostdlib -T stage1-ld.txt
LOG = @echo [+]

OUTPUT = stage1.bin

all: build soft-clean

build: $(OUTPUT)

$(OUTPUT): stage1.elf
	$(LOG) Dumping stage1 binary
	@$(OBJCOPY) -O binary $< $@

stage1.elf: start.o main.o serial.o
	$(LOG) Linking stage1
	@$(LD) $(LDFLAGS) $^ -o $@

start.o: start.S
	@$(AS) $< -o $@

serial.o: $(ROOTDIR)/common/serial.c $(ROOTDIR)/include/serial.h
	@$(CC) $(CCFLAGS) -c $< -o $@

main.o:	main.c $(ROOTDIR)/include/serial.h
	$(LOG) Building stage1
	@$(CC) $(CCFLAGS) -c $< -o $@

soft-clean:
	-@rm stage1.elf *.o 2> /dev/null || true

clean: soft-clean
	-@rm $(OUTPUT) 2> /dev/null || true
