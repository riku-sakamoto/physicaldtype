project('physical_dtype', 'c')


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
    'src/physical_dtype/module.c',
]

py.extension_module(
    'physical_dtype',
    sources: srcs,
    install: true,
    include_directories: includes
)
