cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(simple_plugin_proj VERSION 1.0)

find_package(PkgConfig REQUIRED)

add_subdirectory(simple_plugin)
add_subdirectory(simple_plugin_tester)
