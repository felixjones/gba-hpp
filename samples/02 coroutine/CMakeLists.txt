#===============================================================================
#
# Fiber sample
#
# Copyright (C) 2022 gba-hpp contributors
# For conditions of distribution and use, see copyright notice in LICENSE.md
#
#===============================================================================

cmake_minimum_required(VERSION 3.18)

project(sample CXX)

# Add gba-toolchain libraries
gba_add_library_subdirectory(rom agbabi)
# Add gba-hpp
add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/gba-hpp)

add_executable(game main.cpp)
set_target_properties(game PROPERTIES SUFFIX ".elf")
target_link_libraries(game PRIVATE rom gba-hpp agbabi)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(warnings -Weverything -Wno-c++98-compat -Wno-c++20-compat)
else()
    set(warnings -Wall -Wextra -Wshadow -Wnon-virtual-dtor)
endif()

target_compile_options(game PRIVATE -mthumb ${warnings} -pedantic)
