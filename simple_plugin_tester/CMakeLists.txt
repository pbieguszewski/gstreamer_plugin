cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(simple_plugin_tester VERSION 1.0 LANGUAGES C)

pkg_check_modules(GST REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.14)

add_library(${PROJECT_NAME} MODULE
  src/main.c
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    PkgConfig::GST
)
