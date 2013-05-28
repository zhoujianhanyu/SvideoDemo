#ffmpeg版本是：1.0.5


export ANDROID_NDK=/home/zhoujian/Android/NDK/android-ndk-r8d/
export TOOLCHAIN=/home/zhoujian/Android/ffmpeg/toolchain
export SYSROOT=$TOOLCHAIN/sysroot/

rm -rf $TOOLCHAIN
$ANDROID_NDK/build/tools/make-standalone-toolchain.sh --platform=android-9 --install-dir=$TOOLCHAIN

export PATH=$TOOLCHAIN/bin:$PATH
export CC=arm-linux-androideabi-gcc
export LD=arm-linux-androideabi-ld
export AR=arm-linux-androideabi-ar
export NM=arm-linux-androideabi-nm
export STRIP=arm-linux-androideabi-strip
export RANLIB=arm-linux-androideabi-ranlib
export CXX=arm-linux-androideabi-g++

MYINCLUDES="-I/home/zhoujian/mylibs/android/faac/include \
		 	 -I/home/zhoujian/mylibs/android/x264/include"
		 	 
MYLIBS="-L/home/zhoujian/mylibs/android/x264/lib \
		 -L/home/zhoujian/mylibs/android/faac/lib -lx264 -lfaac -lz"


CFLAGS="-O3 -Wall -mthumb -pipe -fpic -fasm \
  -finline-limit=300 -ffast-math \
  -fstrict-aliasing -Werror=strict-aliasing \
  -fmodulo-sched -fmodulo-sched-allow-regmoves \
  -Wno-psabi -Wa,--noexecstack \
  -D__ARM_ARCH_5__ -D__ARM_ARCH_5E__ \
  -D__ARM_ARCH_5T__ -D__ARM_ARCH_5TE__ \
  -DANDROID -DNDEBUG"

EXTRA_CFLAGS="-march=armv7-a -mfpu=neon -mfloat-abi=softfp -mvectorize-with-neon-quad $MYINCLUDES"
EXTRA_LDFLAGS="-Wl,--fix-cortex-a8 $MYLIBS"

FFMPEG_FLAGS="--prefix=/home/zhoujian/mylibs/android/ffmpeg_h_a \
	--target-os=linux \
  	--arch=arm \
  	--enable-cross-compile \
  	--cross-prefix=arm-linux-androideabi- \
  	--disable-shared \
  	--enable-static \
	--disable-stripping \
	--enable-nonfree \
	--enable-version3 \
	--enable-gpl \
	--disable-doc \
	--enable-avresample \
	--enable-demuxer=rtsp \
	--enable-muxer=rtsp \
	--disable-ffplay \
	--disable-ffserver \
	--disable-ffmpeg \
	--disable-ffprobe \
	--enable-libx264 \
	--enable-encoder=libx264 \
	--enable-decoder=h264 \
	--enable-encoder=h264 \
	--enable-protocol=rtp \
	--enable-libfaac \
	--enable-encoder=aac \
	--enable-hwaccels \
	--enable-zlib \
	--disable-devices \
	--disable-avdevice"


cd $1

./configure $FFMPEG_FLAGS --extra-cflags="$CFLAGS $EXTRA_CFLAGS" --extra-ldflags="$EXTRA_LDFLAGS"

make
make install

