# SPDX-License-Identifier: GPL-2.0-only
# Copyright © Interactive Echoes. All rights reserved.
# Author: mozahzah

cmake_minimum_required(VERSION 3.20)

add_compile_definitions(Resources_Folder_Path="${CMAKE_SOURCE_DIR}/Resources")
set(CMAKE_AUTOMOC ON)
set(IEMidi_SOURCE_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiApp.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiApp.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiProcessor.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiProcessor.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiProfileManager.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiProfileManager.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiTypes.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiTypes.h"
)
add_subdirectory(IEWidgets)
add_library(LIEMidi STATIC ${IEMidi_SOURCE_FILES} ${IEMidi_WIDGET_FILES})
target_include_directories(LIEMidi PUBLIC "./")
set(IEMidi_HEADER_FILES "./IEMidi.h")
set_property(TARGET LIEMidi PROPERTY PUBLIC_HEADER ${IEMidi_HEADER_FILES})

message("Linking LIEMidi with required libraries")
target_link_libraries(LIEMidi PUBLIC IEActions)
target_link_libraries(LIEMidi PUBLIC rtmidi)
target_link_libraries(LIEMidi PUBLIC ryml)
target_link_libraries(LIEMidi PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets)
target_link_libraries(LIEMidi PUBLIC IELog)
target_link_libraries(LIEMidi PUBLIC IEConcurrency)
target_link_libraries(LIEMidi PUBLIC IEResources)

install(TARGETS LIEMidi
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
include(InstallRequiredSystemLibraries)