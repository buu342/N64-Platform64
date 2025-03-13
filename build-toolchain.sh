#!/bin/bash

# Build Binutils
cd Tools/Binutils
mkdir build-binutils
cd build-binutils
../configure \
  --target=mips32-elf --prefix=$(pwd)/../../CompilerSuite \
  --program-prefix=n64- --with-cpu=vr4300 \
  --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install

# Build GCC
cd ../../GCC/
mkdir build-gcc; cd build-gcc
../configure \
  --target=mips32-elf --prefix=$(pwd)/../../CompilerSuite \
  --program-prefix=n64- --with-arch=vr4300 \
  --with-languages=c,c++ --disable-threads \
  --disable-nls --without-headers --with-newlib
make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
sudo make install-gcc
sudo make install-target-libgcc