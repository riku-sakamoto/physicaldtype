#ifndef _PHYSICALDTYPE_CASTS_H
#define _PHYSICALDTYPE_CASTS_H

#include <stdbool.h>
#include <Python.h>
#include <numpy/ndarrayobject.h>

PyArrayMethod_Spec **
init_casts(PyArray_DTypeMeta *dtype_meta, bool include_mutual_casts);

void
free_casts(PyArrayMethod_Spec **casts);

#endif /* _NPY_CASTS_H */
