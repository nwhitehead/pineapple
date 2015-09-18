### Options

option(INCLUDE_EXTRA_PACKAGES "Include extra python packages for Full version (SciPy, ggplot, etc.)" OFF)

option(USE_PYTHON3 "Use latest Python3 version (instead of Python2.7)" ON)

option(USE_CCACHE "Try to speed up compiles with CCACHE" ON)

set (SIGNING_IDENTITY "Nathan Whitehead")

# always install locally
set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")


### Name to use

project(Pineapple)

cmake_minimum_required(VERSION 2.8)


### Version info

set(VERSION_NAME ${PROJECT_NAME})
set(UPPERCASE_VERSION_NAME "PINEAPPLE")

file(STRINGS "config/MAJOR" VERSION_MAJOR)
file(STRINGS "config/MINOR" VERSION_MINOR)
file(STRINGS "config/PATCH" VERSION_PATCH)
message("VERSION is ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

if (USE_PYTHON3)
    set(PYTHON_VERSION "3.5")
    set(PYTHON_FULL_VERSION "3.5.0")
else()
    set(PYTHON_VERSION "2.7")
    set(PYTHON_FULL_VERSION "2.7.10")
endif()
set(OPENSSL_VERSION "1.0.2d")
set(CURL_VERSION "7.44.0")
set(PYCURL_VERSION "7.19.5")

if (USE_PYTHON3 AND APPLE)
    set (PYTHON_FRAMEWORK_BUILD ON)
else()
    set (PYTHON_FRAMEWORK_BUILD OFF)
endif()

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
if(USE_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    endif(CCACHE_FOUND)
endif()

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
endif()


## Python

set(PYTHON_PREFIX "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}-prefix")
set(PYTHON_DIR "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}")
set(PYTHON_SOURCE "${PYTHON_PREFIX}/src/python${PYTHON_VERSION}-external")
set(PYTHON "${PYTHON_DIR}/bin/python${PYTHON_VERSION}")

set(PYTHON_OPENSSL "")

# Base options
set(PYTHON_CONFIGURE_OPTIONS --enable-ipv6 --with-threads --with-fpectl)

## Linux specific options
if(LINUX)
    set(PYTHON_OPENSSL "CPPFLAGS=-I${OPENSSL_DIR}" "LDFLAGS=-L${OPENSSL_DIR}/lib -L/usr/lib/x86_64-linux-gnu")
endif()

## Apple specific options
if(APPLE)
    set(PYTHON_CONFIGURE_OPTIONS ${PYTHON_CONFIGURE_OPTIONS})
    if(EXISTS /usr/local/include/libintl.h)
        message(FATAL_ERROR "You need to move /usr/local/include/libintl.h somewhere else")
    endif()
    if (PYTHON_FRAMEWORK_BUILD)
        set(PYTHON_CONFIGURE_OPTIONS ${PYTHON_CONFIGURE_OPTIONS} --enable-framework=${PYTHON_DIR})
    else()
        set(PYTHON_CONFIGURE_OPTIONS ${PYTHON_CONFIGURE_OPTIONS} darwin64-x86_64-cc)
    endif()
endif()

set (PYTHON_PATCH_COMMAND "")
if (USE_PYTHON3)
    # python3 specific
else()
    # python2.7 specific
    if(LINUX)
        set(PYTHON_CONFIGURE_OPTIONS ${PYTHON_CONFIGURE_OPTIONS} --enable-unicode=ucs4)
        set (PYTHON_PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL}/python-${PYTHON_VERSION}/Setup.local ${PYTHON_SOURCE}/Modules/)
    endif()
endif()

ExternalProject_Add(
    python${PYTHON_VERSION}-external
    URL "${EXTERNAL}/python-${PYTHON_VERSION}/Python-${PYTHON_FULL_VERSION}.tgz"
    PREFIX "${PYTHON_PREFIX}"
    PATCH_COMMAND ${PYTHON_PATCH_COMMAND}
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env ${PYTHON_OPENSSL} ./configure --prefix=${PYTHON_DIR} ${PYTHON_CONFIGURE_OPTIONS}
    BUILD_COMMAND make
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND make install PYTHONAPPSDIR=${PYTHON_DIR}
)
add_custom_target(python${PYTHON_VERSION} DEPENDS python${PYTHON_VERSION}-external openssl)
add_custom_target(python${PYTHON_VERSION}-postinstall
    DEPENDS python${PYTHON_VERSION}
    COMMAND ${PYTHON} ${EXTERNAL}/python-${PYTHON_VERSION}/get-pip.py
    COMMENT "Setting up pip"
)
set(PIP "${PYTHON_DIR}/bin/pip${PYTHON_VERSION}")


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

add_custom_target(requirements-install-base
    DEPENDS python${PYTHON_VERSION} python${PYTHON_VERSION}-postinstall openssl libcurl pycurl
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${SSL_DIR}/lib:${CURL_DIR}/lib ${PIP} install -r ${PROJECT_SOURCE_DIR}/requirements-base1.txt
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${SSL_DIR}/lib:${CURL_DIR}/lib ${PIP} install -r ${PROJECT_SOURCE_DIR}/requirements-base2.txt
)

add_custom_target(requirements-install-extra
    DEPENDS python${PYTHON_VERSION} python${PYTHON_VERSION}-postinstall openssl libcurl pycurl
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${SSL_DIR}/lib:${CURL_DIR}/lib ${PIP} install -r ${PROJECT_SOURCE_DIR}/requirements-extra1.txt
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${SSL_DIR}/lib:${CURL_DIR}/lib ${PIP} install -r ${PROJECT_SOURCE_DIR}/requirements-extra2.txt
)

if (INCLUDE_EXTRA_PACKAGES)
    add_custom_target(requirements-install
        DEPENDS requirements-install-base requirements-install-extra
        COMMAND ${PROJECT_SOURCE_DIR}/scripts/clean-python.sh ${PYTHON_DIR}
    )
else()
    add_custom_target(requirements-install DEPENDS requirements-install-base
        COMMAND ${PROJECT_SOURCE_DIR}/scripts/clean-python.sh ${PYTHON_DIR}
    )
endif()

add_custom_target(requirements-clean
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${SSL_DIR}/lib:${CURL_DIR}/lib ${PIP} freeze | ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${SSL_DIR}/lib:${CURL_DIR}/lib xargs ${PIP} uninstall -y
)

# Server resources
set(LESSC "lessc")
if (PYTHON_FRAMEWORK_BUILD)
    set(CUSTOM_DIR "${PYTHON_DIR}/Python.framework/Versions/${PYTHON_VERSION}/lib/python${PYTHON_VERSION}/site-packages/notebook/static/custom/")
else()
    set(CUSTOM_DIR "${PYTHON_DIR}/lib/python${PYTHON_VERSION}/site-packages/notebook/static/custom/")
endif()
add_custom_target(custom-reinstall-first
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/custom" "${CUSTOM_DIR}"
)
add_custom_target(compile-less
    DEPENDS custom-reinstall-first
    COMMAND ${LESSC} ${PROJECT_SOURCE_DIR}/custom/theme-light.less ${CUSTOM_DIR}/theme-light.css
    COMMAND ${LESSC} ${PROJECT_SOURCE_DIR}/custom/theme-dark.less ${CUSTOM_DIR}/theme-dark.css
    COMMAND ${LESSC} ${PROJECT_SOURCE_DIR}/custom/theme-borlandia.less ${CUSTOM_DIR}/theme-borlandia.css
    COMMAND ${LESSC} ${PROJECT_SOURCE_DIR}/custom/theme-red.less ${CUSTOM_DIR}/theme-red.css
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
    data/examples/python${PYTHON_VERSION}/Basics.ipynb
    data/examples/python${PYTHON_VERSION}/Editing.ipynb
    data/examples/python${PYTHON_VERSION}/Execution.ipynb
    data/examples/python${PYTHON_VERSION}/Markdown.ipynb
    data/examples/python${PYTHON_VERSION}/Packages.ipynb
    data/examples/python${PYTHON_VERSION}/Plotting.ipynb
    data/examples/python${PYTHON_VERSION}/ReadOnly.ipynb
    data/examples/python${PYTHON_VERSION}/RichOutput.ipynb
    data/examples/python${PYTHON_VERSION}/Timing.ipynb
    data/examples/python${PYTHON_VERSION}/Video.ipynb
    data/examples/python${PYTHON_VERSION}/Welcome.ipynb
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

if (PYTHON_FRAMEWORK_BUILD)
    set (CPP_PYTHON_DIR "..")
else()
    set (CPP_PYTHON_DIR "python${PYTHON_VERSION}")
endif()
set (CPP_PYTHON_NAME "python${PYTHON_VERSION}")
configure_file("${PROJECT_SOURCE_DIR}/src/config.hh.in" "${PROJECT_BINARY_DIR}/include/config.hh")
include_directories("${PROJECT_BINARY_DIR}/include")
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

## Mac OS X specific information

if(APPLE)
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION_NAME}.${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION_NAME}.${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
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
    set (TESTNAME "${PROJECT_NAME}.app/Contents/MacOS/Pineapple")
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

# HACK dependecy of "make install" on "make custom-install"
install(CODE "
    execute_process(COMMAND ${CMAKE_BUILD_TOOL} custom-install
                    WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\")"
)

if(APPLE)
    set (APP_BIN ".")
    set (APP_RES "${PROJECT_NAME}.app/Contents/Resources")
else()
    set (APP_BIN "bin")
    set (APP_RES "share/${VERSION_NAME}")
endif()

install (TARGETS ${PROJECT_NAME}
    DESTINATION ${APP_BIN})
install (FILES ${DATAFILES}
    DESTINATION "${APP_RES}")
install (FILES "scripts/pineapple-server"
    DESTINATION "${APP_RES}")

if(APPLE AND PYTHON_FRAMEWORK_BUILD)

    configure_file("${PROJECT_SOURCE_DIR}/scripts/pip" "${PROJECT_BINARY_DIR}/scripts/pip${PYTHON_VERSION}" @ONLY)
    install (PROGRAMS ${PROJECT_BINARY_DIR}/scripts/pip${PYTHON_VERSION}
        DESTINATION "${PROJECT_NAME}.app/Contents/bin/")

    set (APP_FRAMEWORKS "${PROJECT_NAME}.app/Contents/Frameworks")
    set (PYTHON_FRAMEWORK "${APP_FRAMEWORKS}/Python.framework/")
    install (DIRECTORY "${PYTHON_DIR}/Python.framework/Versions" DESTINATION "${PYTHON_FRAMEWORK}" PATTERN bin/* EXCLUDE)

    # Install Freetype from build computer
    if (USE_PYTHON3)
        install (PROGRAMS "/opt/X11/lib/libfreetype.6.dylib"
            DESTINATION ${APP_FRAMEWORKS})
        install (PROGRAMS "/opt/X11/lib/libpng15.15.dylib"
            DESTINATION ${APP_FRAMEWORKS})
    endif()

    # Post-install rpath fixups for Mac
    # (also installs framework since cmake had problems)
    # NOTE: extra backslash for cmake_install_prefix to delay evaluation for CPACK

    set (APP "\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")
    set (PYTHON_DEST "${PROJECT_NAME}.app/Contents/bin/python${PYTHON_VERSION}")
    set (PY_FRAMEWORK "Python.framework/Versions/${PYTHON_VERSION}")
    set (PY_APP "Contents/Frameworks/${PY_FRAMEWORK}/Resources/Python.app/Contents/MacOS/Python")
    install (CODE "
            execute_process(COMMAND ${PROJECT_SOURCE_DIR}/scripts/clean-python.sh \"${APP}/Contents/Frameworks/${PY_FRAMEWORK}\")
        ")
    install (CODE "
        include(BundleUtilities)
        fixup_bundle(\"${APP}\"    \"\"    \"\")
    ")
    install (CODE "
        execute_process(COMMAND mkdir -p \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/bin\")
        execute_process(COMMAND cp ${PYTHON} \"\${CMAKE_INSTALL_PREFIX}/${PYTHON_DEST}\")
        execute_process(COMMAND chmod a+x \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/${PY_APP}\")
    ")
    install (CODE "
        execute_process(COMMAND install_name_tool -change \"${PYTHON_DIR}/${PY_FRAMEWORK}/Python\" \"@executable_path/../Frameworks/${PY_FRAMEWORK}/Python\" \"\${CMAKE_INSTALL_PREFIX}/${PYTHON_DEST}\" )
        execute_process(COMMAND install_name_tool -change \"${PYTHON_DIR}/${PY_FRAMEWORK}/Python\" \"@executable_path/../../../../Python\" \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/${PY_APP}\" )
    ")
    if (USE_PYTHON3)
        # fixup freetype, dangling dylib reference
        install (CODE "
            execute_process(COMMAND install_name_tool -change \"/opt/X11/lib/libfreetype.6.dylib\" \"@executable_path/../../../../../../../libfreetype.6.dylib\" 
          \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks/${PY_FRAMEWORK}/lib/python${PYTHON_VERSION}/site-packages/matplotlib/ft2font.cpython-35m-darwin.so\" )
        ")
        install (CODE "
            execute_process(COMMAND install_name_tool -change \"/opt/X11/lib/libfreetype.6.dylib\" \"@executable_path/../../../../../../../libfreetype.6.dylib\" 
          \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks/${PY_FRAMEWORK}/lib/python${PYTHON_VERSION}/site-packages/matplotlib/backends/_backend_agg.cpython-35m-darwin.so\" )
        ")
        # fixup libpng
        install (CODE "
            execute_process(COMMAND install_name_tool -change \"/opt/X11/lib/libpng15.15.dylib\" \"@executable_path/../../../../../../../libpng15.15.dylib\" 
          \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks/${PY_FRAMEWORK}/lib/python${PYTHON_VERSION}/site-packages/matplotlib/_png.cpython-35m-darwin.so\" )
        ")
    endif()
    install (CODE "
        execute_process(COMMAND codesign --force -s \"${SIGNING_IDENTITY}\" \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/MacOS/libwx_osx_cocoau-3.1.dylib\")
        execute_process(COMMAND codesign --force -s \"${SIGNING_IDENTITY}\" \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks/Python.framework\")
        execute_process(COMMAND codesign --force -s \"${SIGNING_IDENTITY}\" \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app\")
    ")

else()

    file (GLOB binprogs "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}/bin/*")
    install (DIRECTORY "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}/include" DESTINATION "${APP_RES}/python${PYTHON_VERSION}")
    install (DIRECTORY "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}/lib" DESTINATION "${APP_RES}/python${PYTHON_VERSION}")
    install (DIRECTORY "${PROJECT_BINARY_DIR}/python${PYTHON_VERSION}/share" DESTINATION "${APP_RES}/python${PYTHON_VERSION}")
    install (PROGRAMS ${binprogs}
        DESTINATION "${APP_RES}/python${PYTHON_VERSION}/bin")
#    install (PROGRAMS ${PROJECT_SOURCE_DIR}
#        DESTINATION "${APP_RES}/python${PYTHON_VERSION}/bin")

    # Clean up to make final size smaller
    install (CODE "
        execute_process(COMMAND ${PROJECT_SOURCE_DIR}/scripts/clean-python.sh \"\${CMAKE_INSTALL_PREFIX}/${APP_RES}/python${PYTHON_VERSION}\")
    ")

    # rpath fixups (simpler for non framework builds)
    if(APPLE)
        set (APPS "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")
        install (CODE "
            include(BundleUtilities)
            fixup_bundle(\"${APPS}\"    \"\"    \"\")
            execute_process(COMMAND codesign --force -s \"${SIGNING_IDENTITY}\" \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/MacOS/libwx_osx_cocoau-3.1.dylib\")
            execute_process(COMMAND codesign --force -s \"${SIGNING_IDENTITY}\" \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app\")
        ")
    endif()
endif()

### HACK for dev work inside install to avoid "make install"
if (PYTHON_FRAMEWORK_BUILD)
    set(CUSTOM_INSTALL_LOCATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks/Python.framework/Versions/${PYTHON_VERSION}/lib/python${PYTHON_VERSION}/site-packages/notebook/static/custom")
else()
    set(CUSTOM_INSTALL_LOCATION "${CMAKE_INSTALL_PREFIX}/${APP_RES}/python${PYTHON_VERSION}/lib/python${PYTHON_VERSION}/site-packages/notebook/static/custom")
endif()
add_custom_target(custom-install-dev
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CUSTOM_INSTALL_LOCATION}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CUSTOM_DIR}" "${CUSTOM_INSTALL_LOCATION}"
    DEPENDS custom-reinstall
)


### CPACK


include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VENDOR "${SIGNING_IDENTITY}")
set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set (CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set (CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "${PROJECT_NAME}")
set (CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-Python${PYTHON_VERSION}")
if (INCLUDE_EXTRA_PACKAGES)
    set (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-Full")
endif()

if(APPLE)
    set (CPACK_GENERATOR "DragNDrop")
else()
    set (CPACK_GENERATOR "TBZ2")
endif()

include (CPack)
