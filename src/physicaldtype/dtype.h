#ifndef _PHYSICALDTYPE_DTYPE_H
#define _PHYSICALDTYPE_DTYPE_H


#include <numpy/ndarraytypes.h>
#include <numpy/dtype_api.h>

#include "physical_dimension.h"


typedef struct {
    PyArray_Descr base;
    PhysicalDimensionObject *physical_dimension;
} PhysicalDTypeObject;

extern PyArray_DTypeMeta PhysicalDType;

PhysicalDTypeObject *
new_physicaldtype_instance(PhysicalDimensionObject *physical_dimension);

int
init_physical_dtype(void);

#endif
