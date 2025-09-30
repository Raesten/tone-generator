#!/bin/bash

# iOS Build Script for Tone Generator
echo "Building Tone Generator for iOS..."

# Create build directory
mkdir -p build/ios
cd build/ios

# Configure CMake for iOS
cmake -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="" \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="iPhone Developer" \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE=Automatic \
    -DCMAKE_XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER="" \
    -DCMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET=12.0 \
    -DCMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY="1,2" \
    -DCMAKE_XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS="iphoneos iphonesimulator" \
    -DCMAKE_XCODE_ATTRIBUTE_ARCHS="arm64" \
    -DCMAKE_XCODE_ATTRIBUTE_VALID_ARCHS="arm64" \
    -DCMAKE_XCODE_ATTRIBUTE_ARCHS[sdk=iphonesimulator*]="arm64" \
    -DCMAKE_XCODE_ATTRIBUTE_VALID_ARCHS[sdk=iphonesimulator*]="arm64" \
    -DCMAKE_XCODE_ATTRIBUTE_ARCHS[sdk=iphoneos*]="arm64" \
    -DCMAKE_XCODE_ATTRIBUTE_VALID_ARCHS[sdk=iphoneos*]="arm64" \
    ../../

# Build the project
xcodebuild -project ToneGenerator.xcodeproj \
    -scheme ToneGenerator \
    -configuration Release \
    -destination 'generic/platform=iOS' \
    -archivePath ToneGenerator.xcarchive \
    archive

echo "iOS build completed! Archive created at build/ios/ToneGenerator.xcarchive"
