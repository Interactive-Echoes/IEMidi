# SPDX-License-Identifier: GPL-2.0-only
# Copyright © Interactive Echoes. All rights reserved.
# Author: mozahzah

cmake_minimum_required(VERSION 3.20)
project(IEMidi VERSION 1.2.0 LANGUAGES CXX HOMEPAGE_URL "https://github.com/Interactive-Echoes/IEMidi")

# BEGIN misc function definitions

function(begin_section_message section_title)
  message("\n------------------------------------------------------------")
  message("${section_title}\n")
endfunction()

# END misc function definitions

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

begin_section_message("Compiler Setup")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
message("Using Compiler: ${CMAKE_CXX_COMPILER_ID}")
message("version: ${CMAKE_CXX_COMPILER_VERSION}")
message("CXX standard: ${CMAKE_CXX_STANDARD}")
begin_section_message("Working environment")
message("System name: ${CMAKE_SYSTEM_NAME}")
message("System version: ${CMAKE_SYSTEM_VERSION}")
message("System processor: ${CMAKE_SYSTEM_PROCESSOR}")
include(GNUInstallDirs)

message("\n------------------------------------------------------------")
message("Configuring ThirdParty Libraries")
message("\n------------------------------------------------------------")
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
add_subdirectory(ThirdParty/IELog)
add_subdirectory(ThirdParty/IEActions)
add_subdirectory(ThirdParty/IEConcurrency)
add_subdirectory(Resources/IEResources)
message("Configuring rtmidi")
add_subdirectory(ThirdParty/rtmidi)
message("Configuring ryml")
add_subdirectory(ThirdParty/rapidyaml)
message("\n------------------------------------------------------------")
add_subdirectory(Source)
add_subdirectory(Application)