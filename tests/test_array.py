import numpy as np

from physicaldtype import physical_dimension_names, PhysicalDType, PhysicalScalar
from hypothesis import given
from hypothesis import strategies as st


@given(
    exponents=st.lists(
        st.floats(allow_nan=False, allow_infinity=False, width=32),
        min_size=7,
        max_size=7,
    ),
    values=st.lists(
        st.floats(allow_nan=False, allow_infinity=False, width=32),
        min_size=2,
        max_size=20,
    ),
)
def test__physical_dtype_getitem(exponents: list[float], values: list[float]):
    phy_dim = {name: exp for name, exp in zip(physical_dimension_names, exponents)}
    arr = np.array(values, dtype=PhysicalDType(phy_dim))

    for i in range(len(arr)):
        value = arr[i]
        assert isinstance(value, PhysicalScalar)

        assert value.physical_dimension.exponents == tuple(exponents)
        np.testing.assert_allclose(value.value, values[i])
