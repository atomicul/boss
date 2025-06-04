# bOSs
Practice repository to get me accustomed to low-level OS concepts.

## Running the operating system
First, download an image from the [releases](https://github.com/atomicul/boss/releases) page
or follow the instructions from [Building a bootable image](#building-a-bootable-image).
Then, boot it with your favorite VM hypervisor software, or flash it to a USB drive to
run directly on hardware.

## Building a bootable image
### Dependencies
* A [GCC Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler) for **i686-elf**
* Netwide Assembler (NASM)
* `grub-mkrescue`, `xorriso` for creating a bootable image containing the GRUB bootloader
* GNU make
* Optionally, `qemu-system` for running a virtual machine (if you wish to run `make run`)

### Instructions
```bash
# Checkout the repository
git clone https://github.com/atomicul/boss && cd boss

# Set a suitable cross compiler
export CC="<GCC BINARY>"

# Build the image file
make build # this creates ./build/boss.iso
# ...OR...
make run   # requires QEMU
```
