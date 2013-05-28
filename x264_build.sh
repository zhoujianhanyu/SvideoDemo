#x264库版本是2013-05-13---2245


export ANDROID_NDK=/home/zhoujian/Android/NDK/android-ndk-r8d/
export TOOLCHAIN=/home/zhoujian/Downloads/toolchain
export SYSROOT=$TOOLCHAIN/sysroot/

rm -rf $TOOLCHAIN
$ANDROID_NDK/build/tools/make-standalone-toolchain.sh --platform=android-9 --install-dir=$TOOLCHAIN

export PATH=$TOOLCHAIN/bin:$PATH
export CC=arm-linux-androideabi-gcc
export LD=arm-linux-androideabi-ld
export AR=arm-linux-androideabi-ar

X264_FLAGS="--prefix=/home/zhoujian/mylibs/android/x264 \
  	--enable-static \
	--enable-pic \
	--disable-asm \
	--disable-cli \
	--host=arm-linux \
	--cross-prefix=arm-linux-androideabi- "
 
  

cd $1
./configure $X264_FLAGS
make
make install
