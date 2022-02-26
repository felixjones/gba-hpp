#===============================================================================
#
# Copyright (C) 2022 gba-hpp contributors
# For conditions of distribution and use, see copyright notice in LICENSE.md
#
#===============================================================================

cmake_minimum_required(VERSION 3.19)

project(gba-hpp CXX)

add_library(gba-hpp INTERFACE)
target_include_directories(gba-hpp INTERFACE include)
target_compile_features(gba-hpp INTERFACE cxx_std_20)
