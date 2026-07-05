

#include "physical_backend_type.h"
#include <Python.h>
#include <stddef.h> /* for offsetof() */


static PyObject *PhysicalBackendObject_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PhysicalBackendObject *self;
    self = (PhysicalBackendObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        for (int i = 0; i < DIM_COUNT; i++) {
            self->exponents[i] = 0.0; // Initialize exponents to zero
        }
    }
    return (PyObject *)self;
};


static int parse_dimension_name_to_index(const char *dim_name) {
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
    return -1; // Invalid dimension name
};

static int PhysicalBackendObject_init(PhysicalBackendObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"exponents", NULL};
    PyObject *exponents_obj = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &exponents_obj)) {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments: expected a list or dictionary for 'exponents'");
        return -1;
    }

    if (exponents_obj && PyList_Check(exponents_obj)) {
        if (PyList_Size(exponents_obj) > DIM_COUNT) {
            PyErr_SetString(PyExc_ValueError, "Exponents list must not exceed DIM_COUNT=7");
            return -1;
        }

        for (Py_ssize_t i = 0; i < PyList_Size(exponents_obj); i++) {
            PyObject *item = PyList_GetItem(exponents_obj, i);
            if (!PyFloat_Check(item) && !PyLong_Check(item)) {
                PyErr_SetString(PyExc_TypeError, "Exponents list must contain only floats or integers");
                return -1;
            }
            self->exponents[i] = PyFloat_AsDouble(item);
            Py_DECREF(item);
        }
        return 0;
    }

    if (exponents_obj && PyDict_Check(exponents_obj)) {
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(exponents_obj, &pos, &key, &value)) {
            if (!PyUnicode_Check(key)) {
                PyErr_SetString(PyExc_TypeError, "Keys in exponents dictionary must be strings");
                return -1;
            }
            if (!PyFloat_Check(value) && !PyLong_Check(value)) {
                PyErr_SetString(PyExc_TypeError, "Values in exponents dictionary must be floats or integers");
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

    return -1;
};


static PyMemberDef PhysicalBackendObject_members[] = {
    // {"exponents", T_DOUBLE, offsetof(PhysicalBackendObject, exponents), 0, "Exponents for each dimension"},
    {NULL}  /* Sentinel */
};


static PyObject *PhysicalBackendObject_get_exponent(PhysicalBackendObject *self, PyObject *args) {
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

static PyMethodDef PhysicalBackendObject_methods[] = {
    {"get_exponent", (PyCFunction)PhysicalBackendObject_get_exponent, METH_VARARGS, "Get the exponent for a given dimension name"},
    {NULL}  /* Sentinel */
};

PyTypeObject PhysicalBackendObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "_physicaldtype_main.PhysicalBackend",
    .tp_basicsize = sizeof(PhysicalBackendObject),
    .tp_itemsize = 0,
    .tp_new = PhysicalBackendObject_new,
    .tp_init = PhysicalBackendObject_init,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_members = PhysicalBackendObject_members,
    .tp_methods = PhysicalBackendObject_methods,
};