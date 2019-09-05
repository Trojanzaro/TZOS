
# TZOS

just a simple attempt at a Kernel for a very limited OS

Cross compiler required for this to work with the following steps for installation (Still not successful, I'll go through a few more attempts before I have enough of the VMs)

1. Install dependencies

`sudo apt-get install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libcloog-isl-dev libisl-dev qemu grub-common xorriso nasm grub-pc-bin`

3. Setup some basic environmental variables 

```
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
mkdir ~/src
```

4. Install the bin-utils

```
cd ~/src  
wget https://ftp.gnu.org/gnu/binutils/binutils-2.31.1.tar.xz  
tar -xf binutils-2.31.1.tar.xz  
rm binutils-2.31.1.tar.xz  
mkdir build-binutils  
cd build-binutils/  
../binutils-2.31.1/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror  
make  
make install
```

5. Install GCC with cross compiler settings

```
cd ~/src 
wget https://ftp.gnu.org/gnu/gcc/gcc-7.4.0/gcc-7.4.0.tar.xz
tar -xf gcc-7.4.0.tar.xz  
rm gcc-7.4.0.tar.xz  
mkdir build-gcc  
cd build-gcc  
../gcc-7.4.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers  
make all-gcc  
make all-target-libgcc  
make install-gcc  
make install-target-libgcc
```

6. Set gcc env variable

```
export PATH=$HOME/opt/cross/bin:$PATH
```

After that we should probably be able to compile the code using
 `i686-elf-gcc kernle.c -o kernel.o`
 
to run the OS use: 
```
make
qemu-system-i386 -cdrom tzos.iso
```

Sources found here https://medium.com/@lduck11007/operating-systems-development-for-dummies-3d4d786e8ac


