set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist") # install locally

project(Pineapple)
cmake_minimum_required(VERSION 2.8)

### General compilation options
# Use C++14 for everything
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++1y")
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -pedantic -Wconversion -Wsign-conversion -Werror")
endif()

#### Prerequisites section

# Turn on CCACHE
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

include(ExternalProject)

# External libraries location
set(EXTERNAL "${PROJECT_SOURCE_DIR}/external")

## OpenSSL

if(APPLE)
    add_custom_target(openssl) # nop
endif()
if(LINUX)
    set(OPENSSL_PREFIX "${PROJECT_BINARY_DIR}/openssl-prefix")
    set(OPENSSL_DIR "${PROJECT_BINARY_DIR}/openssl")
    set(OPENSSL_SOURCE "${OPENSSL_PREFIX}/src/openssl")
    ExternalProject_Add(
        openssl-external
        URL "${EXTERNAL}/openssl/openssl-1.0.2d.tar.gz"
        PREFIX "${OPENSSL_PREFIX}"
        CONFIGURE_COMMAND ./config --prefix=$ {OPENSSL_DIR} shared no-idea no-mdc2 no-rc5
                                            BUILD_COMMAND make
                                            BUILD_IN_SOURCE 1
                                            INSTALL_COMMAND make install
    )
    add_custom_target(openssl DEPENDS openssl-external)
## Python
endif()

set(PYTHON_CONFIGURE_OPTIONS
    --enable-ipv6
    --with-system-ffi
    --with-fpectl
    --with-threads
    no-ssl2
)
if(LINUX)
    # Ubuntu uses ucs4
    set(PYTHON_CONFIGURE_OPTIONS ${PYTHON_CONFIGURE_OPTIONS} --enable-unicode=ucs4)
endif()
if(APPLE)
    # Choose 64-bit build on Mac
    set(PYTHON_CONFIGURE_OPTIONS ${PYTHON_CONFIGURE_OPTIONS}
        darwin64-x86_64-cc
        --enable-unicode=ucs2
    )
endif()

set(PYTHON_OPENSSL "")
if(LINUX)
    set(PYTHON_OPENSSL "CPPFLAGS=-I${OPENSSL_DIR}" "LDFLAGS=-L${OPENSSL_DIR}/lib -L/usr/lib/x86_64-linux-gnu")
endif()

set(PYTHON_PREFIX "${PROJECT_BINARY_DIR}/python2.7-prefix")
set(PYTHON_DIR "${PROJECT_BINARY_DIR}/python2.7")
set(PYTHON_SOURCE "${PYTHON_PREFIX}/src/python2.7-external")
set(PYTHON "${PYTHON_DIR}/bin/python2.7")
ExternalProject_Add(
    python2.7-external
    URL "${EXTERNAL}/python-2.7/Python-2.7.10.tgz"
    PREFIX "${PYTHON_PREFIX}"
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL}/python-2.7/Setup.local ${PYTHON_SOURCE}/Modules/
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env ${PYTHON_OPENSSL} ./configure --prefix=${PYTHON_DIR} ${PYTHON_CONFIGURE_OPTIONS}
    BUILD_COMMAND make
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND make install
)
add_custom_target(python2.7 DEPENDS python2.7-external openssl)
add_custom_target(python2.7-postinstall
    DEPENDS python2.7
    COMMAND ${PYTHON} ${EXTERNAL}/python-2.7/get-pip.py
    COMMENT "Setting up pip"
)
set(PIP "${PYTHON_DIR}/bin/pip")


if(LINUX)

    ## libcurl

    set(CURL_PREFIX "${PROJECT_BINARY_DIR}/curl-prefix")
    set(CURL_DIR "${PROJECT_BINARY_DIR}/curl")
    ExternalProject_Add(
        libcurl-external
        URL "${EXTERNAL}/curl/curl-7.44.0.zip"
        PREFIX "${CURL_PREFIX}"
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env "CPPFLAGS=-I${OPENSSL_DIR}" "LDFLAGS=-L${OPENSSL_DIR}/lib" ./configure --with-ssl=${OPENSSL_DIR} --prefix=${CURL_DIR}
                BUILD_COMMAND make
                BUILD_IN_SOURCE 1
                INSTALL_COMMAND make install
    )
    add_custom_target(libcurl DEPENDS libcurl-external)

endif()
if(APPLE)
    add_custom_target(libcurl) # nop
endif()


## pycurl

if(LINUX)
    set(PYCURL_OPTS --curl-config=${CURL_DIR}/bin/curl-config --with-ssl)
endif()
if(APPLE)
    set(PYCURL_OPTS "")
endif()

set(PYCURL_PREFIX "${PROJECT_BINARY_DIR}/pycurl-prefix")
set(PYCURL_DIR "${PROJECT_BINARY_DIR}/pycurl")
set(PYCURL_SOURCE "${PYCURL_PREFIX}/src/pycurl-external")
ExternalProject_Add(
    pycurl-external
    DEPENDS python2.7 libcurl
    URL "${EXTERNAL}/pycurl/pycurl-7.19.5.tar.gz"
    PREFIX "${PYCURL_PREFIX}"
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE 1
    BUILD_COMMAND ${PYTHON} ${PYCURL_SOURCE}/setup.py install ${PYCURL_OPTS}
            INSTALL_COMMAND ""
)
add_custom_target(pycurl DEPENDS pycurl-external)


## wxWidgets

set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets REQUIRED core base net webview)
include(${wxWidgets_USE_FILE})

## PIP prerequisites

add_custom_target(requirements-install
    DEPENDS python2.7 python2.7-postinstall openssl libcurl pycurl
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${SSL_DIR}/lib:${CURL_DIR}/lib ${PIP} install -r ${PROJECT_SOURCE_DIR}/requirements.txt
)
# Server resources
add_custom_target(custom-reinstall
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/custom" "${PYTHON_DIR}/lib/python2.7/site-packages/notebook/static/custom/"
)
add_custom_target(custom-install DEPENDS requirements-install custom-reinstall)



#### PINEAPPLE

### Version info

set(VERSION_NAME "Pineapple")
set(UPPERCASE_VERSION_NAME "PINEAPPLE")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)

### Pineapple app

## General

set(EMPTY data/empty)
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
    data/examples/Basics.ipynb
    data/examples/Editing.ipynb
    data/examples/Execution.ipynb
    data/examples/Markdown.ipynb
    data/examples/Plotting.ipynb
    data/examples/RichOutput.ipynb
    data/examples/Timing.ipynb
)

set(SRC_LIST
    src/callback.cc
    src/gui_util.cc
    src/util.cc
    src/ExamplesFrame.cc
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

### Server startup script

file(COPY "scripts/pineapple-server" DESTINATION "${PROJECT_BINARY_DIR}/scripts/")
set(SERVER "${PROJECT_BINARY_DIR}/scripts/pineapple-server")

### Resources for testing

file(COPY ${DATAFILES} DESTINATION "${PROJECT_BINARY_DIR}")

### Testing targets

if (APPLE)
    set (TESTNAME "Pineapple.app/Contents/MacOS/Pineapple")
else()
    set (TESTNAME "${PROJECT_BINARY_DIR}/${PROJECT_NAME}")
endif()
add_custom_target(local-retest
    DEPENDS ${PROJECT_NAME} custom-reinstall bin/${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E env "PINEAPPLE_SERVER=${PYTHON} ${SERVER}" ${TESTNAME}
)
add_custom_target(local-test DEPENDS custom-install local-retest)
set_target_properties(local-test PROPERTIES EXCLUDE_FROM_ALL 1)
set_target_properties(local-retest PROPERTIES EXCLUDE_FROM_ALL 1)


### Install

file (GLOB binprogs "${PROJECT_BINARY_DIR}/python2.7/bin/*")
if(APPLE)
    set (APP_BIN ".")
    set (APP_RES "Pineapple.app/Contents/Resources")
else()
    set (APP_BIN "bin")
    set (APP_RES "share/${VERSION_NAME}")
endif()

install (TARGETS ${PROJECT_NAME}
    DESTINATION ${APP_BIN})
install (FILES ${DATAFILES}
    DESTINATION "${APP_RES}")
install (DIRECTORY "${PROJECT_BINARY_DIR}/python2.7/"
    DESTINATION "${APP_RES}/python2.7")
install (PROGRAMS ${binprogs}
    DESTINATION "${APP_RES}/python2.7/bin")
install (FILES "scripts/pineapple-server"
    DESTINATION "${APP_RES}")
if(APPLE)
    set (APPS "${CMAKE_INSTALL_PREFIX}/Pineapple.app")
    install (CODE "
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\"    \"\"    \"\")
        execute_process(COMMAND codesign -s \"Nathan Whitehead\" \"${CMAKE_INSTALL_PREFIX}/Pineapple.app/Contents/MacOS/libwx_osx_cocoau-3.1.dylib\")
        execute_process(COMMAND codesign -s \"Nathan Whitehead\" \"${CMAKE_INSTALL_PREFIX}/Pineapple.app\")
    ")
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
    set (CPACK_GENERATOR "DragNDrop")
else()
    set (CPACK_GENERATOR "ZIP")
endif()

include (CPack)
