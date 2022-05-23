cmake_minimum_required (VERSION 3.12)

project ("GLSLGen")

set(CMAKE_CXX_STANDARD 20)

add_executable (GLSLGen "main.cpp")

include(tools/cmake/utility.cmake)

target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_SOURCE_ROOT="${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE source)

ADD_CMAKE_SUBDIRS_HERE()
ADD_CPP_SOURCES_HERE(${PROJECT_NAME})


target_link_libraries(${PROJECT_NAME} PUBLIC jclib glfw glad cppopengl)

