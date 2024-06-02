ROOTDIR = ..
CCFLAGS =  -mno-abicalls -mxgot -fno-pic -ffunction-sections -fdata-sections -I$(ROOTDIR)/include -Wno-builtin-declaration-mismatch  -Werror=all
LDFLAGS = -nostdlib -T stage1-ld.txt
LOG = @echo [+]

OUTPUT = stage1.bin

all: build soft-clean

build: $(OUTPUT)

$(OUTPUT): stage1.elf
	$(LOG) Dumping stage1 binary
	@$(OBJCOPY) -O binary $< $@

stage1.elf: start.o main.o libc.o serial.o fastlz.o
	$(LOG) Linking stage1
	@$(LD) $(LDFLAGS) $^ -o $@

start.o: start.S
	@$(AS) $< -o $@

fastlz.o: $(ROOTDIR)/common/fast_lz/fastlz.c $(ROOTDIR)/include/fastlz.h
	@$(CC) $(CCFLAGS) -c $< -o $@

libc.o: $(ROOTDIR)/common/libc.c $(ROOTDIR)/include/libc.h
	@$(CC) $(CCFLAGS) -c $< -o $@

serial.o: $(ROOTDIR)/common/serial.c $(ROOTDIR)/include/serial.h
	@$(CC) $(CCFLAGS) -c $< -o $@

main.o:	main.c $(ROOTDIR)/include/serial.h
	$(LOG) Building stage1
	@$(CC) $(CCFLAGS) -c $< -o $@

soft-clean:
	-@rm stage1.elf *.o 2> /dev/null || true

clean: soft-clean
	-@rm $(OUTPUT) 2> /dev/null || true
