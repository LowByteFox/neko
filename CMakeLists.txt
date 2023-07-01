cmake_minimum_required(VERSION 3.22)
project(neko VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -Wall -Wextra -pedantic")

find_package(PkgConfig REQUIRED)

pkg_check_modules(V8 IMPORTED_TARGET REQUIRED v8)

FILE(GLOB_RECURSE SRC ./src/*.cpp ./src/*.c ./src/*.h ./src/*.hpp)

link_directories(${V8_LIBRARY_DIRS})

add_executable(neko ${SRC})
add_compile_definitions(V8_COMPRESS_POINTERS)
target_link_libraries(neko ${V8_LIBRARIES})
target_include_directories(neko PUBLIC ${V8_INCLUDE_DIRS})
set(EXECUTABLE_OUTPUT_PATH ../)
