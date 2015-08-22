project(Pineapple)
cmake_minimum_required(VERSION 2.8)

### Version info

set(VERSION_NAME "Pineapple")
set(UPPERCASE_VERSION_NAME "PINEAPPLE")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)

### General compilation options
# Use C++14 for everything
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++1y")
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -pedantic -Wconversion -Wsign-conversion -Werror")
endif()

### wxWidgets

set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets REQUIRED core base net webview)
include(${wxWidgets_USE_FILE})

### Pineapple app

## General

set(DATAFILES
    data/loading.html
    data/blank.ipynb
    data/images/Pineapple.icns
    data/images/Pineapple-Doc.icns
    data/images/Copy-50.png
    data/images/Cut-50.png
    data/images/Delete-50.png
    data/images/Down-50.png
    data/images/Edit-50.png
    data/images/Fantasy-50.png
    data/images/FastForward-50.png
    data/images/Led-Blue-On-1632.png
    data/images/Led-Blue-Off-1632.png
    data/images/Paste-50.png
    data/images/Pen-50.png
    data/images/Play-50.png
    data/images/Plus-50.png
    data/images/Save-50.png
    data/images/SaveAs-50.png
    data/images/Stop-50.png
    data/images/Synchronize-50.png
    data/images/Up-50.png
)

set(SRC_LIST
    src/callback.cc
    src/gui_util.cc
    src/util.cc
    src/MainFrame.cc
    src/MainApp.cc
    src/RecentManager.cc
    ${DATAFILES}
)
configure_file("${PROJECT_SOURCE_DIR}/src/config.hh.in" "${PROJECT_BINARY_DIR}/include/config.hh")
include_directories("${PROJECT_BINARY_DIR}/include")
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

## Mac OS X specific information

if(APPLE)
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION_NAME}.${VERSION_MAJOR}.${VERSION_MINOR}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION_NAME}.${VERSION_MAJOR}.${VERSION_MINOR}")
    configure_file("${PROJECT_SOURCE_DIR}/cmake/Pineapple.plist.in"
        "${PROJECT_BINARY_DIR}/cmake/Pineapple.plist")
    set_source_files_properties(${DATAFILES} PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE 1
        MACOSX_BUNDLE_INFO_PLIST "${PROJECT_BINARY_DIR}/cmake/Pineapple.plist")
endif()

### Rules for virtualenv

set(VENV_DIR "${PROJECT_BINARY_DIR}/venv")
add_custom_command(
    OUTPUT "${VENV_DIR}/bin/activate"
    COMMAND virtualenv "${VENV_DIR}"
    COMMAND "${VENV_DIR}/bin/pip" install -r "${PROJECT_SOURCE_DIR}/requirements.txt"
    COMMAND virtualenv --relocatable "${VENV_DIR}"
    COMMENT "Setting up virtualenv"
)
add_custom_target(venv DEPENDS "${VENV_DIR}/bin/activate")
add_dependencies(${PROJECT_NAME} venv)

### Server startup script

file(COPY "scripts/pineapple-server" DESTINATION "${PROJECT_BINARY_DIR}/scripts/")
file(COPY "scripts/pineapple-server.spec" DESTINATION "${PROJECT_BINARY_DIR}/scripts/")

### Resources

add_custom_target(notebook_install
    DEPENDS venv
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/custom" "${VENV_DIR}/lib/python2.7/site-packages/notebook/static/custom/"
)

### Testing targets

add_custom_target(localtest
    DEPENDS ${PROJECT_NAME} venv notebook_install
    COMMAND ${CMAKE_COMMAND} -E env "PINEAPPLE_SERVER=venv/bin/python --version" "bin/${PROJECT_NAME}"
)
set_target_properties(localtest PROPERTIES
    EXCLUDE_FROM_ALL 1)

### Server targets

add_custom_target(server
    DEPENDS venv notebook_install
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
    COMMAND "${VENV_DIR}/bin/pyinstaller" -y "${PROJECT_BINARY_DIR}/scripts/pineapple-server.spec"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/dist/pineapple-server/tcl"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/dist/pineapple-server/tk"
)

### Install

if(APPLE)
    install (TARGETS ${PROJECT_NAME}
        DESTINATION ".")
else()
    install (TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION data
        COMPONENT applications)
    install (FILES ${DATAFILES}
        DESTINATION "share/${VERSION_NAME}")
    install (DIRECTORY ${PROJECT_BINARY_DIR}/dist/pineapple-server/
        DESTINATION "share/${VERSION_NAME}/pineapple-server")
endif()

### CPACK

include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VENDOR "Nathan Whitehead")
set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set (CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set (CPACK_PACKAGE_EXECUTABLES "Pineapple" "Pineapple")

if(APPLE)
else()
    set (CPACK_GENERATOR "ZIP")
endif()

include (CPack)
