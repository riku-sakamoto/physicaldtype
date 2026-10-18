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
new_physicaldtype_instance(PhysicalDimensionObject *physical_dimension,
                           PyArray_Descr *storage_descr)
{
    PyTypeObject *type = get_physicaldtype_type(storage_descr);
    if (type == NULL) {
        return NULL;
    }
    PhysicalDTypeObject *self = (PhysicalDTypeObject *)PyArrayDescr_Type.tp_new(type, NULL, NULL);

    if (self == NULL) {
        return NULL;
    }

    if (PyDataType_ISNUMBER(storage_descr) == 0) {
        PyErr_SetString(PyExc_TypeError, "Storage dtype must be a numeric dtype");
        Py_DECREF(self);
        return NULL;
    }

    Py_INCREF(physical_dimension);
    self->physical_dimension = physical_dimension;

    Py_INCREF(storage_descr);
    self->storage_descr = storage_descr;
    self->base.elsize = storage_descr->elsize;
    self->base.alignment = storage_descr->alignment;
    return self;
}

PyTypeObject *
get_physicaldtype_type(PyArray_Descr *storage_descr)
{
    if (storage_descr->type_num == NPY_FLOAT64) {
        return (PyTypeObject *)&PhysicalFloat64DType;
    }
    else if (storage_descr->type_num == NPY_FLOAT32) {
        return (PyTypeObject *)&PhysicalFloat32DType;
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Unsupported storage dtype");
        return NULL;
    }
}

PyArray_Descr *
get_physicaldtype_storage_descr(PyArray_DTypeMeta *self)
{
    if (self == (PyArray_DTypeMeta *)&PhysicalFloat64DType) {
        return PyArray_DescrFromType(NPY_FLOAT64);
    }
    else if (self == (PyArray_DTypeMeta *)&PhysicalFloat32DType) {
        return PyArray_DescrFromType(NPY_FLOAT32);
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Unknown PhysicalDType type");
        return NULL;
    }
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

static PyArray_DTypeMeta *
common_dtype(PyArray_DTypeMeta *self, PyArray_DTypeMeta *other)
{
    if (self == other) {
        Py_INCREF(self);
        return (PyArray_DTypeMeta *)self;
    }

    if (self == (PyArray_DTypeMeta *)&PhysicalFloat64DType &&
        other == (PyArray_DTypeMeta *)&PhysicalFloat32DType) {
        Py_INCREF(self);
        return (PyArray_DTypeMeta *)self;
    }

    Py_INCREF(Py_NotImplemented);
    return (PyArray_DTypeMeta *)Py_NotImplemented;
}

static PyArray_Descr *
physicaldtype_discover_descriptor_from_pyobject(PyArray_DTypeMeta *cls, PyObject *obj)
{
    if (is_physical_scalar(obj)) {
        PhysicalScalarObject *phys_scalar = (PhysicalScalarObject *)obj;
        PhysicalDimensionObject *dim = phys_scalar->physical_dimension;
        PyArray_Descr *storage_descr = get_physicaldtype_storage_descr(cls);
        if (storage_descr == NULL) {
            return NULL;
        }

        PhysicalDTypeObject *new_dtype = new_physicaldtype_instance(dim, storage_descr);
        Py_DECREF(storage_descr);
        if (new_dtype == NULL) {
            return NULL;
        }
        return (PyArray_Descr *)new_dtype;
    }

    Py_INCREF(Py_NotImplemented);
    return (PyArray_Descr *)Py_NotImplemented;
}

static int
check_scalar_type(PyObject *obj, PhysicalDTypeObject *dtype)
{
    if (is_physical_scalar(obj)) {
        PhysicalScalarObject *phys_scalar = (PhysicalScalarObject *)obj;
        if (physical_dimension_equal(phys_scalar->physical_dimension, dtype->physical_dimension) ==
            false) {
            PyErr_SetString(
                    PyExc_TypeError,
                    "PhysicalScalar must have the same physical dimension as the PhysicalDType");
            return -1;
        }
        return 0;
    }

    if (PyArray_IsPythonScalar(obj)) {
        if (PyNumber_Check(obj) != 1) {
            PyErr_SetString(PyExc_TypeError, "Object is not a numeric scalar");
            return -1;
        }
        return 0;
    }

    if (PyArray_CheckScalar(obj)) {
        PyArray_Descr *descr = PyArray_DescrFromScalar(obj);
        if (descr == NULL) {
            return -1;
        }
        if (PyDataType_ISNUMBER(descr) != 1) {
            PyErr_SetString(PyExc_TypeError, "Object is not a numeric scalar");

            Py_DECREF(descr);
            return -1;
        }
        Py_DECREF(descr);
        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "Object is not a recognized scalar type");
    return -1;
}

static int
physicaldtype_setitem(PhysicalDTypeObject *descr, PyObject *obj, char *dataptr)
{
    if (obj == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot set item to NULL");
        return -1;
    }

    if (check_scalar_type(obj, descr) != 0) {
        return -1;
    }

    PyObject *value = NULL;
    if (is_physical_scalar(obj)) {
        if (physical_dimension_equal(descr->physical_dimension,
                                     ((PhysicalScalarObject *)obj)->physical_dimension) == false) {
            PyErr_SetString(
                    PyExc_TypeError,
                    "PhysicalDType can only be constructed from a PhysicalScalar with the same "
                    "physical dimension");
            return -1;
        }
        value = ((PhysicalScalarObject *)obj)->value;
    }
    else {
        value = obj;
    }

    if (value == NULL) {
        // This should not happen, but just in case
        PyErr_SetString(PyExc_TypeError, "Cannot set item to NULL");
        return -1;
    }

    PyObject *tmp = PyArray_FromAny(value, (PyArray_Descr *)Py_NewRef(descr->storage_descr), 0, 0,
                                    NPY_ARRAY_ENSUREARRAY | NPY_ARRAY_FORCECAST, NULL);
    if (tmp == NULL) {
        return -1;
    }
    if (PyArray_IsZeroDim(tmp) == 0) {
        PyErr_SetString(PyExc_TypeError, "Failed to convert value to a zero-dimensional array");
        Py_DECREF(tmp);
        return -1;
    }

    memcpy(dataptr, PyArray_DATA((PyArrayObject *)tmp), descr->storage_descr->elsize);
    Py_DECREF(tmp);

    return 0;
}

static PyObject *
physicaldtype_getitem(PhysicalDTypeObject *descr, char *dataptr)
{
    PyArray_Descr *base_descr = descr->storage_descr;
    PyObject *value_obj = PyArray_Scalar(dataptr, base_descr, NULL);

    if (value_obj == NULL) {
        return NULL;
    }

    PyTypeObject *scalar_type = get_physicalscalar_type(base_descr);
    if (scalar_type == NULL) {
        Py_DECREF(value_obj);
        return NULL;
    }

    PhysicalScalarObject *phys_scalar =
            PhysicalScalar_raw_new(scalar_type, value_obj, descr->physical_dimension);
    if (phys_scalar == NULL) {
        return NULL;
    }

    PyObject *out = (PyObject *)phys_scalar;
    Py_DECREF(value_obj);
    return out;
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
physicaldtype_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"physical_dimension", NULL};

    PyObject *dimension_like_obj = NULL;

    PhysicalDimensionObject *dimension = NULL;
    PyArray_Descr *storage_descr = NULL;
    PyObject *res = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &dimension_like_obj)) {
        goto finalize;
    }

    dimension = PhysicalDimension_raw_new(dimension_like_obj);
    if (dimension == NULL) {
        goto finalize;
    }

    storage_descr = get_physicaldtype_storage_descr((PyArray_DTypeMeta *)type);
    if (storage_descr == NULL) {
        goto finalize;
    }

    res = (PyObject *)new_physicaldtype_instance(dimension, storage_descr);
    goto finalize;

finalize:
    Py_XDECREF(dimension);
    Py_XDECREF(storage_descr);
    return res;
}

static void
physicaltype_dealloc(PhysicalDTypeObject *self)
{
    Py_CLEAR(self->physical_dimension);
    Py_CLEAR(self->storage_descr);
    PyArrayDescr_Type.tp_dealloc((PyObject *)self);
}

static PyObject *
physicaldtype_repr(PhysicalDTypeObject *self)
{
    return PyUnicode_FromFormat("PhysicalDType[%S](%R)", self->storage_descr,
                                self->physical_dimension);
}

static PyMemberDef PhysicalDType_members[] = {
        {"physical_dimension", T_OBJECT_EX, offsetof(PhysicalDTypeObject, physical_dimension),
         READONLY, "Physical dimension object"},
        {"storage_dtype", T_OBJECT_EX, offsetof(PhysicalDTypeObject, storage_descr), READONLY,
         "Storage dtype descriptor"},
        {NULL} /* Sentinel */
};

// https://numpy.org/doc/stable/reference/c-api/types-and-structures.html#c.PyArray_DTypeMeta

PyArray_DTypeMeta PhysicalFloat64DType = {{{
        PyVarObject_HEAD_INIT(NULL, 0).tp_name = "physicaldtype.PhysicalFloat64DType",
        .tp_basicsize = sizeof(PhysicalDTypeObject),
        .tp_new = physicaldtype_new,
        .tp_dealloc = (destructor)physicaltype_dealloc,
        .tp_repr = (reprfunc)physicaldtype_repr,
        .tp_str = (reprfunc)physicaldtype_repr,
        .tp_members = PhysicalDType_members,
}}};

PyArray_DTypeMeta PhysicalFloat32DType = {{{
        PyVarObject_HEAD_INIT(NULL, 0).tp_name = "physicaldtype.PhysicalFloat32DType",
        .tp_basicsize = sizeof(PhysicalDTypeObject),
        .tp_new = physicaldtype_new,
        .tp_dealloc = (destructor)physicaltype_dealloc,
        .tp_repr = (reprfunc)physicaldtype_repr,
        .tp_str = (reprfunc)physicaldtype_repr,
        .tp_members = PhysicalDType_members,
}}};

int
init_physical_dtype_meta(PyArray_DTypeMeta *dtype_meta, PyTypeObject *scalar_type)
{
    // Initialize the physical dtype
    PyArrayMethod_Spec **casts = init_casts();
    if (!casts) {
        return -1;
    }

    // https://numpy.org/doc/stable/reference/c-api/types-and-structures.html#c.PyArrayDTypeMeta_Spec
    PyArrayDTypeMeta_Spec spec = {
            .typeobj = scalar_type,
            .flags = NPY_DT_PARAMETRIC | NPY_DT_NUMERIC,
            .casts = casts,
            .slots = PhysicalDType_Slots,
    };

    ((PyObject *)dtype_meta)->ob_type = &PyArrayDTypeMeta_Type;
    ((PyTypeObject *)dtype_meta)->tp_base = &PyArrayDescr_Type;

    if (PyType_Ready((PyTypeObject *)dtype_meta) < 0) {
        free_casts(casts);
        return -1;
    }

    if (PyArrayInitDTypeMeta_FromSpec(dtype_meta, &spec) < 0) {
        free_casts(casts);
        return -1;
    }

    free_casts(casts);
    return 0;
}

int
init_physical_dtype(void)
{
    if (init_physical_dtype_meta(&PhysicalFloat64DType, &PhysicalFloat64Scalar_Type) < 0) {
        return -1;
    }

    if (init_physical_dtype_meta(&PhysicalFloat32DType, &PhysicalFloat32Scalar_Type) < 0) {
        return -1;
    }

    return 0;
}
