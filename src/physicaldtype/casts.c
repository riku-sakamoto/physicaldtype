#include <Python.h>

#define NPY_NO_DEPRECATED_API NPY_2_0_API_VERSION
#define NPY_TARGET_VERSION NPY_2_0_API_VERSION
#define NO_IMPORT_ARRAY
#define NO_IMPORT_UFUNC
#include "numpy/arrayobject.h"
#include "numpy/dtype_api.h"
#include "numpy/ndarraytypes.h"

#include "casts.h"
#include "dtype.h"

static int
phy_to_float64_contiguous(char **args, npy_intp const *dimensions, npy_intp const *strides,
                          NpyAuxData *NPY_UNUSED(auxdata))
{
    npy_intp N = dimensions[0];
    double *in = (double *)args[0];
    double *out = (double *)args[1];

    for (npy_intp i = 0; i < N; i++) {
        out[i] = in[i];
    }
    return 0;
}

static int
phy_to_float64_strided(char **args, npy_intp const *dimensions, npy_intp const *strides,
                       NpyAuxData *NPY_UNUSED(auxdata))
{
    npy_intp N = dimensions[0];
    char *in = args[0];
    char *out = args[1];
    npy_intp in_stride = strides[0];
    npy_intp out_stride = strides[1];

    while (N--) {
        *(double *)out = *(double *)in;
        out += out_stride;
        in += in_stride;
    }

    return 0;
}

static int
phy_to_float64_unaligned(char **args, npy_intp const *dimensions, npy_intp const *strides,
                         NpyAuxData *NPY_UNUSED(auxdata))
{
    npy_intp N = dimensions[0];
    char *in = args[0];
    char *out = args[1];
    npy_intp in_stride = strides[0];
    npy_intp out_stride = strides[1];

    while (N--) {
        double in_val, out_val;
        memcpy(&in_val, in, sizeof(double));  // NOLINT
        out_val = in_val;
        memcpy(out, &out_val, sizeof(double));  // NOLINT
        out += out_stride;
        in += in_stride;
    }

    return 0;
}

static int
phy_to_float64_get_loop(PyArrayMethod_Context *context, int aligned,
                        int NPY_UNUSED(move_references), const npy_intp *strides,
                        PyArrayMethod_StridedLoop **out_loop, NpyAuxData **out_transferdata,
                        NPY_ARRAYMETHOD_FLAGS *flags)
{
    int contig = (strides[0] == sizeof(double) && strides[1] == sizeof(double));

    if (aligned && contig) {
        *out_loop = (PyArrayMethod_StridedLoop *)&phy_to_float64_contiguous;
    }
    else if (aligned) {
        *out_loop = (PyArrayMethod_StridedLoop *)&phy_to_float64_strided;
    }
    else {
        *out_loop = (PyArrayMethod_StridedLoop *)&phy_to_float64_unaligned;
    }

    *flags = 0;
    return 0;
}

// #region PhysicalDtype to PhysicalDtype

static NPY_CASTING
phy_to_phy_resolve_descriptors(PyObject *NPY_UNUSED(self), PyArray_DTypeMeta *NPY_UNUSED(dtypes[2]),
                               PyArray_Descr *given_descrs[2], PyArray_Descr *loop_descrs[2],
                               npy_intp *view_offset)
{
    if (given_descrs[1] == NULL) {
        Py_INCREF(given_descrs[0]);
        loop_descrs[1] = given_descrs[0];
    }
    else {
        // HACK: Need to convert with proper casting
        Py_INCREF(given_descrs[1]);
        loop_descrs[1] = given_descrs[1];
    }

    return NPY_SAFE_CASTING;
}

/*
 * NumPy currently allows NULL for the own DType/"cls".
 */
static PyArray_DTypeMeta *phy2phy_dtypes[2] = {NULL, NULL};

static PyType_Slot phy2phy_slots[] = {
        {NPY_METH_resolve_descriptors, &phy_to_phy_resolve_descriptors},
        // {NPY_METH_get_loop, &phy_to_phy_get_loop},
        {0, NULL}};

static PyArrayMethod_Spec PhyToPhyCastSpec = {
        .name = "cast_PhysicalDType_to_PhysicalDType",
        .nin = 1,
        .nout = 1,
        .flags = NPY_METH_SUPPORTS_UNALIGNED,
        .casting = NPY_SAFE_CASTING,
        .dtypes = phy2phy_dtypes,
        .slots = phy2phy_slots,
};

// #endregion

static PyType_Slot p2f_slots[] = {{NPY_METH_get_loop, &phy_to_float64_get_loop}, {0, NULL}};

static char *p2f_name = "cast_PhysicalDType_to_Float64";

PyArrayMethod_Spec **
init_casts(void)
{
    PyArray_DTypeMeta **p2f_dtypes = malloc(2 * sizeof(PyArray_DTypeMeta *));
    p2f_dtypes[0] = NULL;
    p2f_dtypes[1] = &PyArray_DoubleDType;

    PyArrayMethod_Spec *PhyToFloat64CastSpec = malloc(sizeof(PyArrayMethod_Spec));
    PhyToFloat64CastSpec->name = p2f_name;
    PhyToFloat64CastSpec->nin = 1;
    PhyToFloat64CastSpec->nout = 1;
    PhyToFloat64CastSpec->flags = NPY_METH_SUPPORTS_UNALIGNED;
    PhyToFloat64CastSpec->casting = NPY_SAFE_CASTING;
    PhyToFloat64CastSpec->dtypes = p2f_dtypes;
    PhyToFloat64CastSpec->slots = p2f_slots;

    PyArrayMethod_Spec **casts = malloc(3 * sizeof(PyArrayMethod_Spec *));
    casts[0] = &PhyToPhyCastSpec;
    casts[1] = PhyToFloat64CastSpec;
    casts[2] = NULL;

    return casts;
}
