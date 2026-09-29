import numpy as np

from physicaldtype import physical_dimension_names, PhysicalDType
from hypothesis import given
from hypothesis import strategies as st

N_DIM = len(physical_dimension_names)


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
def test__physical_dtype_substract(exponents: list[float], values: list[float]):
    phy_dim = {name: exp for name, exp in zip(physical_dimension_names, exponents)}
    arr1 = np.array(values, dtype=PhysicalDType(phy_dim))
    arr2 = np.array(values, dtype=PhysicalDType(phy_dim))

    result: np.ndarray = arr1 - arr2
    dtype = result.dtype
    assert isinstance(dtype, PhysicalDType)
    assert dtype.physical_dimension.exponents == tuple(exponents)


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
