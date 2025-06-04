OSNAME ?= boss
BUILDDIR ?= build

ARCHDIR := arch
ARCH ?= i386

KERNELDIR := kernel

include $(ARCHDIR)/$(ARCH)/make.config
ARCH_OBJS := $(addprefix $(BUILDDIR)/$(ARCHDIR)/$(ARCH)/,$(ARCH_OBJS))

OBJS=\
$(ARCH_OBJS) \
$(BUILDDIR)/$(KERNELDIR)/kernel.o

ISODIR := $(BUILDDIR)/isodir
ISO := $(BUILDDIR)/$(OSNAME).iso
BIN := $(ISODIR)/boot/$(OSNAME).bin
LINKERFILE := $(ARCHDIR)/$(ARCH)/linker.ld

CFLAGS ?= -O2 -g
CFLAGS := $(CFLAGS) -ffreestanding -Wall -Wextra

ifeq ($(ARCH),i386)
    NASMTARGET := elf32
else
    $(error Unsupported ARCH: $(ARCH). Supported: i386)
endif

.PHONY: build
build: $(ISO)

.PHONY: run
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

.PHONY: help
help:
	@echo 'make build		default target, builds' $(ISO)
	@echo 'make run  		runs generated iso with qemu'
	@echo 'make clean  		deletes build directory'
	@echo 'make help  		shows this help screen'

$(ISO): $(BIN) $(ISODIR)/boot/grub/grub.cfg
	dirname $@ | xargs mkdir -p
	grub-mkrescue -o $@ $(ISODIR)

$(BIN): $(OBJS) $(LINKER_FILE)
	dirname $@ | xargs mkdir -p
	$(CC) -nostdlib -T $(LINKERFILE) $(OBJS) -o $@ -lgcc

$(ISODIR)/boot/grub/grub.cfg:
	dirname $@ | xargs mkdir -p
	echo 'menuentry "boss" { multiboot /boot/boss.bin }' > $@

.PREFIXES: .o .c .asm

$(BUILDDIR)/%.o : %.c
	dirname $@ | xargs mkdir -p
	$(CC) $(CFLAGS) -std=gnu99 -c $< -o $@

$(BUILDDIR)/%.o : %.asm
	dirname $@ | xargs mkdir -p
	nasm -f $(NASMTARGET) $< -o $@

