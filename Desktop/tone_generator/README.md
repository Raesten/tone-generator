# Tone Generator - JUCE Cross-Platform Audio Application

A simple tone generator application built with JUCE that can generate sine, square, sawtooth, and triangle waves with adjustable frequency and amplitude.

## Features

- **Multiple Wave Types**: Sine, Square, Sawtooth, and Triangle waves
- **Adjustable Frequency**: 20Hz to 20kHz range
- **Volume Control**: 0% to 100% amplitude
- **Real-time Audio**: Low-latency audio processing
- **Cross-Platform**: Supports iOS and Android

## Project Structure

```
tone_generator/
├── Source/                 # Source code files
│   ├── Main.cpp           # Application entry point
│   ├── MainComponent.h    # Main UI component header
│   ├── MainComponent.cpp  # Main UI component implementation
│   ├── ToneGenerator.h    # Audio generator header
│   └── ToneGenerator.cpp  # Audio generator implementation
├── Assets/                # App assets (icons, etc.)
├── JUCE-8.0.10/          # JUCE framework
├── CMakeLists.txt        # CMake configuration
├── build_ios.sh          # iOS build script
├── build_android.sh      # Android build script
└── README.md             # This file
```

## Prerequisites

- **JUCE 8.0.10** (included)
- **Xcode** (for iOS development)
- **Android Studio** with NDK and CMake
- **CMake 3.22+**

## Building the Project

### iOS Build

```bash
./build_ios.sh
```

This will:
1. Create an Xcode project configured for iOS
2. Build the app for iOS devices
3. Create an archive ready for distribution

### Android Build

```bash
./build_android.sh
```

This will:
1. Configure CMake for Android
2. Build the native Android app
3. Generate an APK file

### Manual CMake Build

For more control, you can use CMake directly:

```bash
# iOS
mkdir build-ios && cd build-ios
cmake -G Xcode -DCMAKE_SYSTEM_NAME=iOS ../

# Android
mkdir build-android && cd build-android
cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=$ANDROID_NDK_HOME ../
```

## Using Projucer (Alternative)

If you prefer the GUI approach:

1. **Launch Projucer**: Open `JUCE-8.0.10/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app`
2. **Create New Project**: Choose "Audio Application" template
3. **Configure Paths**: Go to File > Global Paths and set:
   - Android SDK: `~/Library/Android/sdk`
   - Android NDK: `~/Library/Android/sdk/ndk/25.2.9519653`
4. **Add Exporters**: Add both Android and iOS exporters
5. **Generate Projects**: Use the platform buttons to generate Xcode/Android Studio projects

## Development

### Adding New Features

1. **Audio Processing**: Modify `ToneGenerator.cpp` for audio generation
2. **UI Components**: Update `MainComponent.cpp` for interface changes
3. **New Modules**: Add JUCE modules in `CMakeLists.txt`

### JUCE Modules Used

- `juce_audio_basics` - Core audio functionality
- `juce_audio_devices` - Audio device management
- `juce_audio_formats` - Audio file format support
- `juce_audio_processors` - Audio processing framework
- `juce_audio_utils` - Audio utility classes
- `juce_core` - Core JUCE functionality
- `juce_dsp` - Digital signal processing
- `juce_gui_basics` - Basic GUI components
- `juce_events` - Event handling

## Troubleshooting

### iOS Issues
- Ensure Xcode is properly installed
- Check iOS deployment target compatibility
- Verify code signing settings

### Android Issues
- Verify Android SDK and NDK paths
- Check CMake version compatibility
- Ensure proper API level settings

### Build Issues
- Clean build directories: `rm -rf build/`
- Check CMake configuration
- Verify JUCE module paths

## License

This project uses JUCE, which is licensed under the GPL v3. See the JUCE license for details.
