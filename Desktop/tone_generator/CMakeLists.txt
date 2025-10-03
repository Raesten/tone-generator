cmake_minimum_required(VERSION 3.22)

project(ToneGenerator VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE modules
add_subdirectory(JUCE-8.0.10)

# Create the main application
juce_add_gui_app(ToneGenerator
    PRODUCT_NAME "Tone Generator"
    VERSION ${PROJECT_VERSION}
    COMPANY_NAME "Your Company"
    BUNDLE_ID "com.yourcompany.tonegenerator"
    JUCE_OPTIONS
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="Tone Generator"
        JUCE_APPLICATION_VERSION_STRING="1.0.0"
)

# Generate JUCE header
juce_generate_juce_header(ToneGenerator)

# Add source files
target_sources(ToneGenerator PRIVATE
    Source/Main.cpp
    Source/MainComponent.mm
    Source/Synth.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/CustomLookAndFeel.cpp
    Source/WrattDelay.cpp
    # STK library files
    Source/STK/Stk.cpp
    Source/STK/SineWave.cpp
    Source/STK/BlitSquare.cpp
    Source/STK/BlitSaw.cpp
    Source/STK/Noise.cpp
    Source/STK/ADSR.cpp
    Source/STK/FreeVerb.cpp
    Source/STK/Chorus.cpp
)

# Link JUCE modules
target_link_libraries(ToneGenerator PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)