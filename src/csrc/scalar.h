#ifndef _PHYSICALDTYPE_SCALAR_H
#define _PHYSICALDTYPE_SCALAR_H

#include <Python.h>
#include <stdbool.h>
#include "physical_dimension.h"

typedef struct {
    PyObject_HEAD
    PyObject *value;                              // NumPy scalar
    PhysicalDimensionObject *physical_dimension;  // Associated physical dimension
} PhysicalScalarObject;

extern PyTypeObject PhysicalFloat64Scalar_Type;
extern PyTypeObject PhysicalFloat32Scalar_Type;

PhysicalScalarObject *
PhysicalScalar_raw_new(PyTypeObject *type, PyObject *value,
                       PhysicalDimensionObject *physical_dimension);

int
init_physical_scalar(void);

bool
is_physical_scalar(PyObject *obj);

PyTypeObject *
get_physicalscalar_type(PyArray_Descr *storage_descr);

#endif
