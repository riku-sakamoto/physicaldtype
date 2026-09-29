#ifndef _PHYSICALDTYPE_DIMENSION_OPS_H
#define _PHYSICALDTYPE_DIMENSION_OPS_H

#include <Python.h>
#include <stdbool.h>

#include "physical_dimension.h"

bool
physical_dimension_equal(const PhysicalDimensionObject *dim1, const PhysicalDimensionObject *dim2);

typedef PhysicalDimensionObject *
PhysicalDimensionResolver(const PhysicalDimensionObject *dim1, const PhysicalDimensionObject *dim2);

PhysicalDimensionObject *
physical_dimension_resolve_add(const PhysicalDimensionObject *dim1,
                               const PhysicalDimensionObject *dim2);

PhysicalDimensionObject *
physical_dimension_resolve_subtract(const PhysicalDimensionObject *dim1,
                                    const PhysicalDimensionObject *dim2);

PhysicalDimensionObject *
physical_dimension_resolve_multiply(const PhysicalDimensionObject *dim1,
                                    const PhysicalDimensionObject *dim2);

PhysicalDimensionObject *
physical_dimension_resolve_truediv(const PhysicalDimensionObject *dim1,
                                   const PhysicalDimensionObject *dim2);

#endif