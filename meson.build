project('physicaldtype', 'c')


py = import('python').find_installation(pure: false)


incdir_numpy = run_command(py,
  [
    '-c',
    'import numpy; print(numpy.get_include())'
  ],
  check: true
).stdout().strip()


includes = include_directories(
  [
    incdir_numpy,
  ]
)
srcs = [
    'src/physicaldtype/physicaldtype_main.c',
    'src/physicaldtype/physical_dimension.c',
    'src/physicaldtype/physical_dimension.h',
]


py.install_sources(
  [
    'src/__init__.py',
  ],
  subdir: 'physicaldtype',
  pure: false
)

py.extension_module(
    '_physicaldtype_main',
    sources: srcs,
    install: true,
    subdir: 'physicaldtype',
    include_directories: includes
)
