# SPDX-License-Identifier: GPL-2.0-only
# Copyright © Interactive Echoes. All rights reserved.
# Author: mozahzah

cmake_minimum_required(VERSION 3.20)

set(IEMidi_WIDGET_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/IEActionTypeDropDownWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEActionTypeDropDownWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiInputEditorWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiInputEditorWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiMessageEditorWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiMessageEditorWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiOutputEditorWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiOutputEditorWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiTypeDropDownWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiTypeDropDownWidget.h"
  PARENT_SCOPE
)