#===============================================================================
#
# Copyright (C) 2022-2023 gba-hpp contributors
# For conditions of distribution and use, see copyright notice in LICENSE.md
#
#===============================================================================

project('gba-hpp', 'cpp',
  version: '5.0.0',
  license: 'Zlib',
  default_options: ['cpp_std=c++20'])

gba_hpp_dep = declare_dependency(
  include_directories: ['include'])

meson.override_dependency('gba-hpp', gba_hpp_dep)
