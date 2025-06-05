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
* GRUB, xorriso for creating a bootable image containing the GRUB bootloader
* GNU Make
* Optionally, `qemu-system` for running a virtual machine (if you wish to run `make run`)

### Instructions
```bash
# Checkout the repository
git clone https://github.com/atomicul/boss && cd boss

# Set the target architecture
export ARCH="i386" # the default, only one supported for now

# Set a suitable cross compiler
export CC="<GCC BINARY>"

# Set the qemu binary (only if you want `make run`)
export QEMU="<QEMU BINARY>"

# Build the image file
make iso   # this creates ./boss.iso
# ...OR...
make run   # requires QEMU
```
