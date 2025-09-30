#!/bin/bash

# Android Build Script for Tone Generator
echo "Building Tone Generator for Android..."

# Set Android environment variables
export ANDROID_HOME=$HOME/Library/Android/sdk
export ANDROID_NDK_HOME=$ANDROID_HOME/ndk/25.2.9519653
export PATH=$PATH:$ANDROID_HOME/cmake/3.22.1/bin

# Create build directory
mkdir -p build/android
cd build/android

# Configure CMake for Android
cmake -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a \
    -DCMAKE_ANDROID_NDK=$ANDROID_NDK_HOME \
    -DCMAKE_ANDROID_STL_TYPE=c++_static \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_ANDROID_API_MIN=21 \
    -DCMAKE_ANDROID_API=33 \
    -DANDROID_PLATFORM=android-33 \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    ../../

# Build the project
make -j$(sysctl -n hw.ncpu)

echo "Android build completed! APK should be available in build/android/"
