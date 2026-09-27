#include <Python.h>

#define PY_ARRAY_UNIQUE_SYMBOL physicaldtype_ARRAY_API
#define NPY_NO_DEPRECATED_API NPY_2_0_API_VERSION
#define NPY_TARGET_VERSION NPY_2_4_API_VERSION
#define NO_IMPORT_ARRAY

#include "numpy/arrayobject.h"
#include "numpy/ndarraytypes.h"
#include "numpy/dtype_api.h"

#include "scalar.h"
#include "scalar_ops.h"
#include "physical_dimension.h"

/*
　This function creates a new PhysicalScalarObject with the given value and physical dimension.
  NOTE: This function create new references to both value and physical_dimension.
 */
PhysicalScalarObject *
PhysicalScalar_raw_new(PyObject *value, PhysicalDimensionObject *physical_dimension)
{
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "value argument is required");
        return NULL;
    }

    if (PyArray_CheckScalar(value) == 0) {
        PyErr_SetString(PyExc_TypeError, "value must be a NumPy scalar");
        return NULL;
    }
    PyArray_Descr *descr = PyArray_DescrFromScalar(value);
    if (descr == NULL) {
        return NULL;
    }
    if (PyDataType_ISNUMBER(descr) == 0) {
        Py_DECREF(descr);
        PyErr_SetString(PyExc_TypeError, "value must be a numeric NumPy scalar");
        return NULL;
    }

    PhysicalScalarObject *self = PyObject_New(PhysicalScalarObject, &PhysicalScalar_Type);
    if (self == NULL) {
        Py_DECREF(descr);
        return NULL;
    }

    self->value = Py_NewRef(value);
    self->physical_dimension = (PhysicalDimensionObject *)Py_NewRef(physical_dimension);
    Py_DECREF(descr);
    return self;
}

static PyObject *
PhysicalScalar_new(PyTypeObject *Py_UNUSED(cls), PyObject *args, PyObject *kwargs)
{
    PyObject *value = NULL;
    PyObject *physical_dimension = NULL;
    static char *kwlist[] = {"value", "physical_dimension", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OO", kwlist, &value, &physical_dimension)) {
        return NULL;
    }

    // PhysicalDimensionObject
    PhysicalDimensionObject *dimension = PhysicalDimension_raw_new(physical_dimension);
    if (dimension == NULL) {
        return NULL;
    }

    PyObject *res = (PyObject *)PhysicalScalar_raw_new(value, dimension);
    Py_DECREF(dimension);
    return res;
}

static void
PhysicalScalar_dealloc(PhysicalScalarObject *self)
{
    Py_XDECREF(self->value);
    Py_XDECREF(self->physical_dimension);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
PhysicalScalarObject_repr(PhysicalScalarObject *self)
{
    PyObject *value_repr = PyObject_Repr(self->value);
    if (value_repr == NULL) {
        return NULL;
    }

    PyObject *dimension_repr = PyObject_Repr((PyObject *)self->physical_dimension);
    if (dimension_repr == NULL) {
        Py_DECREF(value_repr);
        return NULL;
    }

    PyObject *result = PyUnicode_FromFormat("PhysicalScalar(value=%U, dimension=%U)", value_repr,
                                            dimension_repr);

    Py_DECREF(value_repr);
    Py_DECREF(dimension_repr);

    return result;
}

static PyObject *
PhysicalScalarObject_str(PhysicalScalarObject *self)
{
    PyObject *value_str = PyObject_Str(self->value);
    if (value_str == NULL) {
        return NULL;
    }

    PyObject *dimension_str = PyObject_Str((PyObject *)self->physical_dimension);
    if (dimension_str == NULL) {
        Py_DECREF(value_str);
        return NULL;
    }

    PyObject *result = PyUnicode_FromFormat("%U %U", value_str, dimension_str);

    Py_DECREF(value_str);
    Py_DECREF(dimension_str);

    return result;
}

static PyMethodDef PhysicalScalarObject_methods[] = {
        {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyObject *
PhysicalScalarObject_get_value(PhysicalScalarObject *self, void *Py_UNUSED(closure))
{
    Py_INCREF(self->value);
    return self->value;
}

static PyObject *
PhysicalScalarObject_get_dimension(PhysicalScalarObject *self, void *Py_UNUSED(closure))
{
    Py_INCREF(self->physical_dimension);
    return (PyObject *)self->physical_dimension;
}

static PyGetSetDef PhysicalScalarObject_getset[] = {
        {"value", (getter)PhysicalScalarObject_get_value, NULL,
         "Get the value of the PhysicalScalarObject", NULL},
        {"physical_dimension", (getter)PhysicalScalarObject_get_dimension, NULL,
         "Get the physical dimension of the PhysicalScalarObject", NULL},
        // {"dtype", (getter)PhysicalScalarObject_get_dtype, NULL, "Get the dtype of the
        // PhysicalScalarObject", NULL},
        // {"dimensions", (getter)PhysicalScalarObject_get_dimension, NULL, "Get the physical
        // dimension of the PhysicalScalarObject", NULL},
        {NULL} /* Sentinel */
};

PyTypeObject PhysicalScalar_Type = {
        PyVarObject_HEAD_INIT(NULL, 0).tp_name = "_physicaldtype_main.PhysicalScalar",
        .tp_basicsize = sizeof(PhysicalScalarObject),
        .tp_itemsize = 0,
        // This scalar class is immutable, so we don't need to implement tp_init
        .tp_new = PhysicalScalar_new,
        .tp_dealloc = (destructor)PhysicalScalar_dealloc,
        .tp_repr = (reprfunc)PhysicalScalarObject_repr,
        .tp_str = (reprfunc)PhysicalScalarObject_str,
        .tp_as_number = &PhysicalScalarObject_as_scalar,
        .tp_methods = PhysicalScalarObject_methods,
        .tp_getset = PhysicalScalarObject_getset,
};

int
init_physical_scalar(void)
{
    // PhysicalScalar_Type.tp_base = &PyFloatingArrType_Type;
    return PyType_Ready(&PhysicalScalar_Type);
}
