MAKEFLAGS += --no-print-directory

LOG = @echo [+]
OUTPUT = flash.bin

all: clean build

run: build
	@qemu-system-mips -M mipssim -bios $(OUTPUT) -serial stdio 2> /dev/null

build: $(OUTPUT)

$(OUTPUT): stage1/stage1.bin stage2/stage2.bin
	$(LOG) Arranging flash partitions
	@utils/format_flash.py $@ \
		--partition stage1/stage1.bin:0 \
		--partition stage2/stage2.bin:0x1000
	$(LOG) Done

stage1/stage1.bin:
	@make -C stage1

stage2/stage2.bin:
	@make -C stage2

clean:
	$(LOG) Cleaning
	@make -C stage1 clean
	@make -C stage2 clean
	-@rm flash.bin 2> /dev/null || true
