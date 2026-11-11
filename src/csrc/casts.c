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

/* This function converts array from PhysicalDType to numeric dtype.  */
/* HACK: This is a temporary implementation.  In the future, we should use the existing NumPy
 * casting mechanism to handle this conversion. */
static int
phy_to_numpy_copy(PyArrayMethod_Context *context, char *const *data, npy_intp const *dimensions,
                  npy_intp const *strides, NpyAuxData *NPY_UNUSED(auxdata))
{
    if (dimensions[0] == 0) {
        return 0;
    }

    PhysicalDTypeObject *src = (PhysicalDTypeObject *)context->descriptors[0];
    PyArray_Descr *src_descr = src->storage_descr;
    PyArray_Descr *dst = context->descriptors[1];

    Py_INCREF(src_descr);
    PyArrayObject *tmp_src = (PyArrayObject *)PyArray_NewFromDescr(
            &PyArray_Type, src_descr, 1, (npy_intp *)dimensions, &strides[0], data[0], 0, NULL);
    if (tmp_src == NULL) {
        return -1;
    }

    Py_INCREF(dst);
    PyArrayObject *tmp_dst =
            (PyArrayObject *)PyArray_NewFromDescr(&PyArray_Type, dst, 1, (npy_intp *)dimensions,
                                                  &strides[1], data[1], NPY_ARRAY_WRITEABLE, NULL);
    if (tmp_dst == NULL) {
        Py_DECREF(tmp_src);
        return -1;
    }

    int ret = PyArray_CopyInto(tmp_dst, tmp_src);
    Py_DECREF(tmp_src);
    Py_DECREF(tmp_dst);
    return ret;
}

static int
phy_to_numeric_get_loop(PyArrayMethod_Context *context, int NPY_UNUSED(aligned),
                        int NPY_UNUSED(move_references), const npy_intp *NPY_UNUSED(strides),
                        PyArrayMethod_StridedLoop **out_loop,
                        NpyAuxData **NPY_UNUSED(out_transferdata), NPY_ARRAYMETHOD_FLAGS *flags)
{
    PhysicalDTypeObject *src_dtype = (PhysicalDTypeObject *)context->descriptors[0];
    PyArray_Descr *dst_dtype = context->descriptors[1];

    if (PyArray_CanCastTypeTo(src_dtype->storage_descr, dst_dtype, NPY_SAFE_CASTING) == 0) {
        // Unreachable, since we check in resolve_descriptors
        PyErr_SetString(PyExc_TypeError, "Cannot cast storage dtype to destination dtype");
        return -1;
    }

    *out_loop = (PyArrayMethod_StridedLoop *)&phy_to_numpy_copy;
    *flags = NPY_METH_REQUIRES_PYAPI;
    return 0;
}

static int
phy_to_numeric_resolve_descriptors(PyObject *NPY_UNUSED(self),
                                   PyArray_DTypeMeta *const *NPY_UNUSED(dtypes),
                                   PyArray_Descr *const *given_descrs, PyArray_Descr **loop_descrs,
                                   npy_intp *NPY_UNUSED(view_offset))
{
    PhysicalDTypeObject *src_dtype = (PhysicalDTypeObject *)given_descrs[0];
    PyArray_Descr *dst_dtype = given_descrs[1];
    if (dst_dtype == NULL) {
        PyErr_SetString(PyExc_TypeError, "Destination dtype cannot be NULL");
        return -1;
    }

    if (PyArray_CanCastTypeTo(src_dtype->storage_descr, dst_dtype, NPY_SAFE_CASTING) == 0) {
        PyErr_SetString(PyExc_TypeError, "Cannot cast storage dtype to destination dtype");
        return -1;
    }

    loop_descrs[0] = (PyArray_Descr *)Py_NewRef(src_dtype);
    loop_descrs[1] = (PyArray_Descr *)Py_NewRef(dst_dtype);

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

static int
phy_f32_to_phy_f64_copy(PyArrayMethod_Context *NPY_UNUSED(context), char *const *data,
                        npy_intp const *dimensions, npy_intp const *strides,
                        NpyAuxData *NPY_UNUSED(auxdata))
{
    char *in = data[0];
    char *out = data[1];

    for (npy_intp i = 0; i < dimensions[0]; i++) {
        float src_value;
        double dst_value;

        memcpy(&src_value, in, sizeof(float));
        dst_value = (double)src_value;
        memcpy(out, &dst_value, sizeof(double));

        in += strides[0];
        out += strides[1];
    }

    return 0;
}

const NPY_CASTING rules[] = {
        NPY_NO_CASTING,        NPY_EQUIV_CASTING,  NPY_SAFE_CASTING,
        NPY_SAME_KIND_CASTING, NPY_UNSAFE_CASTING,
};

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

    if (physical_dimension_equal(((PhysicalDTypeObject *)loop_descrs[0])->physical_dimension,
                                 ((PhysicalDTypeObject *)loop_descrs[1])->physical_dimension) ==
        false) {
        return NPY_UNSAFE_CASTING;
    }

    for (int i = 0; i < 5; i++) {
        if (PyArray_CanCastTypeTo(((PhysicalDTypeObject *)loop_descrs[0])->storage_descr,
                                  ((PhysicalDTypeObject *)loop_descrs[1])->storage_descr,
                                  rules[i]) != 0) {
            return rules[i];
        }
    }

    return -1;
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
    if (src_elsize == dst_elsize) {
        *out_loop = (PyArrayMethod_StridedLoop *)&phy_to_phy_copy;
        *flags = 0;
        return 0;
    }

    if (src_dtype->storage_descr->type_num == NPY_FLOAT32 &&
        dst_dtype->storage_descr->type_num == NPY_FLOAT64) {
        *out_loop = (PyArrayMethod_StridedLoop *)&phy_f32_to_phy_f64_copy;
        *flags = 0;
        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "unsupported physical to physical cast");
    return -1;
}

// # region Numeric to PhysicalDType

static int
numeric_to_phy_resolve_descriptors(PyObject *NPY_UNUSED(self),
                                   PyArray_DTypeMeta *const *NPY_UNUSED(dtypes),
                                   PyArray_Descr *const *given_descrs, PyArray_Descr **loop_descrs,
                                   npy_intp *NPY_UNUSED(view_offset))
{
    PyArray_Descr *src_dtype = given_descrs[0];
    PhysicalDTypeObject *dst_dtype = (PhysicalDTypeObject *)given_descrs[1];
    if (dst_dtype == NULL) {
        PyErr_SetString(PyExc_TypeError, "Destination dtype is NULL");
        return -1;
    }

    if (src_dtype->type_num != dst_dtype->storage_descr->type_num) {
        // NOTE: So far, only same storage dtype casts are supported
        PyErr_Format(PyExc_TypeError, "Cannot cast from %s to %s", src_dtype->typeobj->tp_name,
                     dst_dtype->storage_descr->typeobj->tp_name);
        return -1;
    }

    loop_descrs[0] = (PyArray_Descr *)Py_NewRef(src_dtype);
    loop_descrs[1] = (PyArray_Descr *)Py_NewRef(dst_dtype);
    return NPY_SAFE_CASTING;
}

static int
numeric_to_phy_copy(PyArrayMethod_Context *context, char *const *data, npy_intp const *dimensions,
                    npy_intp const *strides, NpyAuxData *NPY_UNUSED(auxdata))
{
    PyArray_Descr *src = context->descriptors[0];
    npy_intp itemsize = src->elsize;

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
numeric_to_phy_get_loop(PyArrayMethod_Context *NPY_UNUSED(context), int NPY_UNUSED(aligned),
                        int NPY_UNUSED(move_references), const npy_intp *NPY_UNUSED(strides),
                        PyArrayMethod_StridedLoop **out_loop,
                        NpyAuxData **NPY_UNUSED(out_transferdata), NPY_ARRAYMETHOD_FLAGS *flags)
{
    *out_loop = (PyArrayMethod_StridedLoop *)&numeric_to_phy_copy;
    *flags = 0;
    return 0;
}

// #endregion

static PyArrayMethod_Spec *
create_cast_spec(const char *name, PyArray_DTypeMeta *src_dtype, PyArray_DTypeMeta *dst_dtype,
                 PyType_Slot *slots, NPY_CASTING casting)
{
    PyArrayMethod_Spec *spec = calloc(1, sizeof(*spec));
    if (spec == NULL) {
        return NULL;
    }

    spec->dtypes = malloc(2 * sizeof(*spec->dtypes));
    if (spec->dtypes == NULL) {
        free(spec);
        return NULL;
    }

    spec->dtypes[0] = src_dtype;
    spec->dtypes[1] = dst_dtype;

    spec->name = name;
    spec->nin = 1;
    spec->nout = 1;
    spec->flags = NPY_METH_SUPPORTS_UNALIGNED;
    spec->casting = casting;
    spec->slots = slots;

    return spec;
}

PyArrayMethod_Spec **
init_casts(PyArray_DTypeMeta *dtype_meta, bool include_mutual_casts)
{
    // Items must be total num of casts + 1 for the NULL terminator at the end.
    int num_casts = (include_mutual_casts ? 7 : 5);
    PyArrayMethod_Spec **casts = calloc(num_casts + 1, sizeof(*casts));
    if (casts == NULL) {
        return NULL;
    }

    // physical to physical dtypes array
    static PyType_Slot phy2phy_slots[] = {
            {NPY_METH_resolve_descriptors, &phy_to_phy_resolve_descriptors},
            {NPY_METH_get_loop, &phy_to_phy_get_loop},
            {0, NULL}};
    casts[0] = create_cast_spec("cast_PhysicalDType_to_PhysicalDType", dtype_meta, dtype_meta,
                                phy2phy_slots, NPY_UNSAFE_CASTING);
    if (casts[0] == NULL) {
        goto fail;
    }

    // physical to numeric dtypes array
    static PyType_Slot p2f_slots[] = {
            {NPY_METH_resolve_descriptors, (void *)&phy_to_numeric_resolve_descriptors},
            {NPY_METH_get_loop, (void *)&phy_to_numeric_get_loop},
            {0, NULL},
    };

    casts[1] = create_cast_spec("cast_PhysicalDType_to_Float32", dtype_meta, &PyArray_FloatDType,
                                p2f_slots, NPY_SAFE_CASTING);
    if (casts[1] == NULL) {
        goto fail;
    }

    casts[2] = create_cast_spec("cast_PhysicalDType_to_Float64", dtype_meta, &PyArray_DoubleDType,
                                p2f_slots, NPY_SAFE_CASTING);
    if (casts[2] == NULL) {
        goto fail;
    }

    // numeric to physical dtypes array
    static PyType_Slot f2p_slots[] = {
            {NPY_METH_resolve_descriptors, (void *)&numeric_to_phy_resolve_descriptors},
            {NPY_METH_get_loop, (void *)&numeric_to_phy_get_loop},
            {0, NULL},
    };

    casts[3] = create_cast_spec("cast_Float64_to_PhysicalDType", &PyArray_DoubleDType, dtype_meta,
                                f2p_slots, NPY_SAFE_CASTING);
    if (casts[3] == NULL) {
        goto fail;
    }

    casts[4] = create_cast_spec("cast_Float32_to_PhysicalDType", &PyArray_FloatDType, dtype_meta,
                                f2p_slots, NPY_SAFE_CASTING);
    if (casts[4] == NULL) {
        goto fail;
    }

    if (include_mutual_casts) {
        casts[5] = create_cast_spec("cast_PhysicalFloat32DType_to_PhysicalFloat64DType",
                                    &PhysicalFloat32DType, &PhysicalFloat64DType, phy2phy_slots,
                                    NPY_UNSAFE_CASTING);
        if (casts[5] == NULL) {
            goto fail;
        }

        casts[6] = create_cast_spec("cast_PhysicalFloat64DType_to_PhysicalFloat32DType",
                                    &PhysicalFloat64DType, &PhysicalFloat32DType, phy2phy_slots,
                                    NPY_SAME_KIND_CASTING);
        if (casts[6] == NULL) {
            goto fail;
        }
    }

    return casts;

fail:
    free_casts(casts);
    return NULL;
}

void
free_casts(PyArrayMethod_Spec **casts)
{
    if (casts == NULL) {
        return;
    }

    for (int i = 0; casts[i] != NULL; i++) {
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
