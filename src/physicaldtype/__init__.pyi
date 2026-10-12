import abc
from typing import Literal, TYPE_CHECKING

if TYPE_CHECKING:
    import numpy as np

physical_dimension_names: tuple[str, str, str, str, str, str, str]

type PhysicalDimensionKey = Literal["L", "M", "T", "I", "Theta", "N", "J"]

class PhysicalDimension(abc.ABC):
    @property
    @abc.abstractmethod
    def exponents(self) -> tuple[float, float, float, float, float, float, float]:
        """
        Returns the exponents of the physical dimensions in order.

        Returns:
            A tuple of 7 floats representing the exponents of the physical dimensions.

        """
        pass

    @abc.abstractmethod
    def get_exponent(self, dimension: PhysicalDimensionKey) -> float:
        """
        Returns the exponent of the specified physical dimension.

        Args:
            dimension: A string representing the physical dimension (e.g., "L", "M", "T", "I", "Theta", "N", "J").

        Returns:
            A float representing the exponent of the specified physical dimension.
        """
        pass

class PhysicalFloat64Scalar(abc.ABC):
    @abc.abstractmethod
    def __init__(self, value: float, dimension: dict[PhysicalDimensionKey, float]):
        pass

    @property
    @abc.abstractmethod
    def value(self) -> float:
        pass

    @property
    @abc.abstractmethod
    def physical_dimension(self) -> PhysicalDimension:
        pass


class PhysicalFloat32Scalar(abc.ABC):
    @abc.abstractmethod
    def __init__(self, value: float, dimension: dict[PhysicalDimensionKey, float]):
        pass

    @property
    @abc.abstractmethod
    def value(self) -> float:
        pass

    @property
    @abc.abstractmethod
    def physical_dimension(self) -> PhysicalDimension:
        pass


class PhysicalFloat64DType(abc.ABC):
    @abc.abstractmethod
    def __init__(self, dimension: dict[PhysicalDimensionKey, float]):
        pass

    @property
    @abc.abstractmethod
    def physical_dimension(self) -> PhysicalDimension:
        pass

    @property
    @abc.abstractmethod
    def storage_dtype(self) -> np.dtype:
        pass


class PhysicalFloat32DType(abc.ABC):
    @abc.abstractmethod
    def __init__(self, dimension: dict[PhysicalDimensionKey, float]):
        pass

    @property
    @abc.abstractmethod
    def physical_dimension(self) -> PhysicalDimension:
        pass

    @property
    @abc.abstractmethod
    def storage_dtype(self) -> np.dtype:
        pass


class PhysicalDType(abc.ABC):
    @abc.abstractmethod
    def __getitem__(self, key: str) -> np.typing.DTypeLike:
        pass


class PhysicalScalar(abc.ABC):
    @abc.abstractmethod
    def __getitem__(self, key: str) -> type:
        pass
