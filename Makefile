builddir := build
boot := ${builddir}/boot.o
kernel := ${builddir}/kernel.o
isodir := ${builddir}/isodir
grub := ${isodir}/boot/grub/grub.cfg
bin := ${isodir}/boot/boss.bin
iso := ${builddir}/boss.iso

.PHONY: build
build: ${iso}

.PHONY: run
run: ${iso}
	qemu-system-i386 -cdrom ${iso}

.PHONY: clean
clean:
	rm -rf ${builddir}

.PHONY: help
help:
	@echo 'make build		default target, builds' ${iso}
	@echo 'make run  		runs generated iso with qemu'
	@echo 'make clean  		deletes build directory'
	@echo 'make help  		shows this help screen'

${iso}: ${bin} ${grub}
	dirname ${iso} | xargs mkdir -p
	grub-mkrescue -o ${iso} ${isodir}

${bin}: ${kernel} ${boot} linker.ld
	dirname ${bin} | xargs mkdir -p
	i686-elf-gcc -ffreestanding -nostdlib -T linker.ld ${boot} ${kernel} -o ${bin} -lgcc

${grub}:
	dirname ${grub} | xargs mkdir -p
	echo 'menuentry "boss" { multiboot /boot/boss.bin }' > ${grub}

${kernel}: kernel.c
	dirname ${kernel} | xargs mkdir -p
	i686-elf-gcc -std=gnu99 -ffreestanding -O2 -c kernel.c -o ${kernel}

${boot}: boot.asm
	dirname ${boot} | xargs	mkdir -p
	nasm -felf32 boot.asm -o ${boot}

