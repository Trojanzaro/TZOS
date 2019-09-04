
CC = i686-elf-gcc

main: kernel.c linker.ld boot.asm
	nasm -felf32 boot.asm -o boot.o
	$(CC) -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -Wall -Wextra
	$(CC) -T linker.ld -o .bin -ffreestanding -nostdlib boot.o kernel.o -lgcc
	cp tzos.bin isodir/boot/tzos.bin
	grub-mkrescue -o tzos.iso isodir
	echo 'DONE!'
clean:
	rm ./*.o ./*.bin ./*.iso ./isodir/boot/*.bin
