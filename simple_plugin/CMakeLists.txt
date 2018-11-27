cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(simple_plugin VERSION 1.0 LANGUAGES C)

pkg_check_modules(GST REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.14)

add_library(${PROJECT_NAME} MODULE
  src/gst_simple_plugin_src.c
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    GST_PACKAGE_VERSION="${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    GST_PACKAGE_LICENSE="LGPL"
    GST_PACKAGE_NAME="${CMAKE_PROJECT_NAME}"
    GST_PACKAGE_ORIGIN="https://www.example.com"
    PACKAGE="${CMAKE_PROJECT_NAME} package"
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    PkgConfig::GST
)
