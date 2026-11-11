cmake_minimum_required(VERSION 3.12)

project(yambar-bluetooth C)

find_package(PkgConfig REQUIRED)

pkg_search_module(SD_BUS REQUIRED libsystemd libelogind)

add_executable(yambar-bluetooth src/main.c)

target_include_directories(yambar-bluetooth PRIVATE ${SD_BUS_INCLUDE_DIRS})
target_link_libraries(yambar-bluetooth PRIVATE ${SD_BUS_LIBRARIES})

install(TARGETS yambar-bluetooth)

