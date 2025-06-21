BUILDDIR ?= build

ARCHDIR := arch
ARCH ?= i386

LIBS ?=
LIBS := $(LIBS) -nostdlib -lgcc -lk

include $(ARCHDIR)/$(ARCH)/make.config
ARCH_OBJS := $(addprefix $(ARCHDIR)/$(ARCH)/,$(ARCH_OBJS))

OBJS=\
$(ARCH_OBJS) \
stdio/putchar.o \
stdio/puts.o

OBJS := $(addprefix $(BUILDDIR)/,$(OBJS))

BIN := $(BUILDDIR)/libk.a

CFLAGS ?= -O2 -g
CFLAGS := $(CFLAGS) -ffreestanding -Wall -Wextra

.PHONY: build
build: $(BIN)

.PHONY: install
install: install-headers install-libs

.PHONY: install-libs
install-libs: $(BIN) ensure-destdir
	mkdir -p $(DESTDIR)/usr/lib
	cp $(BIN) $(DESTDIR)/usr/lib

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
	@echo 'make install		equivalent of `make install-headers && make install-libs`'
	@echo 'make install-headers	writes headers from include/ into $$DESTDIR'
	@echo 'make install-libs	writes lib files into $$DESTDIR'
	@echo 'make clean		deletes build directory'
	@echo 'make help		shows this help screen'

.PHONY: ensure-destdir
ensure-destdir:
ifndef DESTDIR
	$(error you must specify $$DESTDIR)
endif

$(BIN): $(OBJS)
	dirname $@ | xargs mkdir -p
	$(AR) rcs $@ $(OBJS)

.PREFIXES: .o .c

$(BUILDDIR)/%.o : %.c
	dirname $@ | xargs mkdir -p
	$(CC) -std=gnu99 -c $< -o $@ $(CXXFLAGS) $(CFLAGS)
