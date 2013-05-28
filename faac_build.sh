#faac版本是：1.28


export ANDROID_NDK=/home/zhoujian/Android/NDK/android-ndk-r8d/
export TOOLCHAIN=/home/zhoujian/Downloads/toolchain
export SYSROOT=$TOOLCHAIN/sysroot/

rm -rf $TOOLCHAIN
$ANDROID_NDK/build/tools/make-standalone-toolchain.sh --platform=android-9 --install-dir=$TOOLCHAIN

ANDROID_BIN=$ANDROID_NDK/toolchains/arm-linux-androideabi-4.4.3/prebuilt/darwin-x86/bin/
CROSS_COMPILE=${ANDROID_BIN}/arm-linux-androideabi-

export PATH=$TOOLCHAIN/bin:$PATH
export CC=arm-linux-androideabi-gcc
export LD=arm-linux-androideabi-ld
export AR=arm-linux-androideabi-ar
export NM=arm-linux-androideabi-nm
export STRIP=arm-linux-androideabi-strip
export RANLIB=arm-linux-androideabi-ranlib
export CXX=arm-linux-androideabi-g++

export ARM_INC=$SYSROOT/usr/include
export ARM_LIB=$SYSROOT/usr/lib
 
CFLAGS=" -I$ARM_INC -fpic -DANDROID -fpic -mthumb-interwork -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__  -Wno-psabi -march=armv5te -mtune=xscale -msoft-float -mthumb -Os -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -DANDROID  -Wa,--noexecstack -MMD -MP "
LDFLAGS=" -nostdlib -Bdynamic -Wl,--whole-archive -Wl,--no-undefined -Wl,-z,noexecstack  -Wl,-z,nocopyreloc -Wl,-soname,/system/lib/libz.so -Wl,-rpath-link=$ARM_LIB,-dynamic-linker=/system/bin/linker -L$ANDROID_NDK/sources/cxx-stl/gnu-libstdc++/libs/armeabi -L$ANDROID_NDK/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/arm-linux-androideabi/lib -L$ARM_LIB  -lc -lgcc -lm -ldl  "
FLAGS="--host=arm-androideabi-linux --enable-static --disable-shared --prefix=/home/zhoujian/mylibs/android/faac --enable-armv5e  "
 
export CPPFLAGS="$CFLAGS"
export CFLAGS="$CFLAGS"
export CXXFLAGS="$CFLAGS"


cd $1
./configure $FLAGS
make
make install
