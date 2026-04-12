cmake_minimum_required(VERSION 4.2)
project(midi_rig)

set(CMAKE_CXX_STANDARD 23)

add_executable(midi_rig main.cpp)

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

CPMAddPackage(
        URI "gh:ableton/link#Link-3.1.5"
        OPTIONS "LINK_BUILD_TESTS OFF")
