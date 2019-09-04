nasm -felf32 boot.asm -o boot.o
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -T linker.ld -o tzos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

grub-file --is-x86-multiboot tzos.bin

echo -e 'menuentry "basilicau" {' > grub.cfg
echo -e '\tmultiboot /boot/basilica.bin' >> grub.cfg
echo -e '}' >> grub.cfg

mkdir -p isodir/boot/grub
cp tzos.bin isodir/boot/tzos.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o tzos.iso isodir


