#ifndef _PHYSICALDTYPE_CASTS_H
#define _PHYSICALDTYPE_CASTS_H

#include <Python.h>
#include <numpy/ndarrayobject.h>

PyArrayMethod_Spec **
init_casts(void);

void
free_casts(PyArrayMethod_Spec **casts);

#endif /* _NPY_CASTS_H */
