project(Pineapple)
cmake_minimum_required(VERSION 2.8)

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
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

### Rules for virtualenv

set(VENV_DIR ${PROJECT_BINARY_DIR}/venv)
add_custom_command(
    OUTPUT "${VENV_DIR}/bin/activate"
    COMMAND virtualenv "${VENV_DIR}"
    COMMAND "${VENV_DIR}/bin/pip" install -r "${PROJECT_SOURCE_DIR}/requirements.txt"
    COMMENT "Setting up virtualenv"
)
add_custom_target(venv DEPENDS "${VENV_DIR}/bin/activate")
add_dependencies(${PROJECT_NAME} venv)

### Server startup script

configure_file(scripts/eridani-main ${PROJECT_BINARY_DIR}/scripts/eridani-main)

### Testing targets

set (ENV{PINEAPPLE_SERVER}
    "${PROJECT_BINARY_DIR}/venv/bin/python ${PROJECT_BINARY_DIR}/scripts/eridani-main serve")

add_custom_target(test
    DEPENDS ${PROJECT_NAME} venv
    COMMAND "${PROJECT_NAME}"
)
