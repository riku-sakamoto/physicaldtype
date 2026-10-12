#include <Python.h>
#include "stdbool.h"

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

PyTypeObject *
get_physicalscalar_type(PyArray_Descr *storage_descr)
{
    if (storage_descr->type_num == NPY_FLOAT64) {
        return &PhysicalFloat64Scalar_Type;
    }
    else if (storage_descr->type_num == NPY_FLOAT32) {
        return &PhysicalFloat32Scalar_Type;
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Unsupported storage dtype for PhysicalScalar");
        return NULL;
    }
}

bool
is_physical_scalar(PyObject *obj)
{
    if (PyObject_TypeCheck(obj, &PhysicalFloat64Scalar_Type) ||
        PyObject_TypeCheck(obj, &PhysicalFloat32Scalar_Type)) {
        return true;
    }
    return false;
}

PyArray_Descr *
get_physicalscalar_storage_descr(PyTypeObject *type)
{
    if (type == &PhysicalFloat64Scalar_Type) {
        return PyArray_DescrFromType(NPY_FLOAT64);
    }
    else if (type == &PhysicalFloat32Scalar_Type) {
        return PyArray_DescrFromType(NPY_FLOAT32);
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Unknown PhysicalScalar type");
        return NULL;
    }
}

/*
　This function creates a new PhysicalScalarObject with the given value and physical dimension.
  NOTE: This function create new references to both value and physical_dimension.
 */
PhysicalScalarObject *
PhysicalScalar_raw_new(PyTypeObject *type, PyObject *value,
                       PhysicalDimensionObject *physical_dimension)
{
    PyArray_Descr *expected_descr = NULL;
    PyArray_Descr *value_descr = NULL;
    PhysicalScalarObject *self = NULL;

    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "value argument is required");
        goto finalize;
    }

    if (PyArray_CheckScalar(value) == 0) {
        PyErr_SetString(PyExc_TypeError, "value must be a NumPy scalar");
        goto finalize;
    }
    expected_descr = get_physicalscalar_storage_descr(type);
    if (expected_descr == NULL) {
        goto finalize;
    }

    value_descr = PyArray_DescrFromScalar(value);
    if (PyDataType_ISNUMBER(value_descr) == 0) {
        PyErr_SetString(PyExc_TypeError, "value must be a numeric NumPy scalar");
        goto finalize;
    }

    if (PyArray_EquivTypes(expected_descr, value_descr) == 0) {
        PyErr_SetString(PyExc_TypeError, "value must be a NumPy scalar of the expected type");
        goto finalize;
    }

    self = PyObject_New(PhysicalScalarObject, type);
    if (self == NULL) {
        goto finalize;
    }

    self->value = Py_NewRef(value);
    self->physical_dimension = (PhysicalDimensionObject *)Py_NewRef(physical_dimension);
    goto finalize;

finalize:
    Py_XDECREF(expected_descr);
    Py_XDECREF(value_descr);
    return self;
}

static PyObject *
PhysicalScalar_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
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

    PyObject *res = (PyObject *)PhysicalScalar_raw_new(cls, value, dimension);
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

PyTypeObject PhysicalFloat64Scalar_Type = {
        PyVarObject_HEAD_INIT(NULL, 0).tp_name = "_physicaldtype_main.PhysicalFloat64Scalar",
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

PyTypeObject PhysicalFloat32Scalar_Type = {
        PyVarObject_HEAD_INIT(NULL, 0).tp_name = "_physicaldtype_main.PhysicalFloat32Scalar",
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

    if (PyType_Ready(&PhysicalFloat64Scalar_Type) < 0) {
        return -1;
    }
    if (PyType_Ready(&PhysicalFloat32Scalar_Type) < 0) {
        return -1;
    }

    return 0;
}
