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
#include "physical_dimension_ops.h"

/*
 * Returns:
 *   new reference  - conversion succeeded
 *   NULL + error   - conversion failed
 *   NULL + no error - unsupported type
 */
static PhysicalScalarObject *
try_cast_to_new_physical_scalar(PyObject *obj)
{
    if (PyObject_TypeCheck(obj, &PhysicalScalar_Type)) {
        return (PhysicalScalarObject *)Py_NewRef(obj);
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
    PhysicalScalarObject *scalar_a = NULL;
    PhysicalScalarObject *scalar_b = NULL;
    PhysicalDimensionObject *result_dim = NULL;
    PyObject *op_value = NULL;
    PhysicalScalarObject *return_value = NULL;

    scalar_a = try_cast_to_new_physical_scalar(a);
    if (scalar_a == NULL) {
        goto error;
    }

    scalar_b = try_cast_to_new_physical_scalar(b);
    if (scalar_b == NULL) {
        goto error;
    }

    result_dim = physical_dimension_resolve_add(scalar_a->physical_dimension,
                                                scalar_b->physical_dimension);
    if (result_dim == NULL) {
        goto finish;
    }

    op_value = PyNumber_Add(scalar_a->value, scalar_b->value);
    if (op_value == NULL) {
        goto finish;
    }

    return_value = PhysicalScalar_raw_new(op_value, result_dim);
    goto finish;

error:
    Py_XDECREF(scalar_a);
    Py_XDECREF(scalar_b);
    if (PyErr_Occurred()) {
        return NULL;
    }
    else {
        Py_RETURN_NOTIMPLEMENTED;
    }

finish:
    Py_XDECREF(scalar_a);
    Py_XDECREF(scalar_b);
    Py_XDECREF(result_dim);
    Py_XDECREF(op_value);
    return (PyObject *)return_value;
}

static PyObject *
physical_scalar_subtract(PyObject *a, PyObject *b)
{
    PhysicalScalarObject *scalar_a = NULL;
    PhysicalScalarObject *scalar_b = NULL;
    PhysicalDimensionObject *result_dim = NULL;
    PyObject *op_value = NULL;
    PhysicalScalarObject *return_value = NULL;

    scalar_a = try_cast_to_new_physical_scalar(a);
    if (scalar_a == NULL) {
        goto error;
    }
    scalar_b = try_cast_to_new_physical_scalar(b);
    if (scalar_b == NULL) {
        goto error;
    }

    result_dim = physical_dimension_resolve_subtract(scalar_a->physical_dimension,
                                                     scalar_b->physical_dimension);
    if (result_dim == NULL) {
        goto finish;
    }

    op_value = PyNumber_Subtract(scalar_a->value, scalar_b->value);
    if (op_value == NULL) {
        goto finish;
    }

    return_value = PhysicalScalar_raw_new(op_value, result_dim);
    goto finish;

error:
    Py_XDECREF(scalar_a);
    Py_XDECREF(scalar_b);
    if (PyErr_Occurred()) {
        return NULL;
    }
    else {
        Py_RETURN_NOTIMPLEMENTED;
    }

finish:
    Py_XDECREF(scalar_a);
    Py_XDECREF(scalar_b);
    Py_XDECREF(result_dim);
    Py_XDECREF(op_value);
    return (PyObject *)return_value;
}

static PyObject *
physical_scalar_multiply(PyObject *a, PyObject *b)
{
    PhysicalScalarObject *scalar_a = NULL;
    PhysicalScalarObject *scalar_b = NULL;
    PhysicalDimensionObject *result_dim = NULL;
    PyObject *op_value = NULL;
    PhysicalScalarObject *return_value = NULL;

    scalar_a = try_cast_to_new_physical_scalar(a);
    if (scalar_a == NULL) {
        goto error;
    }
    scalar_b = try_cast_to_new_physical_scalar(b);
    if (scalar_b == NULL) {
        goto error;
    }

    result_dim = physical_dimension_resolve_multiply(scalar_a->physical_dimension,
                                                     scalar_b->physical_dimension);
    if (result_dim == NULL) {
        goto finish;
    }

    op_value = PyNumber_Multiply(scalar_a->value, scalar_b->value);
    if (op_value == NULL) {
        goto finish;
    }

    return_value = PhysicalScalar_raw_new(op_value, result_dim);

finish:
    Py_XDECREF(scalar_a);
    Py_XDECREF(scalar_b);
    Py_XDECREF(result_dim);
    Py_XDECREF(op_value);
    return (PyObject *)return_value;

error:
    Py_XDECREF(scalar_a);
    Py_XDECREF(scalar_b);
    if (PyErr_Occurred()) {
        return NULL;
    }
    else {
        Py_RETURN_NOTIMPLEMENTED;
    }
}

static PyObject *
physical_scalar_divide(PyObject *a, PyObject *b)
{
    PhysicalScalarObject *scalar_a = NULL;
    PhysicalScalarObject *scalar_b = NULL;
    PhysicalDimensionObject *result_dim = NULL;
    PyObject *op_value = NULL;
    PhysicalScalarObject *return_value = NULL;

    scalar_a = try_cast_to_new_physical_scalar(a);
    if (scalar_a == NULL) {
        goto error;
    }
    scalar_b = try_cast_to_new_physical_scalar(b);
    if (scalar_b == NULL) {
        goto error;
    }

    result_dim = physical_dimension_resolve_truediv(scalar_a->physical_dimension,
                                                    scalar_b->physical_dimension);
    if (result_dim == NULL) {
        goto finish;
    }

    op_value = PyNumber_TrueDivide(scalar_a->value, scalar_b->value);
    if (op_value == NULL) {
        goto finish;
    }

    return_value = PhysicalScalar_raw_new(op_value, result_dim);

finish:
    Py_XDECREF(scalar_a);
    Py_XDECREF(scalar_b);
    Py_XDECREF(result_dim);
    Py_XDECREF(op_value);
    return (PyObject *)return_value;

error:
    Py_XDECREF(scalar_a);
    Py_XDECREF(scalar_b);
    if (PyErr_Occurred()) {
        return NULL;
    }
    else {
        Py_RETURN_NOTIMPLEMENTED;
    }
}

PyNumberMethods PhysicalScalarObject_as_scalar = {
        .nb_add = (binaryfunc)physical_scalar_add,
        .nb_subtract = (binaryfunc)physical_scalar_subtract,
        .nb_multiply = (binaryfunc)physical_scalar_multiply,
        .nb_remainder = NULL,
        .nb_true_divide = (binaryfunc)physical_scalar_divide,
};
