#include <Python.h>

#define PY_ARRAY_UNIQUE_SYMBOL physicaldtype_ARRAY_API
#define PY_UFUNC_UNIQUE_SYMBOL physicaldtype_UFUNC_API
#define NPY_NO_DEPRECATED_API NPY_2_0_API_VERSION
#define NPY_TARGET_VERSION NPY_2_4_API_VERSION
#define NO_IMPORT_ARRAY
#define NO_IMPORT_UFUNC

#include "numpy/ndarraytypes.h"
#include "numpy/dtype_api.h"
#include "numpy/ufuncobject.h"
#include "numpy/ndarrayobject.h"
#include "physical_arithmetic.h"
#include "../dtype.h"
#include "../physical_dimension.h"
#include "../physical_dimension_ops.h"

static int
translate_given_descrs(int nin, int nout,
                       PyArray_DTypeMeta *NPY_UNUSED(wrapped_dtypes[]),
                       PyArray_Descr *const given_descrs[],
                       PyArray_Descr * new_descrs[])
{
    for (int i = 0; i < nin + nout; i++) {
        if (given_descrs[i] == NULL) {
            new_descrs[i] = NULL;
        }
        else {
            // HACK: Need to use storage_descr in the future
            new_descrs[i] = PyArray_DescrFromType(NPY_DOUBLE);
        }
    }
    return 0;
}

static int
translate_add_loop_descrs(int nin, int NPY_UNUSED(nout),
                      PyArray_DTypeMeta *NPY_UNUSED(new_dtypes[]),
                      PyArray_Descr *const given_descrs[],
                      PyArray_Descr *const original_descrs[],
                      PyArray_Descr *loop_descrs[])
{
    PhysicalDTypeObject *left =
        (PhysicalDTypeObject *)given_descrs[0];
    PhysicalDTypeObject *right =
        (PhysicalDTypeObject *)given_descrs[1];

    PhysicalDimensionObject *result_dim = physical_dimension_resolve_add(left->physical_dimension, right->physical_dimension);
    if (result_dim == NULL) {
        return -1;
    }

    PhysicalDTypeObject *result_dtype = new_physicaldtype_instance(result_dim);
    if (result_dtype == NULL) {
        return -1;
    }
    Py_DECREF(result_dim);

    loop_descrs[0] = (PyArray_Descr *)Py_NewRef(given_descrs[0]);
    loop_descrs[1] = (PyArray_Descr *)Py_NewRef(given_descrs[1]);
    loop_descrs[2] = (PyArray_Descr *)result_dtype;

    return 0;
}

static int
translate_multiply_loop_descrs(int nin, int NPY_UNUSED(nout),
                      PyArray_DTypeMeta *NPY_UNUSED(new_dtypes[]),
                      PyArray_Descr * const given_descrs[],
                      PyArray_Descr * const original_descrs[],
                      PyArray_Descr *loop_descrs[])
{
    PhysicalDTypeObject *left =
        (PhysicalDTypeObject *)given_descrs[0];
    PhysicalDTypeObject *right =
        (PhysicalDTypeObject *)given_descrs[1];

    PhysicalDimensionObject *result_dim = physical_dimension_resolve_multiply(left->physical_dimension, right->physical_dimension);
    if (result_dim == NULL) {
        return -1;
    }

    PhysicalDTypeObject *result_dtype = new_physicaldtype_instance(result_dim);

    loop_descrs[0] = (PyArray_Descr *)Py_NewRef(given_descrs[0]);
    loop_descrs[1] = (PyArray_Descr *)Py_NewRef(given_descrs[1]);
    loop_descrs[2] = (PyArray_Descr *)result_dtype;

    return 0;
}

PyArray_DTypeMeta *physical_dtypes[3] = {&PhysicalDType, &PhysicalDType, &PhysicalDType};

int register_add(PyObject *numpy)
{
    PyObject *add = PyObject_GetAttrString(numpy, "add");

    PyArray_Descr *float64_descr = PyArray_DescrFromType(NPY_FLOAT64);
    if (float64_descr == NULL) {
        return -1;
    }

    PyArray_DTypeMeta *float64_dtype = NPY_DTYPE(float64_descr);

    PyArray_DTypeMeta *wrapped_dtypes[3] = {
            float64_dtype,
            float64_dtype,
            float64_dtype,
    };

    int res = PyUFunc_AddWrappingLoop(add, physical_dtypes, wrapped_dtypes, &translate_given_descrs,
                            &translate_add_loop_descrs);

    Py_DECREF(float64_descr);
    Py_DECREF(add);
    if (res < 0) {
        return -1;
    }

    return 0;
}


int register_multiply(PyObject *numpy)
{
    PyObject *multiply = PyObject_GetAttrString(numpy, "multiply");

    PyArray_Descr *float64_descr = PyArray_DescrFromType(NPY_FLOAT64);
    if (float64_descr == NULL) {
        return -1;
    }

    PyArray_DTypeMeta *float64_dtype = NPY_DTYPE(float64_descr);

    PyArray_DTypeMeta *wrapped_dtypes[3] = {
            float64_dtype,
            float64_dtype,
            float64_dtype,
    };

    int res = PyUFunc_AddWrappingLoop(multiply, physical_dtypes, wrapped_dtypes, &translate_given_descrs,
                            &translate_multiply_loop_descrs);

    Py_DECREF(float64_descr);
    if (res < 0) {
        return -1;
    }

    return 0;
}


int PhysicalDType_InitArithmeticUFuncs(void){

    PyObject *numpy = PyImport_ImportModule("numpy");
    if (numpy == NULL) {
        return -1;
    }

    if(register_add(numpy) < 0){
        Py_DECREF(numpy);
        return -1;
    }

    if(register_multiply(numpy) < 0){
        Py_DECREF(numpy);
        return -1;
    }

    Py_DECREF(numpy);
    return 0;

}
