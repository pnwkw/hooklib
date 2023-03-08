cmake_minimum_required(VERSION 3.17)

project(hooklib VERSION 1.0 LANGUAGES CXX)

# external libs
add_subdirectory(external)

add_library(hooklib STATIC)
add_library(hooklib::hooklib ALIAS hooklib)
target_compile_features(hooklib PUBLIC cxx_std_17)

target_include_directories(hooklib PUBLIC include)
target_sources(hooklib INTERFACE
        include/hooklib/hooklib.h
)

# Actual code
add_subdirectory(src)
add_subdirectory(tests)
