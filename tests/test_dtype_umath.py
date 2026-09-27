import numpy as np
import pytest

from typing import Callable, Any
import operator
from physicaldtype import PhysicalDimension, physical_dimension_names, PhysicalDType
from hypothesis import given, assume
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
def test__physical_dtype_mul(exponents: list[float], values: list[float]):
    phy_dim = {name: exp for name, exp in zip(physical_dimension_names, exponents)}
    arr1 = np.array(values, dtype=PhysicalDType(phy_dim))
    arr2 = np.array(values, dtype=PhysicalDType(phy_dim))

    result: np.ndarray = arr1 * arr2
    dtype = result.dtype
    assert isinstance(dtype, PhysicalDType)
    assert dtype.physical_dimension.exponents == tuple(
        exponents[i] + exponents[i] for i in range(len(exponents))
    )
