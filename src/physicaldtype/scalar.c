#include <Python.h>

#define PY_ARRAY_UNIQUE_SYMBOL physicaldtype_ARRAY_API
#define NPY_NO_DEPRECATED_API NPY_2_0_API_VERSION
#define NO_IMPORT_ARRAY

#include "numpy/arrayobject.h"
#include "numpy/ndarraytypes.h"
#include "numpy/dtype_api.h"

#include "scalar.h"
#include "physical_dimension.h"

PhysicalScalarObject *
PhysicalScalar_raw_new(double value, PhysicalDimensionObject *physical_dimension)
{
    PhysicalScalarObject *self = PyObject_New(PhysicalScalarObject, &PhysicalScalar_Type);
    if (self == NULL) {
        return NULL;
    }

    self->value = value;
    Py_INCREF(physical_dimension);
    self->physical_dimension = physical_dimension;
    return self;
}

static PyObject *
PhysicalScalar_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    PyObject *value = NULL;
    PyObject *physical_dimension = NULL;
    static char *kwlist[] = {"value", "dimension", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OO", kwlist, &value, &physical_dimension)) {
        return NULL;
    }

    // PhysicalDimensionObject
    PhysicalDimensionObject *dimension = PhysicalDimension_raw_new(physical_dimension);
    if (dimension == NULL) {
        return NULL;
    }

    if (value == NULL) {
        value = PyFloat_FromDouble(0.0);
    }

    // TODO: Validate that value is a float or int

    double value_double = PyFloat_AsDouble(value);
    return (PyObject *)PhysicalScalar_raw_new(value_double, dimension);
}

static void
PhysicalScalar_dealloc(PhysicalScalarObject *self)
{
    Py_XDECREF(self->physical_dimension);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyMethodDef PhysicalScalarObject_methods[] = {
        {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyGetSetDef PhysicalScalarObject_getset[] = {
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
        .tp_methods = PhysicalScalarObject_methods,
        .tp_getset = PhysicalScalarObject_getset,
};

int
init_physical_scalar(void)
{
    PhysicalScalar_Type.tp_base = &PyFloatingArrType_Type;
    return PyType_Ready(&PhysicalScalar_Type);
}
