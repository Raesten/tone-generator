# Audio Diagnostic Guide for JUCE Tone Generator

## Issues Fixed

I've identified and fixed several critical issues that were likely causing silence in your JUCE app:

### 1. **Missing `setAudioChannels()` call for non-iOS platforms** ✅ FIXED
- **Problem**: The `initialiseAudioSafely()` method only called `setAudioChannels(0, 2)` on iOS, but not on macOS/Windows/Linux
- **Fix**: Added immediate `setAudioChannels(0, 2)` call for non-iOS platforms with proper error checking

### 2. **Synthesizer not being prepared** ✅ FIXED
- **Problem**: The `padSynthesizer.prepare(sampleRate, samplesPerBlockExpected)` call was missing
- **Fix**: Added the critical `padSynthesizer.prepare()` call in `prepareToPlay()`

### 3. **Insufficient audio debugging** ✅ FIXED
- **Problem**: Limited logging made it hard to diagnose audio issues
- **Fix**: Added comprehensive logging throughout the audio pipeline

### 4. **No test tone for debugging** ✅ FIXED
- **Problem**: No way to test if audio system is working independently of synthesizer
- **Fix**: Added test tone functionality with keyboard shortcuts

## How to Test the Fixes

### 1. **Build and Run**
```bash
cd /Users/magnusnorregaardandersen/Desktop/tone_generator
# Build for your platform (macOS, iOS, etc.)
```

### 2. **Check Console Output**
Look for these debug messages in the console:
```
initialiseAudioSafely() called
Non-iOS platform: Calling setAudioChannels(0, 2) immediately
Audio device opened successfully: [Device Name], Sample Rate: [Rate] Hz, Buffer Size: [Size]
prepareToPlay called - Sample Rate: [Rate] Hz, Block Size: [Size]
PadSynthesizer prepared with sample rate: [Rate] Hz, block size: [Size]
Audio prepared successfully with sample rate: [Rate] Hz, block size: [Size]
```

### 3. **Test Audio System**
- **Press 'T' key**: Toggle test tone (440 Hz sine wave)
- **Press 'D' key**: Show audio device information
- **Click note buttons**: Should trigger synthesizer sounds
- **Check volume sliders**: Ensure they're not at zero

### 4. **Common Issues to Check**

#### macOS Specific:
- **Microphone Permission**: If you see permission dialogs, deny microphone access (you only need output)
- **Audio Device**: Check System Preferences → Sound → Output to ensure a valid device is selected
- **Volume**: Check system volume and app volume

#### iOS Specific:
- **Simulator Audio**: Go to I/O → Audio Output in Simulator and select a real device
- **Silent Switch**: Ensure the device isn't in silent mode
- **AirPlay/Bluetooth**: Disconnect any external audio devices temporarily

#### General:
- **Console Errors**: Look for any error messages in the console
- **Audio Device Info**: Press 'D' to see detailed audio device information
- **Test Tone**: Press 'T' to hear a simple 440 Hz tone (bypasses synthesizer)

## Debugging Steps

### Step 1: Verify Audio Device
1. Run the app
2. Press 'D' key to show audio device info
3. Verify you see a valid device name and sample rate > 0

### Step 2: Test Basic Audio
1. Press 'T' key to enable test tone
2. You should hear a 440 Hz sine wave
3. If you don't hear anything, check system volume and audio device

### Step 3: Test Synthesizer
1. Press 'T' again to disable test tone
2. Click on note buttons (C, C#, D, etc.)
3. You should hear synthesizer sounds
4. Try the chord buttons (C Major, C Minor, etc.)

### Step 4: Check Volume Controls
1. Move the Master volume slider
2. Move the Left/Right volume sliders
3. Ensure none are at zero

## Expected Console Output

When working correctly, you should see:
```
initialiseAudioSafely() called
Non-iOS platform: Calling setAudioChannels(0, 2) immediately
Audio device opened successfully: MacBook Pro Speakers, Sample Rate: 48000 Hz, Buffer Size: 512
prepareToPlay called - Sample Rate: 48000 Hz, Block Size: 512
PadSynthesizer prepared with sample rate: 48000 Hz, block size: 512
Audio prepared successfully with sample rate: 48000 Hz, block size: 512
Audio rendering active - Sample rate: 48000 Hz, Block size: 512, Master gain: 0.8
```

## If Still No Sound

### Check These Common Issues:

1. **System Volume**: Ensure system volume is up and not muted
2. **App Volume**: Check the Master volume slider in the app
3. **Audio Device**: Press 'D' to verify audio device info
4. **Test Tone**: Press 'T' to test basic audio output
5. **Console Errors**: Look for any error messages
6. **Platform Specific**: Check macOS Sound preferences or iOS Simulator I/O settings

### Additional Debugging:

1. **Enable Test Tone**: Press 'T' key to bypass synthesizer and test raw audio
2. **Check Device Info**: Press 'D' key to see detailed audio device information
3. **Console Logging**: Watch for "Audio rendering active" messages every 1000 audio blocks
4. **Volume Sliders**: Ensure Master, Left, and Right sliders are not at zero

## Platform-Specific Notes

### macOS:
- Uses Core Audio for audio output
- Check System Preferences → Sound → Output
- May prompt for microphone permission (deny it, you only need output)

### iOS:
- Uses AVAudioSession for audio management
- Check Simulator I/O → Audio Output settings
- Real devices rarely have audio issues

### Windows:
- Uses DirectSound or WASAPI
- Check Windows Sound settings
- Ensure audio drivers are working

The fixes I've implemented should resolve the most common causes of silence in JUCE audio apps. The test tone feature will help you quickly determine if the issue is with the audio system or the synthesizer specifically.
