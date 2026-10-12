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
                       PyArray_DTypeMeta *const NPY_UNUSED(wrapped_dtypes[]),
                       PyArray_Descr *const given_descrs[],
                       PyArray_Descr * new_descrs[])
{
    for (int i = 0; i < nin + nout; i++) {
        if (given_descrs[i] == NULL) {
            new_descrs[i] = NULL;
        }
        else {
            PhysicalDTypeObject *phys_dtype = (PhysicalDTypeObject *)given_descrs[i];
            new_descrs[i] = (PyArray_Descr *)Py_NewRef(phys_dtype->storage_descr);
        }
    }
    return 0;
}

PhysicalDTypeObject *translate_loop_descrs_impl(PyArray_Descr *const given_descrs[], PhysicalDimensionResolver *resolver)
{
    PhysicalDTypeObject *left =
        (PhysicalDTypeObject *)given_descrs[0];
    PhysicalDTypeObject *right =
        (PhysicalDTypeObject *)given_descrs[1];

    PhysicalDimensionObject *result_dim = resolver(left->physical_dimension, right->physical_dimension);
    if (result_dim == NULL) {
        return NULL;
    }

    // Resolve stroage dtype
    PyArray_Descr *given_storage_descrs[2] = {left->storage_descr, right->storage_descr};
    PyArray_Descr *result_storage = PyArray_ResultType(0, NULL, 2, given_storage_descrs);
    if (result_storage == NULL) {
        Py_DECREF(result_dim);
        return NULL;
    }

    PhysicalDTypeObject *result_dtype = new_physicaldtype_instance(result_dim, result_storage);
    Py_DECREF(result_dim);
    Py_DECREF(result_storage);

    if (result_dtype == NULL) {
        return NULL;
    }

    return result_dtype;
}

static int
translate_add_loop_descrs(int NPY_UNUSED(nin), int NPY_UNUSED(nout),
                          PyArray_DTypeMeta *const NPY_UNUSED(new_dtypes[]),
                          PyArray_Descr *const given_descrs[],
                          PyArray_Descr *NPY_UNUSED(original_descrs[]),
                          PyArray_Descr *loop_descrs[])
{
    PhysicalDTypeObject *result_dtype =
            translate_loop_descrs_impl(given_descrs, &physical_dimension_resolve_add);
    if (result_dtype == NULL) {
        return -1;
    }

    loop_descrs[0] = (PyArray_Descr *)Py_NewRef(given_descrs[0]);
    loop_descrs[1] = (PyArray_Descr *)Py_NewRef(given_descrs[1]);
    loop_descrs[2] = (PyArray_Descr *)result_dtype;

    return 0;
}

static int
translate_multiply_loop_descrs(int NPY_UNUSED(nin), int NPY_UNUSED(nout),
                               PyArray_DTypeMeta *const NPY_UNUSED(new_dtypes[]),
                               PyArray_Descr *const given_descrs[],
                               PyArray_Descr *NPY_UNUSED(original_descrs[]),
                               PyArray_Descr *loop_descrs[])
{
    PhysicalDTypeObject *result_dtype =
            translate_loop_descrs_impl(given_descrs, &physical_dimension_resolve_multiply);
    if (result_dtype == NULL) {
        return -1;
    }

    loop_descrs[0] = (PyArray_Descr *)Py_NewRef(given_descrs[0]);
    loop_descrs[1] = (PyArray_Descr *)Py_NewRef(given_descrs[1]);
    loop_descrs[2] = (PyArray_Descr *)result_dtype;

    return 0;
}

static int translate_subtract_loop_descrs(int NPY_UNUSED(nin), int NPY_UNUSED(nout),
                               PyArray_DTypeMeta *const NPY_UNUSED(new_dtypes[]),
                               PyArray_Descr *const given_descrs[],
                               PyArray_Descr *NPY_UNUSED(original_descrs[]),
                               PyArray_Descr *loop_descrs[])
{
    PhysicalDTypeObject *result_dtype =
            translate_loop_descrs_impl(given_descrs, &physical_dimension_resolve_subtract);
    if (result_dtype == NULL) {
        return -1;
    }

    loop_descrs[0] = (PyArray_Descr *)Py_NewRef(given_descrs[0]);
    loop_descrs[1] = (PyArray_Descr *)Py_NewRef(given_descrs[1]);
    loop_descrs[2] = (PyArray_Descr *)result_dtype;

    return 0;
}

static int translate_truediv_loop_descrs(int NPY_UNUSED(nin), int NPY_UNUSED(nout),
                               PyArray_DTypeMeta *const NPY_UNUSED(new_dtypes[]),
                               PyArray_Descr *const given_descrs[],
                               PyArray_Descr *NPY_UNUSED(original_descrs[]),
                               PyArray_Descr *loop_descrs[])
{
    PhysicalDTypeObject *result_dtype =
            translate_loop_descrs_impl(given_descrs, &physical_dimension_resolve_truediv);
    if (result_dtype == NULL) {
        return -1;
    }

    loop_descrs[0] = (PyArray_Descr *)Py_NewRef(given_descrs[0]);
    loop_descrs[1] = (PyArray_Descr *)Py_NewRef(given_descrs[1]);
    loop_descrs[2] = (PyArray_Descr *)result_dtype;

    return 0;
}

static PyArray_DTypeMeta *physical_float64_dtypes[3] = {&PhysicalFloat64DType, &PhysicalFloat64DType, &PhysicalFloat64DType};
static PyArray_DTypeMeta *physical_float32_dtypes[3] = {&PhysicalFloat32DType, &PhysicalFloat32DType, &PhysicalFloat32DType};


int
_register_impl(PyObject *ufunc, PyArray_DTypeMeta** dtypes, PyArrayMethod_TranslateLoopDescriptors *translate_loop_descrs, int npy_type)
{
    PyArray_Descr *descr = PyArray_DescrFromType(npy_type);
    if (descr == NULL) {
        return -1;
    }

    PyArray_DTypeMeta *dtype = NPY_DTYPE(descr);

    PyArray_DTypeMeta *wrapped_dtypes[3] = {
            dtype,
            dtype,
            dtype,
    };

    int res = PyUFunc_AddWrappingLoop(ufunc, dtypes, wrapped_dtypes, &translate_given_descrs,
                            translate_loop_descrs);

    Py_DECREF(descr);
    if (res < 0) {
        return -1;
    }

    return 0;
}

int
register_impl(PyObject *ufunc, PyArrayMethod_TranslateLoopDescriptors *translate_loop_descrs){

    if (_register_impl(ufunc, physical_float64_dtypes, translate_loop_descrs, NPY_DOUBLE) < 0) {
        return -1;
    }

    if (_register_impl(ufunc, physical_float32_dtypes, translate_loop_descrs, NPY_FLOAT32) < 0) {
        return -1;
    }

    return 0;
}

int register_add(PyObject *numpy)
{
    PyObject *add = PyObject_GetAttrString(numpy, "add");
    if (add == NULL) {
        return -1;
    }

    int res = register_impl(add, &translate_add_loop_descrs);
    Py_DECREF(add);
    if (res < 0) {
        return -1;
    }
    return 0;
}


int register_multiply(PyObject *numpy)
{
    PyObject *multiply = PyObject_GetAttrString(numpy, "multiply");
    if (multiply == NULL) {
        return -1;
    }

    int res = register_impl(multiply, &translate_multiply_loop_descrs);
    Py_DECREF(multiply);
    if (res < 0) {
        return -1;
    }
    return 0;
}

int register_subtract(PyObject *numpy)
{
    PyObject *subtract = PyObject_GetAttrString(numpy, "subtract");
    if (subtract == NULL) {
        return -1;
    }

    int res = register_impl(subtract, &translate_subtract_loop_descrs);
    Py_DECREF(subtract);
    if (res < 0) {
        return -1;
    }
    return 0;
}

int register_truediv(PyObject *numpy)
{
    PyObject *truediv = PyObject_GetAttrString(numpy, "true_divide");
    if (truediv == NULL) {
        return -1;
    }

    int res = register_impl(truediv, &translate_truediv_loop_descrs);
    Py_DECREF(truediv);
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

    if(register_subtract(numpy) < 0){
        Py_DECREF(numpy);
        return -1;
    }

    if(register_truediv(numpy) < 0){
        Py_DECREF(numpy);
        return -1;
    }
    Py_DECREF(numpy);
    return 0;

}
