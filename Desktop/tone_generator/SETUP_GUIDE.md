# JUCE Cross-Platform Development Setup Guide

## ✅ Completed Steps

### 1. JUCE Framework Installation
- **Status**: ✅ COMPLETED
- **Version**: JUCE 8.0.10
- **Location**: `/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/`
- **Source**: Downloaded from official GitHub repository

### 2. iOS Development Setup
- **Status**: ✅ COMPLETED
- **Xcode Version**: 26.0.1 (Build 17A400)
- **iOS SDK Version**: 26.0
- **SDK Path**: `/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS26.0.sdk`
- **Command Line Tools**: ✅ Properly configured

## 🔄 Remaining Steps

### 3. Android Development Setup

#### 3.1 Install Android Studio
**Manual Installation Required:**

1. **Download Android Studio:**
   - Visit: https://developer.android.com/studio
   - Download the latest version for macOS (Apple Silicon recommended)
   - The file will be named something like `android-studio-2024.2.1.20-mac_arm64.dmg`

2. **Install Android Studio:**
   ```bash
   # Mount the DMG file
   hdiutil attach android-studio-2024.2.1.20-mac_arm64.dmg
   
   # Copy to Applications folder
   cp -R "/Volumes/Android Studio/Android Studio.app" /Applications/
   
   # Unmount the DMG
   hdiutil detach "/Volumes/Android Studio"
   ```

3. **Launch Android Studio:**
   - Open Android Studio from Applications folder
   - Follow the setup wizard
   - Accept license agreements
   - Choose "Standard" installation type

#### 3.2 Install Android SDK, NDK, and CMake

1. **Open SDK Manager:**
   - In Android Studio: `Tools` → `SDK Manager`
   - Or: `Android Studio` → `Preferences` → `Appearance & Behavior` → `System Settings` → `Android SDK`

2. **Install SDK Platforms:**
   - Go to `SDK Platforms` tab
   - Select latest Android versions (API 34, 35 recommended)
   - Click `Apply` to install

3. **Install SDK Tools:**
   - Go to `SDK Tools` tab
   - Check the following:
     - ✅ **NDK (Side by side)** - Latest version
     - ✅ **CMake** - Latest version
     - ✅ **Android SDK Build-Tools** - Latest version
     - ✅ **Android SDK Platform-Tools** - Latest version
   - Click `Apply` to install

4. **Set Environment Variables:**
   Add to your `~/.zshrc` file:
   ```bash
   export ANDROID_HOME=$HOME/Library/Android/sdk
   export PATH=$PATH:$ANDROID_HOME/emulator
   export PATH=$PATH:$ANDROID_HOME/platform-tools
   export PATH=$PATH:$ANDROID_HOME/cmdline-tools/latest/bin
   export PATH=$PATH:$ANDROID_HOME/ndk/25.2.9519653
   ```

5. **Reload shell configuration:**
   ```bash
   source ~/.zshrc
   ```

### 4. Verify Installation

After completing the Android setup, run these commands to verify:

```bash
# Check Android SDK
adb version

# Check NDK
ls $ANDROID_HOME/ndk/

# Check CMake
cmake --version

# Check JUCE
ls JUCE-8.0.10/
```

## 🎯 Next Steps

Once all tools are installed:

1. **Create a JUCE Project:**
   - Use Projucer (included with JUCE) to create a new project
   - Configure for both iOS and Android targets

2. **Build and Test:**
   - Build for iOS using Xcode
   - Build for Android using Android Studio or command line

## 📁 Project Structure

Your current project structure:
```
tone_generator/
├── JUCE-8.0.10/          # JUCE framework
│   ├── modules/          # JUCE modules
│   ├── examples/         # Example projects
│   ├── extras/           # Additional tools
│   └── CMakeLists.txt    # CMake configuration
└── SETUP_GUIDE.md        # This guide
```

## 🔗 Useful Links

- [JUCE Documentation](https://docs.juce.com/)
- [JUCE GitHub Repository](https://github.com/juce-framework/JUCE)
- [Android Studio Download](https://developer.android.com/studio)
- [Android NDK Documentation](https://developer.android.com/ndk)
- [iOS Development Guide](https://developer.apple.com/ios/)

## ⚠️ Notes

- Make sure you have administrator privileges for some installations
- The Android SDK path may vary depending on installation method
- NDK version numbers may change with updates
- Always use the latest stable versions for best compatibility


