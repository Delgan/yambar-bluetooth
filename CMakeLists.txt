cmake_minimum_required(VERSION 3.12)

project(yambar-bluetooth C)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SD_BUS QUIET libsystemd)

if(NOT SD_BUS_FOUND)
    pkg_check_modules(SD_BUS REQUIRED libelogind)
endif()

add_executable(yambar-bluetooth src/main.c)

target_include_directories(yambar-bluetooth PRIVATE ${SD_BUS_INCLUDE_DIRS})
target_link_libraries(yambar-bluetooth PRIVATE ${SD_BUS_LIBRARIES})

install(TARGETS yambar-bluetooth)
