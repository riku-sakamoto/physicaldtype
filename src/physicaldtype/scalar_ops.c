#include <Python.h>

#define PY_ARRAY_UNIQUE_SYMBOL physicaldtype_ARRAY_API
#define NPY_NO_DEPRECATED_API NPY_2_0_API_VERSION
#define NO_IMPORT_ARRAY

#include "numpy/arrayobject.h"
#include "numpy/ndarraytypes.h"
#include "numpy/dtype_api.h"

#include "scalar.h"
#include "scalar_ops.h"
#include "physical_dimension.h"

static PhysicalScalarObject *
try_cast_to_physical_scalar(PyObject *obj)
{
    if (PyObject_TypeCheck(obj, &PhysicalScalar_Type)) {
        return (PhysicalScalarObject *)obj;
    }

    if (PyArray_CheckScalar(obj)) {
        // If it's a NumPy scalar, we can wrap it in a PhysicalScalarObject with a default dimension
        PhysicalDimensionObject *default_dimension = PhysicalDimension_raw_new(NULL);
        if (default_dimension == NULL) {
            return NULL;
        }

        PhysicalScalarObject *new = PhysicalScalar_raw_new(obj, default_dimension);
        Py_DECREF(default_dimension);
        return new;
    }

    return NULL;
}

static PyObject *
physical_scalar_add(PyObject *a, PyObject *b)
{
    PhysicalScalarObject *scalar_a = try_cast_to_physical_scalar(a);
    PhysicalScalarObject *scalar_b = try_cast_to_physical_scalar(b);

    if (scalar_a == NULL || scalar_b == NULL) {
        return NULL;
    }

    PhysicalDimensionObject *result_dim = physical_dimension_resolve_add(
            scalar_a->physical_dimension, scalar_b->physical_dimension);
    if (result_dim == NULL) {
        return NULL;
    }

    PyObject_Print(scalar_a->value, stdout, 0);
    PyObject_Print(scalar_b->value, stdout, 0);

    PyObject *result_value = PyNumber_Add(scalar_a->value, scalar_b->value);

    PyObject *new = PhysicalScalar_raw_new(result_value, result_dim);
    Py_DECREF(result_value);
    Py_DECREF(result_dim);
    return new;
}

static PyObject *
physical_scalar_subtract(PyObject *a, PyObject *b)
{
    PhysicalScalarObject *scalar_a = try_cast_to_physical_scalar(a);
    PhysicalScalarObject *scalar_b = try_cast_to_physical_scalar(b);

    if (scalar_a == NULL || scalar_b == NULL) {
        return NULL;
    }

    PhysicalDimensionObject *result_dim = physical_dimension_resolve_subtract(
            scalar_a->physical_dimension, scalar_b->physical_dimension);
    if (result_dim == NULL) {
        return NULL;
    }

    PyObject *result_value = PyNumber_Subtract(scalar_a->value, scalar_b->value);

    PyObject *new = PhysicalScalar_raw_new(result_value, result_dim);
    Py_DECREF(result_value);
    Py_DECREF(result_dim);
    return new;
}

static PyObject *
physical_scalar_multiply(PyObject *a, PyObject *b)
{
    PhysicalScalarObject *scalar_a = try_cast_to_physical_scalar(a);
    PhysicalScalarObject *scalar_b = try_cast_to_physical_scalar(b);

    if (scalar_a == NULL || scalar_b == NULL) {
        return NULL;
    }

    PhysicalDimensionObject *result_dim = physical_dimension_resolve_multiply(
            scalar_a->physical_dimension, scalar_b->physical_dimension);
    if (result_dim == NULL) {
        return NULL;
    }

    PyObject *result_value = PyNumber_Multiply(scalar_a->value, scalar_b->value);

    PyObject *new = PhysicalScalar_raw_new(result_value, result_dim);
    Py_DECREF(result_value);
    Py_DECREF(result_dim);
    return new;
}

static PyObject *
physical_scalar_divide(PyObject *a, PyObject *b)
{
    PhysicalScalarObject *scalar_a = try_cast_to_physical_scalar(a);
    PhysicalScalarObject *scalar_b = try_cast_to_physical_scalar(b);

    if (scalar_a == NULL || scalar_b == NULL) {
        return NULL;
    }

    PhysicalDimensionObject *result_dim = physical_dimension_resolve_truediv(
            scalar_a->physical_dimension, scalar_b->physical_dimension);
    if (result_dim == NULL) {
        return NULL;
    }

    PyObject *result_value = PyNumber_TrueDivide(scalar_a->value, scalar_b->value);

    PyObject *new = PhysicalScalar_raw_new(result_value, result_dim);
    Py_DECREF(result_value);
    Py_DECREF(result_dim);
    return new;
}

PyNumberMethods PhysicalScalarObject_as_scalar = {
        .nb_add = (binaryfunc)physical_scalar_add,
        .nb_subtract = (binaryfunc)physical_scalar_subtract,
        .nb_multiply = (binaryfunc)physical_scalar_multiply,
        .nb_remainder = NULL,
        .nb_true_divide = (binaryfunc)physical_scalar_divide,
};
