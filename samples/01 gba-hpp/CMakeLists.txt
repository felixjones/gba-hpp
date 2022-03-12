#===============================================================================
#
# Basic GBA project using gba-hpp
#
# Copyright (C) 2022 gba-hpp contributors
# For conditions of distribution and use, see copyright notice in LICENSE.md
#
#===============================================================================

cmake_minimum_required(VERSION 3.18)

project(gba-hpp CXX)

# Add gba-toolchain libraries
gba_add_library_subdirectory(rom)
# Add gba-hpp
add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/gba-hpp)

add_executable(game main.cpp)
set_target_properties(game PROPERTIES SUFFIX ".elf")

target_link_libraries(game PRIVATE rom gba-hpp)
