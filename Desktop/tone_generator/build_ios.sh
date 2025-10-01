#!/bin/bash

# iOS Build Script for Tone Generator
echo "Building Tone Generator for iOS..."

# Navigate to the Projucer-generated iOS project
cd Builds/iOS

# Build the project using the existing Xcode project
xcodebuild -project "Tone Generator.xcodeproj" \
    -scheme "Tone Generator - App" \
    -configuration Release \
    -destination 'generic/platform=iOS' \
    -archivePath "Tone Generator.xcarchive" \
    archive

echo "iOS build completed! Archive created at Builds/iOS/Tone Generator.xcarchive"
