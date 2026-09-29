# PhysicalDType

:warning: This is a work in progress. The API is not stable and may change without notice. DO NOT use this in production code.

## Overview

PhysicalDType is an experimental NumPy parametric DType for representing physical dimensions and units 
using [NEP-41](https://numpy.org/neps/nep-0041-improved-dtype-support.html) and [NEP-43](https://numpy.org/neps/nep-0043-extensible-ufuncs.html).

The goal is to create a DType that can represent both physical dimension (e.g. Length, Mass, Time) and physical units (e.g. m, km, ms).

:warning: Physical dimension is only supported for now. Physical unit support is supposed to be implemented in the near future.

## How to build this repo

Only Linux is supported for now.

```bash
uv sync --extra dev
make build
```


## Usage

```python
import numpy as np
from physicaldtype import PhysicalDType


mass = np.array([1, 2, 3], dtype=PhysicalDType({"M": 1}))
acc = np.array([1, 2, 3], dtype=PhysicalDType({"L": 1, "T": -2}))

force = mass * acc
print(force.dtype) 
# PhysicalDType('PhysicalDimension({'L': 1.0, 'M': 1.0, 'T': -2.0, 'I': 0.0, 'Theta': 0.0, 'N': 0.0, 'J': 0.0})')

```


## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## References

To create a parametric DType, I referred to the following resources:

* [unytdtype](https://github.com/numpy/numpy-user-dtypes/tree/main/unytdtype)
    * This library creates a parametric DType for physical units utilizing the [unyt](https://github.com/yt-project/unyt) library.

* [numpy-quaddtype](https://github.com/numpy/numpy-quaddtype)
    * This library creates a cross-platform Quad (128-bit) float Data-Type for NumPy.


## Current status

PhysicalDType is currently experimental.

Supported:
- Physical dimensions represented by SI base dimensions
- NumPy array creation with `PhysicalDType`
- Basic arithmetic operations such as addition and multiplication

Not yet supported:
- Physical units and unit conversion
- Many NumPy ufuncs and operations
- Platforms other than Linux
