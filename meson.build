project('gba-hpp', 'cpp',
  version: '4.0.0',
  license: 'Zlib',
  default_options: ['warning_level=2', 'cpp_std=c++20'])

gba_hpp_dep = declare_dependency(
  include_directories: ['include'])

meson.override_dependency('gba-hpp', gba_hpp_dep)
