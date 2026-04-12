cmake_minimum_required(VERSION 4.2)
project(midi_rig)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add dependencies
include(cmake/CPM.cmake)

# to avoid namespace collisions in rtmidi and rtaudio
# BUILD_TESTING OFF: suppresses the "apinames" test executable in each package.
# TARGETNAME_UNINSTALL: renames each package's "uninstall"
CPMAddPackage(
        URI "gh:thestk/rtmidi#6.0.0"
        OPTIONS
        "RTMIDI_BUILD_TESTING OFF"
        "RTMIDI_TARGETNAME_UNINSTALL rtmidi_uninstall")

CPMAddPackage(
        URI "gh:thestk/rtaudio#6.0.1"
        OPTIONS
        "RTAUDIO_BUILD_TESTING OFF"
        "RTAUDIO_TARGETNAME_UNINSTALL rtaudio_uninstall")

# Link creates an IMPORTED target inside a function
# Download only and include the config directly

CPMAddPackage(
        URI "gh:ableton/link#Link-3.1.5"
        DOWNLOAD_ONLY YES)
if(link_ADDED)
    include(${link_SOURCE_DIR}/AbletonLinkConfig.cmake)
else()
    message(FATAL_ERROR "Failed to fetch Ableton Link")
endif()

add_executable(midi_rig
        src/main.cpp
        src/midi/MidiEvent.cpp
        src/midi/MidiEvent.h)

target_include_directories(midi_rig PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(midi_rig PRIVATE
        rtaudio
        rtmidi
        Ableton::Link)
