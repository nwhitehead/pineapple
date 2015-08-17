project(Pineapple)
cmake_minimum_required(VERSION 2.8)

### Version info

set(VERSION_NAME "Pineapple")
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
find_package(wxWidgets REQUIRED core base webview)
include(${wxWidgets_USE_FILE})

### Pineapple app

if(APPLE)
    set(ICONS 
        ${PROJECT_SOURCE_DIR}/images/Pineapple.icns
        ${PROJECT_SOURCE_DIR}/images/Pineapple-Doc.icns
    )
endif()

## General

set(SRC_LIST
    src/callback.cc
    src/gui_util.cc
    src/util.cc
    src/MainFrame.cc
    src/MainApp.cc
    src/RecentManager.cc
    ${ICONS}
)
configure_file("${PROJECT_SOURCE_DIR}/src/config.hh.in" "${PROJECT_BINARY_DIR}/include/config.hh")
include_directories("${PROJECT_BINARY_DIR}/include")
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

## Mac OS X specific information

if(APPLE)
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_NAME}.${VERSION_MAJOR}.${VERSION_MINOR}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME}.${VERSION_MAJOR}.${VERSION_MINOR}")
    configure_file("${PROJECT_SOURCE_DIR}/cmake/Pineapple.plist.in"
        "${PROJECT_BINARY_DIR}/cmake/Pineapple.plist")
    set_source_files_properties(${ICONS} PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${PROJECT_BINARY_DIR}/cmake/Pineapple.plist")
endif()

### Rules for virtualenv

set(VENV_DIR "${PROJECT_BINARY_DIR}/venv")
add_custom_command(
    OUTPUT "${VENV_DIR}/bin/activate"
    COMMAND virtualenv "${VENV_DIR}"
    COMMAND "${VENV_DIR}/bin/pip" install -r "${PROJECT_SOURCE_DIR}/requirements.txt"
    COMMENT "Setting up virtualenv"
)
add_custom_target(venv DEPENDS "${VENV_DIR}/bin/activate")
add_dependencies(${PROJECT_NAME} venv)

### Server startup script

file(COPY "scripts/pineapple-server" DESTINATION "${PROJECT_BINARY_DIR}/scripts/")
file(COPY "scripts/pineapple-server.spec" DESTINATION "${PROJECT_BINARY_DIR}/scripts/")

### Demo files

file(COPY "demo/" DESTINATION "${PROJECT_BINARY_DIR}/demo/")

### Resources

file(COPY "data/loading.html" DESTINATION "${PROJECT_BINARY_DIR}/data/")
file(COPY "data/blank.ipynb" DESTINATION "${PROJECT_BINARY_DIR}/data/")
file(COPY "images/" DESTINATION "${PROJECT_BINARY_DIR}/images/")
file(COPY "notebook/notebook/static/" DESTINATION "${PROJECT_BINARY_DIR}/notebook/notebook/static/")
file(COPY "notebook/notebook/templates/" DESTINATION "${PROJECT_BINARY_DIR}/notebook/notebook/templates/")

### Testing targets

set (ENV{PINEAPPLE_SERVER}
    "${VENV_DIR}/bin/python ${PROJECT_BINARY_DIR}/scripts/pineapple-server")

add_custom_target(localtest
    DEPENDS ${PROJECT_NAME} venv
    COMMAND "${PROJECT_NAME}"
)

### Production targets

add_custom_target(notebook_install
    DEPENDS venv
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/notebook"
    COMMAND "${VENV_DIR}/bin/python" "${PROJECT_SOURCE_DIR}/notebook/setup.py" install
)

add_custom_target(server
    DEPENDS venv notebook_install
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
    COMMAND "${VENV_DIR}/bin/pyinstaller" -y "${PROJECT_BINARY_DIR}/scripts/pineapple-server.spec"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/dist/pineapple-server/tcl"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/dist/pineapple-server/tk"
)

### Install

install (TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION data
    COMPONENT applications)
#install (FILES "${PROJECT_BINARY_DIR}/data" DESTINATION bin)

### CPACK

include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
include (CPack)

