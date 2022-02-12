# Manager CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

project(manager VERSION 1.0 LANGUAGES CXX)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/Collection.cpp
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/Record.cpp
    ${PROJECT_SOURCE_DIR}/src/Utility.cpp
)
