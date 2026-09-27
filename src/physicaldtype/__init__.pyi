import abc

physical_dimension_names: tuple[str, str, str, str, str, str, str]

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
    def get_exponent(self, dimension: str) -> float:
        """
        Returns the exponent of the specified physical dimension.

        Args:
            dimension: A string representing the physical dimension (e.g., "L", "M", "T", "I", "Theta", "N", "J").

        Returns:
            A float representing the exponent of the specified physical dimension.
        """
        pass

class PhysicalScalar(abc.ABC):
    @abc.abstractmethod
    def __init__(self, value: float, dimension: PhysicalDimension):
        pass

    @property
    @abc.abstractmethod
    def value(self) -> float:
        pass

    @property
    @abc.abstractmethod
    def physical_dimension(self) -> PhysicalDimension:
        pass

class PhysicalDType(abc.ABC):
    @property
    @abc.abstractmethod
    def physical_dimension(self) -> PhysicalDimension:
        pass
