#===============================================================================
#
# Copyright (C) 2022-2023 gba-hpp contributors
# For conditions of distribution and use, see copyright notice in LICENSE.md
#
#===============================================================================

cmake_minimum_required(VERSION 3.18)

project(gba-hpp LANGUAGES CXX VERSION 5.0.0)

add_library(gba-hpp INTERFACE)
target_include_directories(gba-hpp INTERFACE include)
target_compile_features(gba-hpp INTERFACE cxx_std_20)

install(DIRECTORY include DESTINATION include)
