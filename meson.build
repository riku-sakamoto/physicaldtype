project('physicaldtype', 'c')

add_project_arguments(
  '-Wall',
  '-Wextra',
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
    'src/csrc/physical_dimension.h',
    'src/csrc/physical_dimension_ops.c',
    'src/csrc/physical_dimension_ops.h',
    'src/csrc/dtype.c',
    'src/csrc/dtype.h',
    'src/csrc/casts.c',
    'src/csrc/casts.h',
    'src/csrc/scalar.c',
    'src/csrc/scalar.h',
    'src/csrc/scalar_ops.c',
    'src/csrc/scalar_ops.h',
    'src/csrc/umath/physical_arithmetic.c',
    'src/csrc/umath/physical_arithmetic.h',
    'src/csrc/umath/umath.c',
    'src/csrc/umath/umath.h',
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
