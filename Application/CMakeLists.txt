# SPDX-License-Identifier: GPL-2.0-only
# Copyright © Interactive Echoes. All rights reserved.
# Author: mozahzah

cmake_minimum_required(VERSION 3.20)

if(WIN32)
  add_executable(${PROJECT_NAME} WIN32 "./main.cpp")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
elseif(APPLE)
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE "./main.cpp")
elseif(LINUX)
  add_executable(${PROJECT_NAME} "./main.cpp")
endif()

message("Linking IEMidi App with LIEMidi")
target_link_libraries(${PROJECT_NAME} PUBLIC LIEMidi)

message("Setting up install")
message("Installation prefix: ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${PROJECT_NAME} 
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
include(InstallRequiredSystemLibraries)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/Resources" DESTINATION "IE")

begin_section_message("Setting packaging settings for IEMidi")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Interactive Echoes")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_SOURCE_DIR}/package")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Midi Map Editor Tool")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_VERBATIM_VARIABLES TRUE)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(IE_LOGO_ICON_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/IECore/Resources/IE-Brand-Kit/IE-Logo-NoBg.png")
set(CPACK_PACKAGE_ICON "${IE_LOGO_ICON_PATH}")

if(WIN32)
  set(CPACK_GENERATOR NSIS64)
  set(IE_LOGO_ICON_PATH_ICO "${CMAKE_SOURCE_DIR}\\ThirdParty\\IECore\\Resources\\IE-Brand-Kit\\IE-Logo-NoBg.ico")
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\ThirdParty\\IECore\\Resources\\IE-Brand-Kit\\IE-Logo-NoBg.png")
  set(CPACK_NSIS_MUI_ICON "${IE_LOGO_ICON_PATH_ICO}")
  set(CPACK_NSIS_MUI_UNIICON "${IE_LOGO_ICON_PATH_ICO}")
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL TRUE)
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${CPACK_PACKAGE_NAME}.exe")
  set(CPACK_NSIS_DEFINES "RequestExecutionLevel admin")
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    CreateShortCut \"$DESKTOP\\${CPACK_PACKAGE_NAME}.lnk\" \"$INSTDIR\\bin\\${CPACK_PACKAGE_NAME}.exe\" \"\" \"$INSTDIR\\IE\\Resources\\IE-Brand-Kit\\IE-Logo-NoBg.ico\" 0
    CreateShortCut \"$SMPROGRAMS\\${CPACK_PACKAGE_NAME}.lnk\" \"$INSTDIR\\bin\\${CPACK_PACKAGE_NAME}.exe\" \"\" \"$INSTDIR\\IE\\Resources\\IE-Brand-Kit\\IE-Logo-NoBg.ico\" 0")
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    Delete \"$DESKTOP\\${CPACK_PACKAGE_NAME}.lnk\"
    Delete \"$SMPROGRAMS\\${CPACK_PACKAGE_NAME}.lnk\"")
elseif(APPLE)
  set(CPACK_GENERATOR DragNDrop)
  set(CPACK_BUNDLE_NAME ${PROJECT_NAME})
  set(CPACK_BUNDLE_PLIST "${CMAKE_SOURCE_DIR}/info.plist")
  set(CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/ThirdParty/IECore/Resources/IE-Brand-Kit/IE-Logo-NoBg-512.icns")
  set(CPACK_COMPONENTS_ALL Bundle)
elseif(LINUX)
  install(FILES "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.desktop" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")
  install(FILES "${IE_LOGO_ICON_PATH}" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps")
  if(EXISTS "/etc/debian_version")
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
    set(CPACK_DEBIAN_PACKAGE_LICENSE "GPL-2.0-only")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "IE")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS TRUE)
  elseif(EXISTS "/etc/redhat-release")
    set(CPACK_GENERATOR "RPM")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Audio")
    set(CPACK_RPM_PACKAGE_LICENSE "GPL-2.0-only")
    set(CPACK_RPM_PACKAGE_AUTOREQ TRUE)
  else()
    message(WARNING "Unsupported Linux distribution - package might not build correctly")
  endif()
endif()
include(CPack)