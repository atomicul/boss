builddir := build
boot := ${builddir}/boot.o
kernel := ${builddir}/kernel.o
isodir := ${builddir}/isodir
grub := ${isodir}/boot/grub/grub.cfg
bin := ${isodir}/boot/boss.bin
iso := ${builddir}/boss.iso

${iso}: ${bin} ${grub}
	dirname ${iso} | xargs mkdir -p
	grub-mkrescue -o ${iso} ${isodir}

run: ${iso}
	qemu-system-i386 -cdrom ${iso}

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

clean:
	rm -rf ${builddir}
