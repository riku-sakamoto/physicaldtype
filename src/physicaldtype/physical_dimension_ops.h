#ifndef _PHYSICALDTYPE_DIMENSION_OPS_H
#define _PHYSICALDTYPE_DIMENSION_OPS_H

#include <Python.h>
#include <stdbool.h>

#include "physical_dimension.h"

bool
physical_dimension_equal(PhysicalDimensionObject *dim1, PhysicalDimensionObject *dim2);

PhysicalDimensionObject *
physical_dimension_add(PhysicalDimensionObject *dim1, PhysicalDimensionObject *dim2);

PhysicalDimensionObject *
physical_dimension_subtract(PhysicalDimensionObject *dim1, PhysicalDimensionObject *dim2);

PhysicalDimensionObject *
physical_dimension_multiply(PhysicalDimensionObject *dim1, PhysicalDimensionObject *dim2);

PhysicalDimensionObject *
physical_dimension_divide(PhysicalDimensionObject *dim1, PhysicalDimensionObject *dim2);

extern PyNumberMethods PhysicalDimensionObject_as_number;

#endif