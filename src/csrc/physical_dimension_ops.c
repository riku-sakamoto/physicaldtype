#include <Python.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>

#include "physical_dimension_ops.h"
#include "physical_dimension.h"

#define PHYSICAL_DIMENSION_TOLERANCE (1e-14)  // Tolerance for comparing floating-point exponents

bool
physical_dimension_equal(const PhysicalDimensionObject *dim1, const PhysicalDimensionObject *dim2)
{
    for (int i = 0; i < DIM_COUNT; i++) {
        if (fabs(dim1->exponents[i] - dim2->exponents[i]) > PHYSICAL_DIMENSION_TOLERANCE) {
            return false;
        }
    }
    return true;
}

PhysicalDimensionObject *
physical_dimension_resolve_add(const PhysicalDimensionObject *dim1,
                               const PhysicalDimensionObject *dim2)
{
    if (!physical_dimension_equal(dim1, dim2)) {
        PyErr_Format(PyExc_ValueError,
                     "Incompatible physical dimensions for addition. "
                     "dim1=%R and dim2=%R",
                     dim1, dim2);
        return NULL;
    }

    PhysicalDimensionObject *result = PhysicalDimension_raw_new(NULL);
    if (result == NULL) {
        return NULL;
    }

    for (int i = 0; i < DIM_COUNT; i++) {
        result->exponents[i] = dim1->exponents[i];
    }

    return result;
}

PhysicalDimensionObject *
physical_dimension_resolve_subtract(const PhysicalDimensionObject *dim1,
                                    const PhysicalDimensionObject *dim2)
{
    if (!physical_dimension_equal(dim1, dim2)) {
        PyErr_Format(PyExc_ValueError,
                     "Incompatible physical dimensions for subtraction. "
                     "dim1=%R and dim2=%R",
                     dim1, dim2);
        return NULL;
    }

    PhysicalDimensionObject *result = PhysicalDimension_raw_new(NULL);
    if (result == NULL) {
        return NULL;
    }

    for (int i = 0; i < DIM_COUNT; i++) {
        result->exponents[i] = dim1->exponents[i];
    }

    return result;
}

PhysicalDimensionObject *
physical_dimension_resolve_multiply(const PhysicalDimensionObject *dim1,
                                    const PhysicalDimensionObject *dim2)
{
    PhysicalDimensionObject *result = PhysicalDimension_raw_new(NULL);
    if (result == NULL) {
        return NULL;
    }

    for (int i = 0; i < DIM_COUNT; i++) {
        result->exponents[i] = dim1->exponents[i] + dim2->exponents[i];
    }

    return result;
}

PhysicalDimensionObject *
physical_dimension_resolve_truediv(const PhysicalDimensionObject *dim1,
                                   const PhysicalDimensionObject *dim2)
{
    PhysicalDimensionObject *result = PhysicalDimension_raw_new(NULL);
    if (result == NULL) {
        return NULL;
    }

    for (int i = 0; i < DIM_COUNT; i++) {
        result->exponents[i] = dim1->exponents[i] - dim2->exponents[i];
    }

    return result;
}
