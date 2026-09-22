#ifndef _PHYSICALDTYPE_SCALAR_H
#define _PHYSICALDTYPE_SCALAR_H

#include <Python.h>
#include "physical_dimension.h"

typedef struct {
    PyObject_HEAD
    double value;                                 // The scalar value
    PhysicalDimensionObject *physical_dimension;  // Associated physical dimension
} PhysicalScalarObject;

extern PyTypeObject PhysicalScalar_Type;

PhysicalScalarObject *
PhysicalScalar_raw_new(double value, PhysicalDimensionObject *physical_dimension);

int
init_physical_scalar(void);

#endif