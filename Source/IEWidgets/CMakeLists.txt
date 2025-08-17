# SPDX-License-Identifier: GPL-2.0-only
# Copyright © Interactive Echoes. All rights reserved.
# Author: mozahzah

cmake_minimum_required(VERSION 3.20)

set(IEMidi_WIDGET_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/IEDeletePropertyButton.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEDeletePropertyButton.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEDropdownItemDelegate.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEDropdownItemDelegate.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEFileBrowserWidget.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEFileBrowserWidget.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiActionTypeDropdown.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiActionTypeDropdown.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiDeviceInfo.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiDeviceInfo.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiDeviceInputPropertyEditor.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiDeviceInputPropertyEditor.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiDeviceOutputPropertyEditor.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiDeviceOutputPropertyEditor.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiLogger.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiLogger.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiMessageEditor.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiMessageEditor.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiMessageTypeDropdown.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IEMidiMessageTypeDropdown.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/IERecordButton.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/IERecordButton.h"
  PARENT_SCOPE
)