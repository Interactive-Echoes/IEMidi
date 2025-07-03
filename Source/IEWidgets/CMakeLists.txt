# SPDX-License-Identifier: GPL-2.0-only
# Copyright © Interactive Echoes. All rights reserved.
# Author: mozahzah

cmake_minimum_required(VERSION 3.20)

set(IEMidi_WIDGET_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/IEFileBrowserWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEFileBrowserWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiActionTypeDropdownWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiActionTypeDropdownWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiInputEditorWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiInputEditorWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiLoggerTableWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiLoggerTableWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiMessageEditorWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiMessageEditorWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiMessageTypeDropDownWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiMessageTypeDropDownWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiOutputEditorWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiOutputEditorWidget.h"
  PARENT_SCOPE
)