import pytest
import numpy as np

from physicaldtype import physical_dimension_names, PhysicalDType, PhysicalScalar
from hypothesis import given
from hypothesis import strategies as st

from tests.constants import SUPPORTED_DTYPES


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
@pytest.mark.parametrize("storage_dtype", SUPPORTED_DTYPES)
def test__physical_dtype_getitem(
    exponents: list[float], values: list[float], storage_dtype: str
):
    phy_dim = {name: exp for name, exp in zip(physical_dimension_names, exponents)}
    arr = np.array(values, dtype=PhysicalDType[storage_dtype](phy_dim))

    for i in range(len(arr)):
        value = arr[i]
        assert isinstance(value, PhysicalScalar[storage_dtype])

        assert value.physical_dimension.exponents == tuple(exponents)
        np.testing.assert_allclose(value.value, values[i])


@pytest.mark.parametrize("storage_dtype", SUPPORTED_DTYPES)
def test__physical_dtype_invalid_key(storage_dtype: str):
    phy_dim = {"InvalidKey": 1.0}
    with pytest.raises(ValueError, match="Invalid dimension name: InvalidKey"):
        _ = np.array([1.0, 2.0, 3.0], dtype=PhysicalDType[storage_dtype](phy_dim))


@pytest.mark.parametrize("storage_dtype", SUPPORTED_DTYPES)
def test__cannot_convert_to_utf8(storage_dtype: str):
    phy_dim = {"\ud800": 1}
    with pytest.raises(
        UnicodeEncodeError, match="'utf-8' codec can't encode character"
    ):
        _ = np.array([1.0, 2.0, 3.0], dtype=PhysicalDType[storage_dtype](phy_dim))


@pytest.mark.parametrize("storage_dtype", SUPPORTED_DTYPES)
def test__overflow_exponent(storage_dtype: str):
    phy_dim = {"L": 10**1000}
    with pytest.raises(OverflowError, match="int too large to convert to float"):
        _ = np.array([1.0, 2.0, 3.0], dtype=PhysicalDType[storage_dtype](phy_dim))


# HACK: So far, only float64 is supported for casting
@pytest.mark.parametrize("storage_dtype", ["float64"])
def test__cast_from_physical_dtype_to_float64(storage_dtype: str):
    arr = np.array([3.0, 4.0], dtype=PhysicalDType[storage_dtype]({"L": 1}))

    # This should succeed since the physical dimensions are compatible
    _ = arr.astype(storage_dtype, casting="safe")


@pytest.mark.parametrize("storage_dtype", SUPPORTED_DTYPES)
def test__cast_from_physical_dtype_to_physical(storage_dtype: str):
    arr: np.ndarray = np.array([3.0, 4.0], dtype=PhysicalDType[storage_dtype]({"L": 1}))
    with pytest.raises(
        TypeError,
        match=r"Cannot cast array data from PhysicalDType.* according to the rule 'safe'",
    ):
        _ = arr.astype(PhysicalDType[storage_dtype]({"T": 1}), casting="safe")

    arr.astype(PhysicalDType[storage_dtype]({"T": 1}), casting="unsafe")


@given(
    exponents=st.lists(
        st.floats(allow_nan=False, allow_infinity=False, width=32),
        min_size=7,
        max_size=7,
    ),
)
@pytest.mark.parametrize("storage_dtype", SUPPORTED_DTYPES)
def test__immutable_dimensions(exponents: list[float], storage_dtype: str):
    phy_dim = {name: exp for name, exp in zip(physical_dimension_names, exponents)}
    arr = np.array([1.0, 2.0], dtype=PhysicalDType[storage_dtype](phy_dim))

    # Attempt to modify the physical dimension of the array
    _ = arr.dtype.physical_dimension.__init__({"T": 100})

    assert arr.dtype.physical_dimension.exponents == tuple(exponents)


@pytest.mark.parametrize("storage_dtype", SUPPORTED_DTYPES)
def test__initialize_with_storage_dtype(storage_dtype: str):
    phy_dim = {"L": 1, "M": 1, "T": -2}
    dtype = PhysicalDType[storage_dtype](phy_dim)

    assert isinstance(dtype, PhysicalDType[storage_dtype])
    assert dtype.storage_dtype == storage_dtype

    arr = np.array([1.0, 2.0, 3.0], dtype=dtype)

    item = arr[0]
    assert item.value.dtype == storage_dtype
