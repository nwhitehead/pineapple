set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}") # install locally

project(Pineapple)
cmake_minimum_required(VERSION 2.8)


### Version info

set(VERSION_NAME "Pineapple")
set(UPPERCASE_VERSION_NAME "PINEAPPLE")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 5)

set(PYTHON_VERSION "2.7")
set(PYTHON_FULL_VERSION "2.7.10")
set(OPENSSL_VERSION "1.0.2d")
set(CURL_VERSION "7.44.0")
set(PYCURL_VERSION "7.19.5")

# Expect targets: APPLE LINUX WINDOWS
if(NOT APPLE)
    set(LINUX 1)
endif()

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
        URL "${EXTERNAL}/openssl/openssl-${OPENSSL_VERSION}.tar.gz"
        PREFIX "${OPENSSL_PREFIX}"
        CONFIGURE_COMMAND ./config --prefix=${OPENSSL_DIR} shared no-idea no-mdc2 no-rc5
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
)
if(LINUX)
    # Ubuntu uses ucs4
    set(PYTHON_CONFIGURE_OPTIONS ${PYTHON_CONFIGURE_OPTIONS}
        --enable-unicode=ucs4)
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

set(PYTHON_PREFIX "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}-prefix")
set(PYTHON_DIR "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}")
set(PYTHON_SOURCE "${PYTHON_PREFIX}/src/python${PYTHON_VERSION}-external")
set(PYTHON "${PYTHON_DIR}/bin/python")
ExternalProject_Add(
    python${PYTHON_VERSION}-external
    URL "${EXTERNAL}/python-${PYTHON_VERSION}/Python-${PYTHON_FULL_VERSION}.tgz"
    PREFIX "${PYTHON_PREFIX}"
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL}/python-${PYTHON_VERSION}/Setup.local ${PYTHON_SOURCE}/Modules/
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env ${PYTHON_OPENSSL} ./configure --prefix=${PYTHON_DIR} ${PYTHON_CONFIGURE_OPTIONS}
    BUILD_COMMAND make
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND make install
)
add_custom_target(python${PYTHON_VERSION} DEPENDS python${PYTHON_VERSION}-external openssl)
add_custom_target(python${PYTHON_VERSION}-postinstall
    DEPENDS python${PYTHON_VERSION}
    COMMAND ${PYTHON} ${EXTERNAL}/python-${PYTHON_VERSION}/get-pip.py
    COMMENT "Setting up pip"
)
set(PIP "${PYTHON_DIR}/bin/pip")


if(LINUX)

    ## libcurl

    set(CURL_PREFIX "${PROJECT_BINARY_DIR}/curl-prefix")
    set(CURL_DIR "${PROJECT_BINARY_DIR}/curl")
    ExternalProject_Add(
        libcurl-external
        URL "${EXTERNAL}/curl/curl-${CURL_VERSION}.zip"
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
    DEPENDS python${PYTHON_VERSION} libcurl
    URL "${EXTERNAL}/pycurl/pycurl-${PYCURL_VERSION}.tar.gz"
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
    DEPENDS python${PYTHON_VERSION} python${PYTHON_VERSION}-postinstall openssl libcurl pycurl
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${SSL_DIR}/lib:${CURL_DIR}/lib ${PIP} install -r ${PROJECT_SOURCE_DIR}/requirements.txt
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/clean-python.sh ${PYTHON_DIR}
)
# Server resources
set(LESSC "lessc")
set(CUSTOM_DIR "${PYTHON_DIR}/lib/python${PYTHON_VERSION}/site-packages/notebook/static/custom/")
add_custom_target(compile-less
    COMMAND ${LESSC} ${PROJECT_SOURCE_DIR}/custom/theme-light.less ${CUSTOM_DIR}/theme-light.css
    COMMAND ${LESSC} ${PROJECT_SOURCE_DIR}/custom/theme-dark.less ${CUSTOM_DIR}/theme-dark.css
    COMMAND ${LESSC} ${PROJECT_SOURCE_DIR}/custom/theme-borlandia.less ${CUSTOM_DIR}/theme-borlandia.css
    COMMAND ${LESSC} ${PROJECT_SOURCE_DIR}/custom/theme-red.less ${CUSTOM_DIR}/theme-red.css
    DEPENDS custom-reinstall-first
)
add_custom_target(custom-reinstall-first
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/custom" "${CUSTOM_DIR}"
)
add_custom_target(custom-reinstall
    DEPENDS compile-less)
add_custom_target(custom-install DEPENDS requirements-install custom-reinstall)



#### PINEAPPLE

### Pineapple app

## General

set(EMPTY data/empty)
set(DATAFILES
    data/loading.html
    data/blank.ipynb
    data/images/Pineapple.icns
    data/images/Pineapple-Doc.icns
    data/images/Copy.png
    data/images/Cut.png
    data/images/Delete.png
    data/images/Down.png
    data/images/FastForward.png
    data/images/Led-Blue-On-1632.png
    data/images/Led-Blue-Off-1632.png
    data/images/Paste.png
    data/images/Pen.png
    data/images/Pencil.png
    data/images/Pineapple-256.png
    data/images/Play.png
    data/images/Plus.png
    data/images/Save.png
    data/images/Stop.png
    data/images/Synchronize.png
    data/images/Up.png
    data/examples/Basics.ipynb
    data/examples/Editing.ipynb
    data/examples/Execution.ipynb
    data/examples/Markdown.ipynb
    data/examples/Packages.ipynb
    data/examples/Plotting.ipynb
    data/examples/ReadOnly.ipynb
    data/examples/RichOutput.ipynb
    data/examples/Timing.ipynb
    data/examples/Video.ipynb
)

set(SRC_LIST
    src/callback.cc
    src/gui_util.cc
    src/util.cc
    src/ExamplesFrame.cc
    src/MainFrame.cc
    src/MainApp.cc
    src/RecentManager.cc
    src/PreferencesManager.cc
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
    COMMAND ${CMAKE_COMMAND} -E env "PINEAPPLE_PYTHON=${PYTHON}" "PINEAPPLE_SERVER=${SERVER}" ${TESTNAME}
)
add_custom_target(local-test DEPENDS custom-reinstall local-retest)
set_target_properties(local-test PROPERTIES EXCLUDE_FROM_ALL 1)
set_target_properties(local-retest PROPERTIES EXCLUDE_FROM_ALL 1)


### Install

file (GLOB binprogs "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}/bin/*")
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
install (DIRECTORY "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}/include" DESTINATION "${APP_RES}/python${PYTHON_VERSION}")
install (DIRECTORY "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}/lib" DESTINATION "${APP_RES}/python${PYTHON_VERSION}")
install (DIRECTORY "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}/share" DESTINATION "${APP_RES}/python${PYTHON_VERSION}")
install (PROGRAMS ${binprogs}
    DESTINATION "${APP_RES}/python${PYTHON_VERSION}/bin")
install (FILES "scripts/pineapple-server"
    DESTINATION "${APP_RES}")
# Clean up to make final size smaller
install (CODE "
        execute_process(COMMAND ${PROJECT_SOURCE_DIR}/scripts/clean-python.sh \"${CMAKE_INSTALL_PREFIX}/${APP_RES}/python${PYTHON_VERSION}\")
    ")
if(APPLE)
    set (APPS "${CMAKE_INSTALL_PREFIX}/Pineapple.app")
    install (CODE "
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\"    \"\"    \"\")
        execute_process(COMMAND codesign --force -s \"Nathan Whitehead\" \"${CMAKE_INSTALL_PREFIX}/Pineapple.app/Contents/MacOS/libwx_osx_cocoau-3.1.dylib\")
        execute_process(COMMAND codesign --force -s \"Nathan Whitehead\" \"${CMAKE_INSTALL_PREFIX}/Pineapple.app\")
    ")
endif()

### HACK for dev work inside install to avoid "make install"
set(CUSTOM_INSTALL_LOCATION "${CMAKE_INSTALL_PREFIX}/${APP_RES}/python${PYTHON_VERSION}/lib/python${PYTHON_VERSION}/site-packages/notebook/static/custom")
add_custom_target(custom-install-dev
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CUSTOM_INSTALL_LOCATION}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CUSTOM_DIR}" "${CUSTOM_INSTALL_LOCATION}"
    DEPENDS custom-reinstall
)


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
    set (CPACK_GENERATOR "TBZ2")
endif()

include (CPack)
