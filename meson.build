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
    'src/physicaldtype/physical_dimension_ops.c',
    'src/physicaldtype/physical_dimension_ops.h',
    'src/physicaldtype/dtype.c',
    'src/physicaldtype/dtype.h',
    'src/physicaldtype/casts.c',
    'src/physicaldtype/casts.h',
    'src/physicaldtype/scalar.c',
    'src/physicaldtype/scalar.h',
    'src/physicaldtype/scalar_ops.c',
    'src/physicaldtype/scalar_ops.h',
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
