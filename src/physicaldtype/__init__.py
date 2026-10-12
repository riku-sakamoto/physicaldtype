from typing import Mapping
from types import MappingProxyType
from ._physicaldtype_main import (
    PhysicalDimension,
    PhysicalFloat64Scalar,
    PhysicalFloat32Scalar,
    PhysicalFloat64DType,
    PhysicalFloat32DType,
)
import numpy as np


physical_dimension_names: tuple[str, str, str, str, str, str, str] = (
    "L",
    "M",
    "T",
    "I",
    "Theta",
    "N",
    "J",
)


class _LowerKeyDict(Mapping):
    def __init__(self, value: dict):
        self._values = MappingProxyType(value)

    def __getitem__(self, key: str) -> np.typing.DTypeLike:
        key = key.lower()
        if key in self._values:
            return self._values[key]

        raise KeyError(f"Invalid key: {key}.")

    def __iter__(self):
        return iter(self._values)

    def __len__(self):
        return len(self._values)


PhysicalDType = _LowerKeyDict(
    {
        "float64": PhysicalFloat64DType,
        "float32": PhysicalFloat32DType,
    }
)

PhysicalScalar = _LowerKeyDict(
    {
        "float64": PhysicalFloat64Scalar,
        "float32": PhysicalFloat32Scalar,
    }
)

__all__ = [
    "PhysicalDimension",
    "PhysicalFloat64Scalar",
    "PhysicalFloat32Scalar",
    "PhysicalFloat64DType",
    "PhysicalFloat32DType",
    "physical_dimension_names",
    "PhysicalDType",
    "PhysicalScalar",
]
