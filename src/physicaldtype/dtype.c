#include <Python.h>
#include "structmember.h"

#define PY_ARRAY_UNIQUE_SYMBOL physicaldtype_ARRAY_API
#define PY_UFUNC_UNIQUE_SYMBOL physicaldtype_UFUNC_API
#define NO_IMPORT_ARRAY
#include "dtype.h"
#include "physical_dimension.h"
#include "casts.h"

PyTypeObject *PhysicalScalar_Type = NULL;

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
    return self;
};

static PhysicalDTypeObject *
physicaldtype_ensure_canonical(PhysicalDTypeObject *self)
{
    // Ensure that the PhysicalDTypeObject is in its canonical form
    Py_INCREF(self);
    return self;
};

// https://numpy.org/doc/stable/reference/c-api/array.html#slot-ids-and-api-function-typedefs
static PyType_Slot PhysicalDType_Slots[] = {
        {NPY_DT_ensure_canonical, &physicaldtype_ensure_canonical}, {0, NULL}};

PyObject *
physicaldtype_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"physical_dimension", NULL};

    PyObject *dimension_dict = NULL;

    // Need to accept other types like string?
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O!", kwlist, &PyDict_Type, &dimension_dict)) {
        return NULL;
    }

    PhysicalDimensionObject *dimension = (PhysicalDimensionObject *)PyObject_CallFunctionObjArgs(
            (PyObject *)&PhysicalDimensionObjectType, dimension_dict, NULL);
    if (dimension == NULL) {
        return NULL;
    }

    PyObject *res = (PyObject *)new_physicaldtype_instance(dimension);
    Py_DECREF(dimension);
    return res;
};

static void
physicaltype_dealloc(PhysicalDTypeObject *self)
{
    Py_CLEAR(self->physical_dimension);
    PyArrayDescr_Type.tp_dealloc((PyObject *)self);
};

static PyObject *
physicaldtype_repr(PhysicalDTypeObject *self)
{
    return PyUnicode_FromFormat("PhysicalDType('%R')", self->physical_dimension);
};

static PyMemberDef PhysicalDType_members[] = {
        {"physical_dimension", T_OBJECT_EX, offsetof(PhysicalDTypeObject, physical_dimension),
         READONLY, "Physical dimension object"},
        {NULL} /* Sentinel */
};

// https://numpy.org/doc/stable/reference/c-api/types-and-structures.html#c.PyArray_DTypeMeta
PyArray_DTypeMeta PhysicalDType = {
        {{
                PyVarObject_HEAD_INIT(NULL, 0).tp_name = "physicaldtype.PhysicalDType",
                .tp_basicsize = sizeof(PhysicalDTypeObject),
                .tp_new = physicaldtype_new,
                .tp_dealloc = (destructor)physicaltype_dealloc,
                .tp_repr = (reprfunc)physicaldtype_repr,
                .tp_str = (reprfunc)physicaldtype_repr,
                .tp_members = PhysicalDType_members,
        }},
};

int
init_physical_dtype(void)
{
    // Initialize the physical dtype

    PyArrayMethod_Spec **casts = init_casts();
    if (!casts) 
        return -1;

    // https://numpy.org/doc/stable/reference/c-api/types-and-structures.html#c.PyArrayDTypeMeta_Spec
    PyArrayDTypeMeta_Spec PhysicalDType_DTypeSpec = {
            .typeobj = &PhysicalScalar_Type,
            .flags = NPY_DT_PARAMETRIC | NPY_DT_NUMERIC,
            .casts = casts,
            .slots = PhysicalDType_Slots,
    };

    return 0;
}
