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

    /** Storage dtype descriptor. */
    PyArray_Descr *storage_descr;

} PhysicalDTypeObject;

extern PyArray_DTypeMeta PhysicalFloat64DType;
extern PyArray_DTypeMeta PhysicalFloat32DType;

PhysicalDTypeObject *
new_physicaldtype_instance(PhysicalDimensionObject *physical_dimension,
                           PyArray_Descr *storage_descr);

PyTypeObject *
get_physicaldtype_type(PyArray_Descr *storage_descr);

int
init_physical_dtype(void);

#endif
