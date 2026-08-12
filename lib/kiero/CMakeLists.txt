cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(kiero)

add_library(kiero STATIC kiero.cpp)

set(minhook "${CMAKE_SOURCE_DIR}/lib/minhook/include")
set(polyhook "${CMAKE_CURRENT_SOURCE_DIR}/lib/polyhook")

target_include_directories(kiero SYSTEM PRIVATE ${minhook})
target_include_directories(kiero SYSTEM PRIVATE ${polyhook})

target_link_libraries(kiero PRIVATE minhook)
target_link_libraries(kiero PRIVATE PolyHook_2)