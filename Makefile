.RECIPEPREFIX := >

# Toolchain
CC = gcc
LD = ld
CFLAGS = -ffreestanding -m32 -fno-pie -fno-stack-protector -Wall -g
LDFLAGS = -m elf_i386 -T kernel.ld

# Dirs
ODIR = obj
SDIR = src

# Objects
OBJS = \
    kernel_main.o \
    rprintf.o \
    esp_printf.o \

OBJ = $(patsubst %,$(ODIR)/%,$(OBJS))

# Default
all: kernel

# Compile C to obj/
$(ODIR)/%.o: $(SDIR)/%.c
> mkdir -p $(ODIR)
> $(CC) $(CFLAGS) -c $< -o $@

# Link kernel
kernel: $(OBJ)
> $(LD) $(LDFLAGS) -o $@ $(OBJ)
> @echo ">>> Kernel built successfully!"

# Build bootable ISO
iso: kernel
> rm -rf iso_root
> mkdir -p iso_root/boot/grub
> cp kernel iso_root/boot/kernel
> cp grub.cfg iso_root/boot/grub/
> grub-mkrescue -o os.iso iso_root

# Run in QEMU (text mode)
run-iso: iso
> qemu-system-i386 -cdrom os.iso -nographic

# Clean
clean:
> rm -rf $(ODIR) kernel os.iso iso_root grub.img rootfs.img

