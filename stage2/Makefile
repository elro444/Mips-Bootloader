ROOTDIR = ..
CCFLAGS = -mno-abicalls -mxgot -fno-pic -ffunction-sections -fdata-sections -I$(ROOTDIR)/include  -Wno-builtin-declaration-mismatch
LDFLAGS = -nostdlib -T stage2-ld.txt
LOG = @echo [+]

OUTPUT = stage2.bin

all: build soft-clean

build: $(OUTPUT)

$(OUTPUT): stage2.elf
	$(LOG) Dumping stage2 binary
	@$(OBJCOPY) -O binary $< $@

stage2.elf: main.o serial.o commands.o
	$(LOG) Linking stage2
	@$(LD) $(LDFLAGS) $^ -o $@

# start.o: start.S
# 	@$(AS) $< -o $@

commands.o: commands.c commands.h
	@$(CC) $(CCFLAGS) -c $< -o $@

serial.o: $(ROOTDIR)/common/serial.c $(ROOTDIR)/include/serial.h
	@$(CC) $(CCFLAGS) -c $< -o $@

main.o:	main.c $(ROOTDIR)/include/serial.h
	$(LOG) Building stage2
	@$(CC) $(CCFLAGS) -c $< -o $@

soft-clean:
	-@rm stage2.elf *.o 2> /dev/null || true

clean: soft-clean
	-@rm stage2.bin 2> /dev/null || true