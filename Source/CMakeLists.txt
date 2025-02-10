# SPDX-License-Identifier: GPL-2.0-only
# Copyright © Interactive Echoes. All rights reserved.
# Author: mozahzah

cmake_minimum_required(VERSION 3.20)

file(GLOB SOURCE_FILES "./*.cpp")
add_library(LIEMidi STATIC ${SOURCE_FILES})
target_include_directories(LIEMidi PUBLIC "./")
file(GLOB HEADER_FILES "./*.h")
set_property(TARGET LIEMidi PROPERTY PUBLIC_HEADER ${HEADER_FILES})
target_link_libraries(LIEMidi PUBLIC IECore)
target_link_libraries(LIEMidi PUBLIC IEActions)
target_link_libraries(LIEMidi PUBLIC rtmidi)
target_link_libraries(LIEMidi PUBLIC ryml)

set_target_properties(LIEMidi PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

message("Installation prefix: ${CMAKE_INSTALL_PREFIX}")
install(TARGETS LIEMidi
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(InstallRequiredSystemLibraries)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/Resources" DESTINATION ".")

get_target_property(DEPENDENCIES LIEMidi LINK_LIBRARIES)
foreach(DEPENDENCY ${DEPENDENCIES})
  if(TARGET ${DEPENDENCY})
    message("Setting up dependency: ${DEPENDENCY}")

    set_target_properties(${DEPENDENCY} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

    install(TARGETS ${DEPENDENCY}
        EXPORT "LIEMidiTargets"
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  endif()
endforeach()
