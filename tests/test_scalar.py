import numpy as np
import pytest

from typing import Callable, Any
import operator
from physicaldtype import PhysicalScalar, PhysicalDimension, physical_dimension_names
from hypothesis import given, assume
from hypothesis import strategies as st


@given(
    exponents=st.lists(
        st.floats(allow_nan=False, allow_infinity=False), min_size=7, max_size=7
    ),
    value=st.floats(allow_nan=False, allow_infinity=False),
)
def test__physical_scalar_can_initialize(exponents: list[float], value: float):
    phy_dim = {dim: exp for dim, exp in zip(physical_dimension_names, exponents)}
    scalar = PhysicalScalar(np.float64(value), phy_dim)

    assert isinstance(scalar.value, float)
    assert isinstance(scalar.physical_dimension, PhysicalDimension)

    for dim in physical_dimension_names:
        assert scalar.physical_dimension.get_exponent(dim) == phy_dim[dim]


@pytest.mark.parametrize(
    "scalar_op, desired_dimension_op",
    [
        (operator.add, None),
        (operator.sub, None),
        (operator.mul, operator.add),
        (operator.truediv, operator.sub),
    ],
)
@given(
    exponents=st.lists(
        st.floats(allow_nan=False, allow_infinity=False), min_size=7, max_size=7
    ),
    value1=st.floats(allow_nan=False, allow_infinity=False, width=32),
    value2=st.floats(allow_nan=False, allow_infinity=False, width=32),
)
def test__physical_scalar_arithmetic_operations(
    scalar_op: Callable[[Any, Any], Any],
    desired_dimension_op: Callable[[Any, Any], Any] | None,
    exponents: list[float],
    value1: float,
    value2: float,
):
    if scalar_op is operator.truediv:
        assume(value2 != 0.0)

    phy_dim = {dim: exp for dim, exp in zip(physical_dimension_names, exponents)}
    scalar1 = PhysicalScalar(np.float64(value1), phy_dim)
    scalar2 = PhysicalScalar(np.float64(value2), phy_dim)

    res = scalar_op(scalar1, scalar2)

    assert isinstance(res, PhysicalScalar)
    assert res.value == scalar_op(scalar1.value, scalar2.value)

    if desired_dimension_op is None:
        assert res.physical_dimension.exponents == scalar1.physical_dimension.exponents


def test__scalar_failed_divide():
    scalar1 = PhysicalScalar(np.float64(1.0), {"L": 1})
    scalar2 = PhysicalScalar(np.float64(0.0), {"L": 1})

    with pytest.raises(FloatingPointError, match="divide by zero"):
        with np.errstate(divide="raise"):
            _ = scalar1 / scalar2


@pytest.mark.parametrize(
    "scalar_op", [operator.add, operator.sub, operator.mul, operator.truediv]
)
def test__scalar_not_implemented(scalar_op: Callable[[Any, Any], Any]):
    scalar1 = PhysicalScalar(np.float64(1.0), {"L": 1})

    with pytest.raises(
        TypeError,
        match="unsupported operand type",
    ):
        _ = scalar1 + object()  # type: ignore
