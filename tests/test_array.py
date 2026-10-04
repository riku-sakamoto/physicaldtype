import pytest
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


def test__physical_dtype_invalid_key():
    phy_dim = {"InvalidKey": 1.0}
    with pytest.raises(ValueError, match="Invalid dimension name: InvalidKey"):
        _ = np.array([1.0, 2.0, 3.0], dtype=PhysicalDType(phy_dim))


def test__cannot_convert_to_utf8():
    phy_dim = {"\ud800": 1}
    with pytest.raises(
        UnicodeEncodeError, match="'utf-8' codec can't encode character"
    ):
        _ = np.array([1.0, 2.0, 3.0], dtype=PhysicalDType(phy_dim))


def test__overflow_exponent():
    phy_dim = {"L": 10**1000}
    with pytest.raises(OverflowError, match="int too large to convert to float"):
        _ = np.array([1.0, 2.0, 3.0], dtype=PhysicalDType(phy_dim))


def test__cast_from_physical_dtype_to_float64():
    arr = np.array([3.0, 4.0], dtype=PhysicalDType({"L": 1}))

    # This should succeed since the physical dimensions are compatible
    _ = arr.astype(np.float64, casting="safe")


def test__cast_from_physical_dtype_to_physical():
    arr: np.ndarray = np.array([3.0, 4.0], dtype=PhysicalDType({"L": 1}))
    with pytest.raises(
        TypeError,
        match=r"Cannot cast array data from PhysicalDType.* according to the rule 'safe'",
    ):
        _ = arr.astype(PhysicalDType({"T": 1}), casting="safe")

    arr.astype(PhysicalDType({"T": 1}), casting="unsafe")
