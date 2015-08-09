project(myProject)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
if(APPLE)
    # Also need to tell linker about C++11 on Mac
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-stdlib=libc++")
endif()

set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

set(SRC_LIST
    src/app.cc
)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
