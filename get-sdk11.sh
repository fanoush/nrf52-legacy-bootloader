#!/bin/bash
if [ ! -d SDK11 ] ; then
[ -f nRF5_SDK_11.0.0_*.zip ] || wget https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v11.x.x/nRF5_SDK_11.0.0_89a8197.zip || exit 0
mkdir SDK11
pushd SDK11
unzip ../nRF5_SDK_11.0.0_*.zip || exit 0
rm *.msi
find . \( -name '*.ld' -o -name '*.c' -o -name '*.h' -o -name '*Makefile*' -o -name '*.txt' -o -name '*.hex' -o -name '*.s' -o -name '*.S' \) -print0 | xargs -0 dos2unix
if [ -d $HOME/gcc-arm-none-eabi-8-* ] ; then
# Makefile.posix is included from SDK examples Makefile so fix that one
printf "GNU_INSTALL_ROOT := %s\nGNU_VERSION := %s\nGNU_PREFIX := %s\n" $HOME/gcc-arm-none-eabi-8-* 8.3.1 arm-none-eabi >components/toolchain/gcc/Makefile.posix
fi
patch -p1 <../sdk11.patch
popd
fi
