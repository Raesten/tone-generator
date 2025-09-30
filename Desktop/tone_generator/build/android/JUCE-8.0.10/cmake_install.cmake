# Install script for directory: /Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Users/magnusnorregaardandersen/Library/Android/sdk/ndk/25.2.9519653/toolchains/llvm/prebuilt/darwin-x86_64/bin/llvm-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/magnusnorregaardandersen/Desktop/tone_generator/build/android/JUCE-8.0.10/modules/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/magnusnorregaardandersen/Desktop/tone_generator/build/android/JUCE-8.0.10/extras/Build/cmake_install.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-8.0.10" TYPE FILE FILES
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/build/android/JUCE-8.0.10/JUCEConfigVersion.cmake"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/build/android/JUCE-8.0.10/JUCEConfig.cmake"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/JUCECheckAtomic.cmake"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/JUCEHelperTargets.cmake"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/JUCEModuleSupport.cmake"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/JUCEUtils.cmake"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/JuceLV2Defines.h.in"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/LaunchScreen.storyboard"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/PIPAudioProcessor.cpp.in"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/PIPAudioProcessorWithARA.cpp.in"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/PIPComponent.cpp.in"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/PIPConsole.cpp.in"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/RecentFilesMenuTemplate.nib"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/UnityPluginGUIScript.cs.in"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/checkBundleSigning.cmake"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/copyDir.cmake"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/juce_runtime_arch_detection.cpp"
    "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/extras/Build/CMake/juce_LinuxSubprocessHelper.cpp"
    )
endif()

