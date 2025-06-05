export OSNAME ?= boss
export ARCH ?= i386

HEADER_PROJECTS=\
kernel

PROJECTS=\
kernel \
grub

CWD != pwd

export DESTDIR := $(CWD)/isodir
ISO := $(OSNAME).iso

CFLAGS ?= -O2 -g
CFLAGS := $(CFLAGS) --sysroot=$(DESTDIR) -isystem=/usr/include
export CFLAGS

.PHONY: build
build:
	for p in $(HEADER_PROJECTS); do $(MAKE) -C "$$p" install-headers; done
	for p in $(PROJECTS); do $(MAKE) -C "$$p" install; done

.PHONY: iso
iso: $(ISO)

$(ISO): build
	grub-mkrescue -o $@ "$(DESTDIR)"

.PHONY: run
run: $(ISO)
ifndef QEMU
	$(error you must specify $$QEMU)
endif
	$(QEMU) -cdrom $(ISO)

.PHONY: clean
clean:
	rm -rf "$(DESTDIR)"
	rm -f $(ISO)
	for p in $(PROJECTS); do $(MAKE) -C "$$p" clean; done


.PHONY: help
help:
	@echo 'make build	default target, builds systemdir'
	@echo 'make iso	makes' $(ISO)
	@echo 'make run  	runs with qemu'
	@echo 'make clean  	deletes all generated artifacts'
	@echo 'make help  	shows this help screen'
