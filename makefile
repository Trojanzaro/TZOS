
CC = i686-elf-gcc

main: kernel.c linker.ld boot.asm
	nasm -felf32 boot.asm -o boot.o
	$(CC) -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -Wall -Wextra
	$(CC) -T linker.ld -o tzos.bin -ffreestanding -nostdlib boot.o kernel.o -lgcc
	grub-file --is-x86-multiboot tzos.bin
	@echo 'menuentry "TZOS" {' > grub.cfg
	@echo '\tmultiboot /boot/tzos.bin' >> grub.cfg
	@echo '}' >> grub.cfg
	mkdir -p isodir/boot/grub
	cp tzos.bin isodir/boot/tzos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o tzos.iso isodir
	@echo 'DONE!'
	@echo 'to run enter "qemu-system-i386 -cdrom tzos.iso"'
clean:
	rm ./*.o ./*.bin ./*.iso ./isodir/boot/*.bin
	rm -r isodir
