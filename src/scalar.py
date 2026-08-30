
class PhysicalScalar:
    def __init__(self, value, dimension_spec):
        from . import PhysicalDType
        from . import PhysicalBackend
        self.value = value
        if isinstance(dimension_spec, PhysicalBackend):
            self.dtype = PhysicalDType(dimension_spec)
        elif isinstance(dimension_spec, PhysicalDType):
            self.dtype = dimension_spec
        else:
            raise RuntimeError

    @property
    def exponent(self):
        return self.dtype.get_exponent()

    def __repr__(self):
        return f"{self.value} {self.dtype.get_exponent()}"

    def __rmul__(self, other):
        return PhysicalScalar(self.value * other, self.dtype.unit)

    def __eq__(self, other):
        return self.value == other.value and self.dtype == other.dtype
