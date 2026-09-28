project(
  'physicaldtype',
  'c',
  version: '0.0.1',
  default_options: ['warning_level=2'],
)

add_project_arguments(
  '-Werror=implicit-function-declaration',
  '-Werror=incompatible-pointer-types',
  '-Werror=return-type',
  '-Werror=int-conversion',
  '-Wno-missing-field-initializers',
  language: 'c',
)


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
    'src/csrc/physicaldtype_main.c',
    'src/csrc/physical_dimension.c',
    'src/csrc/physical_dimension_ops.c',
    'src/csrc/dtype.c',
    'src/csrc/casts.c',
    'src/csrc/scalar.c',
    'src/csrc/scalar_ops.c',
    'src/csrc/umath/physical_arithmetic.c',
    'src/csrc/umath/umath.c',
]


py.install_sources(
  [
    'src/physicaldtype/__init__.py',
    'src/physicaldtype/__init__.pyi',
    'src/physicaldtype/py.typed',
  ],
  subdir: 'physicaldtype',
  pure: false,
)

py.extension_module(
    '_physicaldtype_main',
    sources: srcs,
    install: true,
    subdir: 'physicaldtype',
    include_directories: includes
)
