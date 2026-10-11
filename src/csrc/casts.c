#include <Python.h>

#define PY_ARRAY_UNIQUE_SYMBOL physicaldtype_ARRAY_API
#define PY_UFUNC_UNIQUE_SYMBOL physicaldtype_UFUNC_API
#define NO_IMPORT_ARRAY
#define NPY_NO_DEPRECATED_API NPY_2_0_API_VERSION
#define NPY_TARGET_VERSION NPY_2_0_API_VERSION
#define NO_IMPORT_ARRAY
#define NO_IMPORT_UFUNC
#include "numpy/ndarrayobject.h"
#include "numpy/dtype_api.h"
#include "numpy/ndarraytypes.h"

#include "casts.h"
#include "dtype.h"
#include "physical_dimension.h"
#include "physical_dimension_ops.h"

static int
phy_to_float64_copy(PyArrayMethod_Context *context, char *const *data, npy_intp const *dimensions,
                    npy_intp const *strides, NpyAuxData *NPY_UNUSED(auxdata))
{
    PhysicalDTypeObject *src = (PhysicalDTypeObject *)context->descriptors[0];

    npy_intp itemsize = src->storage_descr->elsize;

    char *in = data[0];
    char *out = data[1];

    for (npy_intp i = 0; i < dimensions[0]; i++) {
        memcpy(out, in, (size_t)itemsize);
        in += strides[0];
        out += strides[1];
    }

    return 0;
}

static int
phy_to_float64_get_loop(PyArrayMethod_Context *context, int NPY_UNUSED(aligned),
                        int NPY_UNUSED(move_references), const npy_intp *NPY_UNUSED(strides),
                        PyArrayMethod_StridedLoop **out_loop,
                        NpyAuxData **NPY_UNUSED(out_transferdata), NPY_ARRAYMETHOD_FLAGS *flags)
{
    PhysicalDTypeObject *src_dtype = (PhysicalDTypeObject *)context->descriptors[0];
    if (src_dtype->storage_descr->type_num != NPY_DOUBLE) {
        // Unreachable, since we check in resolve_descriptors
        // TODO: Implement conversion from other storage types to float64
        // we would like to use the existing NumPy casting mechanism
        PyErr_SetString(PyExc_TypeError, "Unsupported storage dtype");
        return -1;
    }

    *out_loop = (PyArrayMethod_StridedLoop *)&phy_to_float64_copy;
    *flags = 0;
    return 0;
}

static int
phy_to_float64_resolve_descriptors(PyObject *NPY_UNUSED(self),
                                   PyArray_DTypeMeta *const *NPY_UNUSED(dtypes),
                                   PyArray_Descr *const *given_descrs, PyArray_Descr **loop_descrs,
                                   npy_intp *NPY_UNUSED(view_offset))
{
    PhysicalDTypeObject *src_dtype = (PhysicalDTypeObject *)given_descrs[0];
    if (src_dtype->storage_descr->type_num != NPY_DOUBLE) {
        // So far, we banned this casting
        // TODO: Implement conversion using the existing NumPy casting mechanism
        PyErr_SetString(PyExc_TypeError, "Unsupported storage dtype");
        return -1;
    }

    loop_descrs[0] = (PyArray_Descr *)Py_NewRef(src_dtype);
    loop_descrs[1] = (PyArray_Descr *)PyArray_DescrFromType(NPY_DOUBLE);
    if (loop_descrs[1] == NULL) {
        return -1;
    }

    return NPY_SAFE_CASTING;
}

// #region PhysicalDtype to PhysicalDtype

static int
phy_to_phy_copy(PyArrayMethod_Context *context, char *const *data, npy_intp const *dimensions,
                npy_intp const *strides, NpyAuxData *NPY_UNUSED(auxdata))
{
    PhysicalDTypeObject *src = (PhysicalDTypeObject *)context->descriptors[0];

    npy_intp itemsize = src->storage_descr->elsize;

    char *in = data[0];
    char *out = data[1];

    for (npy_intp i = 0; i < dimensions[0]; i++) {
        memcpy(out, in, (size_t)itemsize);
        in += strides[0];
        out += strides[1];
    }

    return 0;
}

static NPY_CASTING
phy_to_phy_resolve_descriptors(PyObject *NPY_UNUSED(self),
                               PyArray_DTypeMeta *const *NPY_UNUSED(dtypes),
                               PyArray_Descr *const *given_descrs, PyArray_Descr **loop_descrs,
                               npy_intp *NPY_UNUSED(view_offset))
{
    loop_descrs[0] = (PyArray_Descr *)Py_NewRef(given_descrs[0]);

    if (given_descrs[1] == NULL) {
        loop_descrs[1] = (PyArray_Descr *)Py_NewRef(given_descrs[0]);
        return NPY_SAFE_CASTING;
    }

    loop_descrs[1] = (PyArray_Descr *)Py_NewRef(given_descrs[1]);

    if (PyObject_RichCompareBool((PyObject *)((PhysicalDTypeObject *)loop_descrs[0])->storage_descr,
                                 (PyObject *)((PhysicalDTypeObject *)loop_descrs[1])->storage_descr,
                                 Py_EQ) < 1) {
        // Temporarily block this casting
        return -1;
    }

    if (physical_dimension_equal(((PhysicalDTypeObject *)loop_descrs[0])->physical_dimension,
                                 ((PhysicalDTypeObject *)loop_descrs[1])->physical_dimension) ==
        false) {
        return NPY_UNSAFE_CASTING;
    }

    return NPY_SAFE_CASTING;
}

static int
phy_to_phy_get_loop(PyArrayMethod_Context *context, int NPY_UNUSED(aligned),
                    int NPY_UNUSED(move_references), const npy_intp *NPY_UNUSED(strides),
                    PyArrayMethod_StridedLoop **out_loop, NpyAuxData **NPY_UNUSED(out_transferdata),
                    NPY_ARRAYMETHOD_FLAGS *flags)
{
    PhysicalDTypeObject *src_dtype = (PhysicalDTypeObject *)context->descriptors[0];
    PhysicalDTypeObject *dst_dtype = (PhysicalDTypeObject *)context->descriptors[1];

    npy_intp src_elsize = src_dtype->storage_descr->elsize;
    npy_intp dst_elsize = dst_dtype->storage_descr->elsize;
    if (src_elsize != dst_elsize) {
        // Unreachable, since we check in resolve_descriptors
        return -1;
    }

    *out_loop = (PyArrayMethod_StridedLoop *)&phy_to_phy_copy;
    *flags = 0;
    return 0;
}

/*
 * NumPy currently allows NULL for the own DType/"cls".
 */
static PyArray_DTypeMeta *phy2phy_dtypes[2] = {NULL, NULL};

static PyType_Slot phy2phy_slots[] = {
        {NPY_METH_resolve_descriptors, &phy_to_phy_resolve_descriptors},
        {NPY_METH_get_loop, &phy_to_phy_get_loop},
        {0, NULL}};

static PyArrayMethod_Spec PhyToPhyCastSpec = {
        .name = "cast_PhysicalDType_to_PhysicalDType",
        .nin = 1,
        .nout = 1,
        .flags = NPY_METH_SUPPORTS_UNALIGNED,
        .casting = NPY_UNSAFE_CASTING,
        .dtypes = phy2phy_dtypes,
        .slots = phy2phy_slots,
};

// #endregion

static PyType_Slot p2f_slots[] = {
        {NPY_METH_resolve_descriptors, (void *)&phy_to_float64_resolve_descriptors},
        {NPY_METH_get_loop, (void *)&phy_to_float64_get_loop},
        {0, NULL}};

static char *p2f_name = "cast_PhysicalDType_to_Float64";

PyArrayMethod_Spec **
init_casts(void)
{
    PyArray_DTypeMeta **p2f_dtypes = malloc(2 * sizeof(PyArray_DTypeMeta *));
    if (p2f_dtypes == NULL) {
        return NULL;
    }
    p2f_dtypes[0] = NULL;
    p2f_dtypes[1] = &PyArray_DoubleDType;

    PyArrayMethod_Spec *PhyToFloat64CastSpec = malloc(sizeof(PyArrayMethod_Spec));
    if (PhyToFloat64CastSpec == NULL) {
        free(p2f_dtypes);
        return NULL;
    }
    PhyToFloat64CastSpec->name = p2f_name;
    PhyToFloat64CastSpec->nin = 1;
    PhyToFloat64CastSpec->nout = 1;
    PhyToFloat64CastSpec->flags = NPY_METH_SUPPORTS_UNALIGNED;
    PhyToFloat64CastSpec->casting = NPY_SAFE_CASTING;
    PhyToFloat64CastSpec->dtypes = p2f_dtypes;
    PhyToFloat64CastSpec->slots = p2f_slots;

    PyArrayMethod_Spec **casts = malloc(3 * sizeof(PyArrayMethod_Spec *));
    if (casts == NULL) {
        free(p2f_dtypes);
        free(PhyToFloat64CastSpec);
        return NULL;
    }
    casts[0] = &PhyToPhyCastSpec;
    casts[1] = PhyToFloat64CastSpec;
    casts[2] = NULL;

    return casts;
}

void
free_casts(PyArrayMethod_Spec **casts)
{
    if (casts == NULL) {
        return;
    }

    // NOTE: i = 0 is the PhyToPhyCastSpec, which is a static variable and should not be freed.
    // TODO: Consider refactoring to make all casts dynamically allocated for consistency.
    for (int i = 1; casts[i] != NULL; i++) {
        PyArrayMethod_Spec *spec = casts[i];
        if (spec == NULL) {
            continue;
        }

        free(spec->dtypes);
        // free(spec->slots);
        free(spec);
    }

    free(casts);
    return;
}
