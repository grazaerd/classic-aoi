cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(imgui)

add_library(imgui STATIC 
            imgui_draw.cpp
            imgui_impl_dx11.cpp
            imgui_impl_win32.cpp
            imgui_tables.cpp
            imgui_widgets.cpp
            imgui.cpp)

target_include_directories(imgui INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
