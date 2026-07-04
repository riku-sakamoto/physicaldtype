#ifndef _PHYSICALDTYPE_DTYPE_H
#define _PHYSICALDTYPE_DTYPE_H


#include <numpy/ndarraytypes.h>
#include <numpy/dtype_api.h>
#include "physical_common.h"


typedef struct {
    PyArray_Descr base;
    PhysicalBackendType unit;
} PhysicalDTypeObject;

extern PyArray_DTypeMeta PhysicalDType;

PhysicalDTypeObject *
new_physicaldtype_instance(PhysicalBackendType backend);

int
init_physical_dtype(void);

#endif
