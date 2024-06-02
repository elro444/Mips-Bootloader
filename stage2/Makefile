ROOTDIR = ..
CCFLAGS = -mno-abicalls -mxgot -fno-pic -ffunction-sections -fdata-sections -I$(ROOTDIR)/include  -Wno-builtin-declaration-mismatch -Werror=all
LDFLAGS = -nostdlib -T stage2-ld.txt
LOG = @echo [+]

OUTPUT = stage2.bin

all: build soft-clean

build: $(OUTPUT)

$(OUTPUT): stage2_uncompressed.bin
	@../utils/fastlz-compress $< $@ 2> /dev/null

stage2_uncompressed.bin: stage2.elf
	$(LOG) Dumping stage2 binary
	@$(OBJCOPY) -O binary $< $@

stage2.elf: main.o libc.o serial.o commands.o fastlz.o
	$(LOG) Linking stage2
	@$(LD) $(LDFLAGS) $^ -o $@

commands.o: commands.c commands.h
	@$(CC) $(CCFLAGS) -c $< -o $@

fastlz.o: $(ROOTDIR)/common/fast_lz/fastlz.c $(ROOTDIR)/include/fastlz.h
	@$(CC) $(CCFLAGS) -c $< -o $@

libc.o: $(ROOTDIR)/common/libc.c $(ROOTDIR)/include/libc.h
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