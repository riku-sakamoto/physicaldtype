#include <Python.h>


#define PY_ARRAY_UNIQUE_SYMBOL PhysicalType_ARRAY_API
#define PY_UFUNC_UNIQUE_SYMBOL PhysicalType_UFUNC_API
#define NPY_NO_DEPRECATED_API NPY_2_0_API_VERSION
#define NPY_TARGET_VERSION NPY_2_4_API_VERSION

#include "numpy/arrayobject.h"
#include "numpy/dtype_api.h"



static PyObject* my_function(PyObject* self){
    return PyUnicode_FromString("Hello from C!");
};


static PyMethodDef module_methods[] = {
    {"my_function", (PyCFunction)my_function, METH_NOARGS, "Returns a greeting from C."},
    {NULL, NULL, 0, NULL} // Sentinel
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_physicaldtype_main",
    .m_size = -1,
    .m_methods = module_methods,
};


PyMODINIT_FUNC PyInit__physicaldtype_main(void) {
    import_array();
    // import_umath();

    PyObject *m = PyModule_Create(&moduledef);
    if (m == NULL) {
        return NULL;
    }

    return m;

    error:
        Py_DECREF(m);
        return NULL;
};
