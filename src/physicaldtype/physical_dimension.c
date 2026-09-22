

#include <Python.h>
#include <stddef.h> /* for offsetof() */

#include "physical_dimension_ops.h"
#include "physical_dimension.h"

static PyObject *
PhysicalDimensionObject_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PhysicalDimensionObject *self;
    self = (PhysicalDimensionObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        for (int i = 0; i < DIM_COUNT; i++) {
            self->exponents[i] = 0.0;  // Initialize exponents to zero
        }
    }
    return (PyObject *)self;
};

static int
parse_dimension_name_to_index(const char *dim_name)
{
    if (strcmp(dim_name, "L") == 0) {
        return DIM_LENGTH;
    }
    if (strcmp(dim_name, "M") == 0) {
        return DIM_MASS;
    }
    if (strcmp(dim_name, "T") == 0) {
        return DIM_TIME;
    }
    if (strcmp(dim_name, "I") == 0) {
        return DIM_CURRENT;
    }
    if (strcmp(dim_name, "Theta") == 0) {
        return DIM_TEMPERATURE;
    }
    if (strcmp(dim_name, "N") == 0) {
        return DIM_AMOUNT;
    }
    if (strcmp(dim_name, "J") == 0) {
        return DIM_LUMINOUS_INTENSITY;
    }
    return -1;  // Invalid dimension name
};

static int
PhysicalDimension_raw_init(PhysicalDimensionObject *self, PyObject *dimensions)
{
    if (dimensions == NULL) {
        // Return with all exponents initialized to zero
        return 0;
    }

    if (PyList_Check(dimensions)) {
        if (PyList_Size(dimensions) > DIM_COUNT) {
            PyErr_SetString(PyExc_ValueError, "Exponents list must not exceed DIM_COUNT=7");
            return -1;
        }

        for (Py_ssize_t i = 0; i < PyList_Size(dimensions); i++) {
            PyObject *item = PyList_GetItem(dimensions, i);
            if (!PyFloat_Check(item) && !PyLong_Check(item)) {
                PyErr_SetString(PyExc_TypeError,
                                "Exponents list must contain only floats or integers");
                return -1;
            }
            self->exponents[i] = PyFloat_AsDouble(item);
        }
        return 0;
    }

    if (PyDict_Check(dimensions)) {
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(dimensions, &pos, &key, &value)) {
            if (!PyUnicode_Check(key)) {
                PyErr_SetString(PyExc_TypeError, "Keys in exponents dictionary must be strings");
                return -1;
            }
            if (!PyFloat_Check(value) && !PyLong_Check(value)) {
                PyErr_SetString(PyExc_TypeError,
                                "Values in exponents dictionary must be floats or integers");
                return -1;
            }
            const char *dim_name = PyUnicode_AsUTF8(key);
            double exponent_value = PyFloat_AsDouble(value);

            int index = parse_dimension_name_to_index(dim_name);
            if (index == -1) {
                PyErr_Format(PyExc_ValueError, "Invalid dimension name: %s", dim_name);
                return -1;
            }
            self->exponents[index] = exponent_value;
        }
        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "Invalid argument type: expected a list or dictionary");
    return -1;
}

PhysicalDimensionObject *
PhysicalDimension_raw_new(PyObject *dimensions)
{
    PhysicalDimensionObject *self =
            PhysicalDimensionObject_new(&PhysicalDimensionObjectType, NULL, NULL);
    if (self == NULL) {
        return NULL;
    }

    if (PhysicalDimension_raw_init(self, dimensions) == -1) {
        Py_DECREF(self);
        return NULL;
    }

    return self;
}

static int
PhysicalDimensionObject_init(PhysicalDimensionObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"exponents", NULL};
    PyObject *exponents_obj = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &exponents_obj)) {
        PyErr_SetString(PyExc_TypeError,
                        "Invalid arguments: expected a list or dictionary for 'exponents'");
        return -1;
    }

    if (PhysicalDimension_raw_init(self, exponents_obj) == -1) {
        return -1;
    }

    return 0;
};

static char *
convert_double_to_string(double value)
{
    static char buffer[32];                           // Adjust size as needed
    snprintf(buffer, sizeof(buffer), "%.6f", value);  // Format with 6 decimal places
    return buffer;
}

static PyObject *
PhysicalDimensionObject_repr(PhysicalDimensionObject *self)
{
    PyObject *list = PyList_New(DIM_COUNT);
    if (list == NULL) {
        return NULL;
    }

    for (int i = 0; i < DIM_COUNT; i++) {
        PyObject *value = PyFloat_FromDouble(self->exponents[i]);
        if (value == NULL) {
            Py_DECREF(list);
            return NULL;
        }

        PyList_SET_ITEM(list, i, value);  // steals reference
    }

    PyObject *result = PyUnicode_FromFormat("PhysicalDimension(%R)", list);

    Py_DECREF(list);
    return result;
};

static PyMemberDef PhysicalDimensionObject_members[] = {
        // {"exponents", T_DOUBLE, offsetof(PhysicalDimensionObject, exponents), 0, "Exponents for
        // each dimension"},
        {NULL} /* Sentinel */
};

static PyObject *
PhysicalDimensionObject_get_exponent(PhysicalDimensionObject *self, PyObject *args)
{
    const char *dim_name;
    if (!PyArg_ParseTuple(args, "s", &dim_name)) {
        return NULL;
    }

    int index = parse_dimension_name_to_index(dim_name);
    if (index == -1) {
        PyErr_Format(PyExc_ValueError, "Invalid dimension name: %s", dim_name);
        return NULL;
    }

    return PyFloat_FromDouble(self->exponents[index]);
};

static PyMethodDef PhysicalDimensionObject_methods[] = {
        {"get_exponent", (PyCFunction)PhysicalDimensionObject_get_exponent, METH_VARARGS,
         "Get the exponent for a given dimension name"},
        {NULL} /* Sentinel */
};

PyTypeObject PhysicalDimensionObjectType = {
        PyVarObject_HEAD_INIT(NULL, 0).tp_name = "_physicaldtype_main.PhysicalDimension",
        .tp_basicsize = sizeof(PhysicalDimensionObject),
        .tp_itemsize = 0,
        .tp_new = PhysicalDimensionObject_new,
        .tp_init = PhysicalDimensionObject_init,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_repr = (reprfunc)PhysicalDimensionObject_repr,
        .tp_str = (reprfunc)PhysicalDimensionObject_repr,
        .tp_as_number = &PhysicalDimensionObject_as_number,
        .tp_members = PhysicalDimensionObject_members,
        .tp_methods = PhysicalDimensionObject_methods,
};
