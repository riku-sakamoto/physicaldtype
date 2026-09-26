#include <Python.h>
#include <math.h>
#include <stdbool.h>

#include "physical_dimension_ops.h"
#include "physical_dimension.h"

double TOLERANCE = 1e-12;  // Tolerance for floating-point comparison

bool
physical_dimension_equal(PhysicalDimensionObject *dim1, PhysicalDimensionObject *dim2)
{
    for (int i = 0; i < DIM_COUNT; i++) {
        if (fabs(dim1->exponents[i] - dim2->exponents[i]) > TOLERANCE) {
            return false;
        }
    }
    return true;
}

PhysicalDimensionObject *
physical_dimension_resolve_add(PhysicalDimensionObject *dim1, PhysicalDimensionObject *dim2)
{
    if (!physical_dimension_equal(dim1, dim2)) {
        PyErr_Format(PyExc_ValueError, "Cannot add dimensions: %s and %s are not compatible", dim1,
                     dim2);
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
physical_dimension_resolve_subtract(PhysicalDimensionObject *dim1, PhysicalDimensionObject *dim2)
{
    if (!physical_dimension_equal(dim1, dim2)) {
        PyErr_Format(PyExc_ValueError, "Cannot add dimensions: %s and %s are not compatible", dim1,
                     dim2);
        return NULL;
    }

    PhysicalDimensionObject *result = PhysicalDimension_raw_new(NULL);
    if (result == NULL) {
        return NULL;
    }

    for (int i = 0; i < DIM_COUNT; i++) {
        result->exponents[i] = dim1->exponents[i];
    }
}

PhysicalDimensionObject *
physical_dimension_resolve_multiply(PhysicalDimensionObject *dim1, PhysicalDimensionObject *dim2)
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
physical_dimension_resolve_truediv(PhysicalDimensionObject *dim1, PhysicalDimensionObject *dim2)
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
