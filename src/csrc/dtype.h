#ifndef _PHYSICALDTYPE_DTYPE_H
#define _PHYSICALDTYPE_DTYPE_H

#include <numpy/ndarraytypes.h>
#include <numpy/dtype_api.h>

#include "physical_dimension.h"

typedef struct {
    /** Base NumPy dtype descriptor. */
    PyArray_Descr base;

    /** Physical dimension associated with this dtype. */
    PhysicalDimensionObject *physical_dimension;

    /** NumPy dtype descriptor used for the underlying storage. */
    PyArray_Descr *storage_descr;

} PhysicalDTypeObject;

extern PyArray_DTypeMeta PhysicalDType;

PhysicalDTypeObject *
new_physicaldtype_instance(PhysicalDimensionObject *physical_dimension,
                           PyArray_Descr *storage_descr);

int
init_physical_dtype(void);

#endif
