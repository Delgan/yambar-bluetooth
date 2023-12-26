cmake_minimum_required(VERSION 3.24)

project(yambar_bluetooth C)

find_package(PkgConfig)
pkg_check_modules(SD_BUS REQUIRED libsystemd)

add_executable(yambar_bluetooth src/main.c)

target_include_directories(yambar_bluetooth PRIVATE ${SD_BUS_INCLUDE_DIRS})
target_link_libraries(yambar_bluetooth PRIVATE ${SD_BUS_LIBRARIES})