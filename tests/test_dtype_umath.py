import numpy as np
import pytest

from physicaldtype import physical_dimension_names, PhysicalDType
from hypothesis import given, assume
from hypothesis import strategies as st

N_DIM = len(physical_dimension_names)
TOLERANCE = 1e-14  # Tolerance for comparing floating-point exponents


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
def test__physical_dtype_add(exponents: list[float], values: list[float]):
    phy_dim = {name: exp for name, exp in zip(physical_dimension_names, exponents)}
    arr1 = np.array(values, dtype=PhysicalDType(phy_dim))
    arr2 = np.array(values, dtype=PhysicalDType(phy_dim))

    result: np.ndarray = arr1 + arr2
    dtype = result.dtype
    assert isinstance(dtype, PhysicalDType)
    assert dtype.physical_dimension.exponents == tuple(exponents)


@given(
    exponents1=st.lists(
        st.floats(allow_nan=False, allow_infinity=False, width=32),
        min_size=7,
        max_size=7,
    ),
    exponents2=st.lists(
        st.floats(allow_nan=False, allow_infinity=False, width=32),
        min_size=7,
        max_size=7,
    ),
)
def test__physical_dtype_add_invalid(exponents1: list[float], exponents2: list[float]):
    assume(any(abs(a - b) > TOLERANCE for a, b in zip(exponents1, exponents2)))
    phy_dim1 = {name: exp for name, exp in zip(physical_dimension_names, exponents1)}
    arr1 = np.array([1.0, 2.0], dtype=PhysicalDType(phy_dim1))
    phy_dim2 = {name: exp for name, exp in zip(physical_dimension_names, exponents2)}
    arr2 = np.array([3.0, 4.0], dtype=PhysicalDType(phy_dim2))

    with pytest.raises(
        ValueError, match="Incompatible physical dimensions for addition"
    ):
        _ = arr1 + arr2


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
def test__physical_dtype_subtract(exponents: list[float], values: list[float]):
    phy_dim = {name: exp for name, exp in zip(physical_dimension_names, exponents)}
    arr1 = np.array(values, dtype=PhysicalDType(phy_dim))
    arr2 = np.array(values, dtype=PhysicalDType(phy_dim))

    result: np.ndarray = arr1 - arr2
    dtype = result.dtype
    assert isinstance(dtype, PhysicalDType)
    assert dtype.physical_dimension.exponents == tuple(exponents)


@given(
    exponents1=st.lists(
        st.floats(allow_nan=False, allow_infinity=False, width=32),
        min_size=7,
        max_size=7,
    ),
    exponents2=st.lists(
        st.floats(allow_nan=False, allow_infinity=False, width=32),
        min_size=7,
        max_size=7,
    ),
)
def test__physical_dtype_subtract_invalid(
    exponents1: list[float], exponents2: list[float]
):
    assume(any(abs(a - b) > TOLERANCE for a, b in zip(exponents1, exponents2)))
    phy_dim1 = {name: exp for name, exp in zip(physical_dimension_names, exponents1)}
    arr1 = np.array([1.0, 2.0], dtype=PhysicalDType(phy_dim1))
    phy_dim2 = {name: exp for name, exp in zip(physical_dimension_names, exponents2)}
    arr2 = np.array([3.0, 4.0], dtype=PhysicalDType(phy_dim2))

    with pytest.raises(
        ValueError, match="Incompatible physical dimensions for subtraction"
    ):
        _ = arr1 - arr2


@given(
    exponents_1=st.lists(
        st.floats(allow_nan=False, allow_infinity=False, width=32),
        min_size=7,
        max_size=7,
    ),
    exponents_2=st.lists(
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
def test__physical_dtype_mul(
    exponents_1: list[float], exponents_2: list[float], values: list[float]
):
    phy_dim1 = {name: exp for name, exp in zip(physical_dimension_names, exponents_1)}
    arr1 = np.array(values, dtype=PhysicalDType(phy_dim1))
    phy_dim2 = {name: exp for name, exp in zip(physical_dimension_names, exponents_2)}
    arr2 = np.array(values, dtype=PhysicalDType(phy_dim2))

    result: np.ndarray = arr1 * arr2
    dtype = result.dtype
    assert isinstance(dtype, PhysicalDType)
    assert dtype.physical_dimension.exponents == tuple(
        exponents_1[i] + exponents_2[i] for i in range(N_DIM)
    )


@given(
    exponents_1=st.lists(
        st.floats(allow_nan=False, allow_infinity=False, width=32),
        min_size=7,
        max_size=7,
    ),
    exponents_2=st.lists(
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
def test__physical_dtype_truediv(
    exponents_1: list[float], exponents_2: list[float], values: list[float]
):
    phy_dim1 = {name: exp for name, exp in zip(physical_dimension_names, exponents_1)}
    arr1 = np.array(values, dtype=PhysicalDType(phy_dim1))
    # To avoid division by zero, we add a small constant to the second array
    phy_dim2 = {name: exp for name, exp in zip(physical_dimension_names, exponents_2)}
    arr2 = np.array(values, dtype=PhysicalDType(phy_dim2)) + np.array(
        1e-5, dtype=PhysicalDType(phy_dim2)
    )

    result: np.ndarray = arr1 / arr2
    dtype = result.dtype
    assert isinstance(dtype, PhysicalDType)
    assert dtype.physical_dimension.exponents == tuple(
        exponents_1[i] - exponents_2[i] for i in range(N_DIM)
    )
