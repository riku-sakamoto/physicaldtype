from ._physicaldtype_main import (
    PhysicalDimension,
    PhysicalScalar,
    PhysicalDType,
)

physical_dimension_names: tuple[str, str, str, str, str, str, str] = (
    "L",
    "M",
    "T",
    "I",
    "Theta",
    "N",
    "J",
)


__all__ = [
    "PhysicalDimension",
    "PhysicalScalar",
    "PhysicalDType",
    "physical_dimension_names",
]
