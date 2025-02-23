# SPDX-License-Identifier: GPL-2.0-only
# Copyright © Interactive Echoes. All rights reserved.
# Author: mozahzah

cmake_minimum_required(VERSION 3.20)

file(GLOB IEMidi_SOURCE_FILES "./*.cpp")
add_library(LIEMidi STATIC ${IEMidi_SOURCE_FILES})
target_include_directories(LIEMidi PUBLIC "./")
file(GLOB IEMidi_HEADER_FILES "./*.h")
set_property(TARGET LIEMidi PROPERTY PUBLIC_HEADER ${IEMidi_HEADER_FILES})

message("Linking LIEMidi with required libraries")
target_link_libraries(LIEMidi PUBLIC IECore)
target_link_libraries(LIEMidi PUBLIC IEActions)
target_link_libraries(LIEMidi PUBLIC rtmidi)
target_link_libraries(LIEMidi PUBLIC ryml)

install(TARGETS LIEMidi
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
include(InstallRequiredSystemLibraries)