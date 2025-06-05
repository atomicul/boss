OSNAME ?= boss
BUILDDIR ?= build

ARCHDIR := arch
ARCH ?= i386

KERNELDIR := kernel

include $(ARCHDIR)/$(ARCH)/make.config
ARCH_OBJS := $(addprefix $(ARCHDIR)/$(ARCH)/,$(ARCH_OBJS))

OBJS=\
$(ARCH_OBJS) \
$(KERNELDIR)/kernel.o

OBJS := $(addprefix $(BUILDDIR)/,$(OBJS))

BIN := $(BUILDDIR)/$(OSNAME).bin
LINKERFILE := $(ARCHDIR)/$(ARCH)/linker.ld

CFLAGS ?= -O2 -g
CFLAGS := $(CFLAGS) -ffreestanding -Wall -Wextra

ifeq ($(ARCH),i386)
    NASMTARGET := elf32
else
    $(error Unsupported ARCH: $(ARCH). Supported: i386)
endif

.PHONY: build
build: $(BIN)

.PHONY: install
install: install-headers install-kernel

.PHONY: install-kernel
install-kernel: $(BIN) ensure-destdir
	mkdir -p $(DESTDIR)/boot
	cp $(BIN) $(DESTDIR)/boot

.PHONY: install-headers
install-headers: ensure-destdir
	mkdir -p $(DESTDIR)/usr/include
	cp -r include/* $(DESTDIR)/usr/include

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

.PHONY: help
help:
	@echo 'make build		default target, builds' $(BIN)
	@echo 'make install		equivalent of `make install-headers && make install-kernel`'
	@echo 'make install-headers	writes headers from include/ into $$DESTDIR'
	@echo 'make install-kernel	writes kernel elf file into $$DESTDIR'
	@echo 'make clean		deletes build directory'
	@echo 'make help		shows this help screen'

.PHONY: ensure-destdir
ensure-destdir:
ifndef DESTDIR
	$(error you must specify $$DESTDIR)
endif

$(BIN): $(OBJS) $(LINKER_FILE)
	dirname $@ | xargs mkdir -p
	$(CC) -nostdlib -T $(LINKERFILE) $(OBJS) -o $@ -lgcc

.PREFIXES: .o .c .asm

$(BUILDDIR)/%.o : %.c
	dirname $@ | xargs mkdir -p
	$(CC) $(CFLAGS) -std=gnu99 -c $< -o $@

$(BUILDDIR)/%.o : %.asm
	dirname $@ | xargs mkdir -p
	nasm -f $(NASMTARGET) $< -o $@

