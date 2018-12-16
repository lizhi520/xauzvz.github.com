#!/bin/sh

BUILDPATH=build_android
WORK_PATH=$(cd `dirname $0`; pwd)

function prepare {
    echo "begin check if build path exist..."
    if test -d $BUILDPATH
    then
        echo "build path exist, should remove and then rebuild"
        rm -rf $BUILDPATH
        mkdir -p $BUILDPATH
    else 
        echo "build path not exist, should create.."
        mkdir -p $BUILDPATH
    fi
}

prepare

#ndk settings
API=22
NDK_ENV=darwin
NDK_ROOTPATH=/Users/luolongzhi/Software/android-ndk-for-mac
#NDK_ENV=linux
#NDK_ROOTPATH=/data/Software/android-ndk-for-ubuntu

#arm/armeabi-v7a/armv7-a  aarch64/arm64-v8a/armv8-a x86/x86/i686
#arm/aarch64/x86
ARCH=arm

#arm arm64 mips mips64 x86 x86_64
NDK=$NDK_ROOTPATH/android-ndk-r15c
SYSROOT=$NDK/platforms/android-$API/arch-$ARCH/
#SYSROOT = $(NDK)/sysroot
ADDI_CFLAGS=-marm
#ADDI_CFLAGS=-marm -isystem $(NDK)/sysroot/usr/include/$(TRIPLE) -D__ANDROID_API__=$(API)
#ADDI_CFLAGS=-marm -isystem $(NDK)/sysroot/usr/include/$(TRIPLE)
#ADDI_LDFLAGS=-L$SYSROOT/usr/lib/ -lm -llog

#armeabi-v7a/arm64-v8a/x86
ARCH_NAME=armeabi-v7a

#armv7-a/armv8-a/i686
CPU=armv7-a

#arm-linux-androideabi/aarch64-linux-android-4.9/x86-4.9
TRIPLE=arm-linux-androideabi

TOOLCHAIN=$NDK/toolchains/$TRIPLE-4.9/prebuilt/$NDK_ENV-x86_64

#arm-linux-androideabi/aarch64-linux-android/i686-linux-android
GCC_PREFIX=arm-linux-androideabi

CROSS_PREFIX=$TOOLCHAIN/bin/$GCC_PREFIX

./configure \
    CFLAGS="--sysroot=$SYSROOT" \
    CPPFLAGS="--sysroot=$SYSROOT" \
    --prefix=$WORK_PATH/dist/xaudiopro/$ARCH_NAME \
    --host=$GCC_PREFIX \
    --build=x86_64 \
    --with-sysroot=$SYSROOT \
    #&& \
#make -j4
#make install

