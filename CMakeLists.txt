project(Pineapple)
cmake_minimum_required(VERSION 2.8)

### Version info

set(VERSION_NAME "Pineapple")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)

# Use C++11 for everything
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

### wxWidgets

set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets REQUIRED core base webview)
include(${wxWidgets_USE_FILE})

### Pineapple app

set(SRC_LIST
    src/app.cc
)
configure_file("${PROJECT_SOURCE_DIR}/src/config.h.in"
    "${PROJECT_BINARY_DIR}/include/config.h")
include_directories("${PROJECT_BINARY_DIR}/include")
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

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

file(COPY "scripts/eridani-main" DESTINATION "${PROJECT_BINARY_DIR}/scripts/")
file(COPY "scripts/eridani-main.spec" DESTINATION "${PROJECT_BINARY_DIR}/scripts/")

### Demo files

file(COPY "demo/" DESTINATION "${PROJECT_BINARY_DIR}/demo/")

### Resources

file(COPY "src/loading.html" DESTINATION "${PROJECT_BINARY_DIR}/html/")
file(COPY "src/blank.ipynb" DESTINATION "${PROJECT_BINARY_DIR}/data/")
file(COPY "images/" DESTINATION "${PROJECT_BINARY_DIR}/images/")
file(COPY "notebook/notebook/static/" DESTINATION "${PROJECT_BINARY_DIR}/notebook/notebook/static/")
file(COPY "notebook/notebook/templates/" DESTINATION "${PROJECT_BINARY_DIR}/notebook/notebook/templates/")

### Testing targets

set (ENV{PINEAPPLE_SERVER}
    "${VENV_DIR}/bin/python ${PROJECT_BINARY_DIR}/scripts/eridani-main")

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
    COMMAND "${VENV_DIR}/bin/pyinstaller" -y "${PROJECT_BINARY_DIR}/scripts/eridani-main.spec"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/dist/eridani-main/tcl"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/dist/eridani-main/tk"
)
