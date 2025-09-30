# Install script for directory: /Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules

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
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_analytics")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_animation")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_audio_basics")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_audio_devices")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_audio_formats")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_audio_plugin_client")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_audio_processors")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_audio_utils")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_box2d")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_core")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_cryptography")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_data_structures")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_dsp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_events")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_graphics")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_gui_basics")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_gui_extra")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_javascript")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_midi_ci")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_opengl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_osc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_product_unlocking")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/JUCE-8.0.10/modules" TYPE DIRECTORY FILES "/Users/magnusnorregaardandersen/Desktop/tone_generator/JUCE-8.0.10/modules/juce_video")
endif()

