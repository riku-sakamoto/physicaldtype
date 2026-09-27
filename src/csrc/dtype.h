#ifndef _PHYSICALDTYPE_DTYPE_H
#define _PHYSICALDTYPE_DTYPE_H

#include <numpy/ndarraytypes.h>
#include <numpy/dtype_api.h>

#include "physical_dimension.h"

typedef struct {
    PyArray_Descr base;

    PhysicalDimensionObject *physical_dimension;

    // In the future, we might want to add a storage descriptor
    // to handle different storage types (e.g., float32, float64) for the physical dtype.
    // PyArray_Descr *storage_descr;

} PhysicalDTypeObject;

extern PyArray_DTypeMeta PhysicalDType;

PhysicalDTypeObject *
new_physicaldtype_instance(PhysicalDimensionObject *physical_dimension);

int
init_physical_dtype(void);

#endif
