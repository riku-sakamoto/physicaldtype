#ifndef _PHYSICALDTYPE_DIMENSION_H
#define _PHYSICALDTYPE_DIMENSION_H

#include <Python.h>

typedef enum {
    DIM_LENGTH = 0,          // L
    DIM_MASS,                // M
    DIM_TIME,                // T
    DIM_CURRENT,             // I
    DIM_TEMPERATURE,         // Θ
    DIM_AMOUNT,              // N
    DIM_LUMINOUS_INTENSITY,  // J

    DIM_COUNT  // Total number of dimensions
} PhysicalDimensionKindType;

typedef struct {
    PyObject_HEAD
    double exponents[DIM_COUNT];  // Exponents for each dimension
    // PyObject* units[DIM_COUNT];  // Python str: "m", "kg", "s"
} PhysicalDimensionObject;

extern PyTypeObject PhysicalDimensionObjectType;

extern PhysicalDimensionObject *
PhysicalDimension_raw_new(PyObject *dimensions);

#endif
