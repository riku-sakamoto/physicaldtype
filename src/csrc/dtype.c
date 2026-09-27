#include <Python.h>
#include "structmember.h"

#define PY_ARRAY_UNIQUE_SYMBOL physicaldtype_ARRAY_API
#define PY_UFUNC_UNIQUE_SYMBOL physicaldtype_UFUNC_API
#define NPY_NO_DEPRECATED_API NPY_2_0_API_VERSION
#define NPY_TARGET_VERSION NPY_2_4_API_VERSION
#define NO_IMPORT_ARRAY
#define NO_IMPORT_UFUNC

#include "numpy/ndarrayobject.h"
#include "numpy/ndarraytypes.h"
#include "numpy/dtype_api.h"

#include "dtype.h"
#include "physical_dimension.h"
#include "physical_dimension_ops.h"
#include "casts.h"
#include "scalar.h"

PhysicalDTypeObject *
new_physicaldtype_instance(PhysicalDimensionObject *physical_dimension)
{
    PhysicalDTypeObject *self = (PhysicalDTypeObject *)PyArrayDescr_Type.tp_new(
            (PyTypeObject *)&PhysicalDType, NULL, NULL);

    if (self == NULL) {
        return NULL;
    }

    Py_INCREF(physical_dimension);
    self->physical_dimension = physical_dimension;
    self->base.elsize = sizeof(double);  // Assuming we are using double for storage
    self->base.alignment = _Alignof(double);
    return self;
}

static double
get_value(PyObject *scalar)
{
    PyTypeObject *type = Py_TYPE(scalar);
    if (type != &PhysicalScalar_Type) {
        // HACK: Need to support other types like int, float, etc
        double res = PyFloat_AsDouble(scalar);
        if (res == -1.0 && PyErr_Occurred()) {
            PyErr_SetString(
                    PyExc_TypeError,
                    "PhysicalDType can only be constructed from a float or a PhysicalScalar");
            return -1;
        }
        return res;
    }

    PhysicalScalarObject *phys_scalar = (PhysicalScalarObject *)scalar;
    PyObject *value = phys_scalar->value;
    double res = PyFloat_AsDouble(value);
    if (res == -1.0 && PyErr_Occurred()) {
        PyErr_SetString(PyExc_TypeError, "PhysicalScalar value must be a float");
        return -1;
    }
    return res;
}

static PhysicalDTypeObject *
common_instance(PhysicalDTypeObject *self, PhysicalDTypeObject *other)
{
    if (physical_dimension_equal(self->physical_dimension, other->physical_dimension) == false) {
        PyErr_SetString(PyExc_TypeError,
                        "PhysicalDType instances must have the same physical dimension");
        return NULL;
    }

    Py_INCREF(self);
    return self;
}

static PyArray_Descr *
common_dtype(PyArray_DTypeMeta *self, PyArray_DTypeMeta *other)
{
    // So far, do not promote any dtype except for the same physical dimension

    if (other == (PyArray_DTypeMeta *)&PhysicalDType) {
        Py_INCREF(self);
        return (PyArray_Descr *)self;
    }

    Py_INCREF(Py_NotImplemented);
    return (PyArray_Descr *)Py_NotImplemented;
}

static PyArray_Descr *
physicaldtype_discover_descriptor_from_pyobject(PyArray_DTypeMeta *NPY_UNUSED(cls), PyObject *obj)
{
    if (Py_TYPE(obj) == &PhysicalScalar_Type) {
        PhysicalScalarObject *phys_scalar = (PhysicalScalarObject *)obj;
        PhysicalDimensionObject *dim = phys_scalar->physical_dimension;

        PhysicalDTypeObject *new_dtype = new_physicaldtype_instance(dim);
        if (new_dtype == NULL) {
            return NULL;
        }
        return (PyArray_Descr *)new_dtype;
    }

    Py_INCREF(Py_NotImplemented);
    return (PyArray_Descr *)Py_NotImplemented;
}

static int
physicaldtype_setitem(PhysicalDTypeObject *descr, PyObject *obj, char *dataptr)
{
    if (obj == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot set item to NULL");
        return -1;
    }

    double value = get_value(obj);
    if (value == -1.0 && PyErr_Occurred()) {
        return -1;
    }

    PyTypeObject *type = Py_TYPE(obj);
    if (type == &PhysicalScalar_Type) {
        if (physical_dimension_equal(descr->physical_dimension,
                                     ((PhysicalScalarObject *)obj)->physical_dimension) == false) {
            PyErr_SetString(
                    PyExc_TypeError,
                    "PhysicalDType can only be constructed from a PhysicalScalar with the same "
                    "physical dimension");
            return -1;
        }
    }

    memcpy(dataptr, &value, sizeof(double));
    return 0;
}

static int
physicaldtype_getitem(PhysicalDTypeObject *descr, char *dataptr, PyObject **out)
{
    double value;
    memcpy(&value, dataptr, sizeof(double));

    PyObject *value_obj = PyFloat_FromDouble(value);
    if (value_obj == NULL) {
        return -1;
    }
    PhysicalScalarObject *phys_scalar =
            PhysicalScalar_raw_new(value_obj, descr->physical_dimension);
    if (phys_scalar == NULL) {
        return -1;
    }

    *out = (PyObject *)phys_scalar;
    Py_DECREF(value_obj);
    return 0;
}

static PhysicalDTypeObject *
physicaldtype_ensure_canonical(PhysicalDTypeObject *self)
{
    // Ensure that the PhysicalDTypeObject is in its canonical form
    Py_INCREF(self);
    return self;
}

// https://numpy.org/doc/stable/reference/c-api/array.html#slot-ids-and-api-function-typedefs
static PyType_Slot PhysicalDType_Slots[] = {
        {NPY_DT_common_instance, &common_instance},
        {NPY_DT_common_dtype, &common_dtype},
        {NPY_DT_discover_descr_from_pyobject, &physicaldtype_discover_descriptor_from_pyobject},
        {NPY_DT_setitem, &physicaldtype_setitem},
        {NPY_DT_getitem, &physicaldtype_getitem},
        {NPY_DT_ensure_canonical, &physicaldtype_ensure_canonical},
        {0, NULL}};

PyObject *
physicaldtype_new(PyTypeObject *Py_UNUSED(type), PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"physical_dimension", NULL};

    PyObject *dimension_dict = NULL;

    // Need to accept other types like string?
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O!", kwlist, &PyDict_Type, &dimension_dict)) {
        return NULL;
    }

    PhysicalDimensionObject *dimension = PhysicalDimension_raw_new(dimension_dict);
    if (dimension == NULL) {
        return NULL;
    }

    PyObject *res = (PyObject *)new_physicaldtype_instance(dimension);
    Py_DECREF(dimension);
    return res;
}

static void
physicaltype_dealloc(PhysicalDTypeObject *self)
{
    Py_CLEAR(self->physical_dimension);
    PyArrayDescr_Type.tp_dealloc((PyObject *)self);
}

static PyObject *
physicaldtype_repr(PhysicalDTypeObject *self)
{
    return PyUnicode_FromFormat("PhysicalDType('%R')", self->physical_dimension);
}

static PyMemberDef PhysicalDType_members[] = {
        {"physical_dimension", T_OBJECT_EX, offsetof(PhysicalDTypeObject, physical_dimension),
         READONLY, "Physical dimension object"},
        {NULL} /* Sentinel */
};

// https://numpy.org/doc/stable/reference/c-api/types-and-structures.html#c.PyArray_DTypeMeta
PyArray_DTypeMeta PhysicalDType = {{{
        PyVarObject_HEAD_INIT(NULL, 0).tp_name = "physicaldtype.PhysicalDType",
        .tp_basicsize = sizeof(PhysicalDTypeObject),
        .tp_new = physicaldtype_new,
        .tp_dealloc = (destructor)physicaltype_dealloc,
        .tp_repr = (reprfunc)physicaldtype_repr,
        .tp_str = (reprfunc)physicaldtype_repr,
        .tp_members = PhysicalDType_members,
}}};

int
init_physical_dtype(void)
{
    // Initialize the physical dtype

    PyArrayMethod_Spec **casts = init_casts();
    if (!casts) {
        return -1;
    }

    //
    // https://numpy.org/doc/stable/reference/c-api/types-and-structures.html#c.PyArrayDTypeMeta_Spec
    PyArrayDTypeMeta_Spec PhysicalDType_DTypeSpec = {
            .typeobj = &PhysicalScalar_Type,
            .flags = NPY_DT_PARAMETRIC | NPY_DT_NUMERIC,
            .casts = casts,
            .slots = PhysicalDType_Slots,
    };

    ((PyObject *)&PhysicalDType)->ob_type = &PyArrayDTypeMeta_Type;
    ((PyTypeObject *)&PhysicalDType)->tp_base = &PyArrayDescr_Type;

    if (PyType_Ready((PyTypeObject *)&PhysicalDType) < 0) {
        free_casts(casts);
        return -1;
    }

    if (PyArrayInitDTypeMeta_FromSpec(&PhysicalDType, &PhysicalDType_DTypeSpec) < 0) {
        free_casts(casts);
        return -1;
    }

    free_casts(casts);
    return 0;
}
