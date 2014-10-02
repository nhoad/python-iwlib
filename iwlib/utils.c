/*
 * Copyright (C) 2009-2012 Red Hat, Inc.
 * Copyright (C) 2013 Nathan Hoad.
 *
 * Interface with iwlib by Nathan Hoad <nathan@getoffmalawn.com>.
 *
 * This application is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; version 2.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include "utils.h"

#if PY_MAJOR_VERSION >= 3
#define PY_FROMSTRING(X) PyBytes_FromString( (X) )
#define PY_FROMFORMAT(X, Y) PyBytes_FromFormat( (X), (Y) )
#else
#define PY_FROMSTRING(X) PyString_FromString( (X) )
#define PY_FROMFORMAT(X, Y) PyString_FromFormat( (X), (Y) )
#endif

PyObject*
wireless_config_to_PyDict(struct wireless_config *basic)
{
    char buffer[1024];
    PyObject *dict;

    if ((dict = PyDict_New()) == NULL)
        return NULL;

    if (basic->has_mode)
        SAFE_SETITEMSTRING(dict, "Mode", PY_FROMSTRING(iw_operation_mode[basic->mode]));

    if (basic->essid_on) {
        SAFE_SETITEMSTRING(dict, "ESSID", PY_FROMSTRING(basic->essid));
    } else {
        SAFE_SETITEMSTRING(dict, "ESSID", PY_FROMSTRING("Auto"));
    }

    if (basic->has_nwid) {
        if(basic->nwid.disabled) {
            SAFE_SETITEMSTRING(dict, "NWID", PY_FROMSTRING("Auto"));
        } else {
            SAFE_SETITEMSTRING(dict, "NWID",
                PY_FROMFORMAT("%X", basic->nwid.value));
        }
    }

    if(basic->has_freq) {
        iw_print_freq_value(buffer, sizeof(buffer), basic->freq);
        SAFE_SETITEMSTRING(dict, "Frequency", PY_FROMSTRING(buffer));
    }

    /* FIXME: This is known to be incorrect for wireless_scan structs. */
    if(basic->has_key) {
        if((basic->key_flags & IW_ENCODE_DISABLED) || (basic->key_size == 0)) {
            SAFE_SETITEMSTRING(dict, "Key", PY_FROMSTRING("off"));
        } else {
            iw_print_key(buffer, sizeof(buffer), basic->key, basic->key_size, basic->key_flags);
            SAFE_SETITEMSTRING(dict, "Key", PY_FROMSTRING(buffer));
        }
    }
    return dict;
}

void
add_wireless_stats_toPyDict(iwstats *stats, PyObject *dict) {
    PyObject *quality;

    if (!dict || !stats) {
        return;
    }

    if ((quality = PyDict_New()) == NULL)
        return;

    SAFE_SETITEMSTRING(quality, "quality", Py_BuildValue("i", stats->qual.qual));
    SAFE_SETITEMSTRING(quality, "level", Py_BuildValue("i", stats->qual.level));
    SAFE_SETITEMSTRING(quality, "noise", Py_BuildValue("i", stats->qual.noise));
    SAFE_SETITEMSTRING(quality, "updated", Py_BuildValue("i", stats->qual.updated));

    SAFE_SETITEMSTRING(dict, "stats", quality);
}

PyObject*
wireless_info_to_PyDict(struct wireless_info *info)
{
    char buffer[1024];
    PyObject *dict = wireless_config_to_PyDict(&info->b);

    if(info->has_ap_addr) {
        iw_ether_ntop((struct ether_addr *)info->ap_addr.sa_data, buffer);
        if((info->b.has_mode) && (info->b.mode == IW_MODE_ADHOC))
            SAFE_SETITEMSTRING(dict, "Cell", PY_FROMSTRING(buffer));
        else
            SAFE_SETITEMSTRING(dict, "Access Point", PY_FROMSTRING(buffer));
    }

    if(info->has_bitrate) {
        iw_print_bitrate(buffer, sizeof(buffer), info->bitrate.value);
        SAFE_SETITEMSTRING(dict, "BitRate", PY_FROMSTRING(buffer));
    }

    if (info->has_stats) {
        add_wireless_stats_toPyDict(&info->stats, dict);
    }

    return dict;
}

PyObject*
wireless_scan_to_PyDict(struct wireless_scan *scan)
{
    char buffer[1024];
    PyObject *dict;

    if ((dict = wireless_config_to_PyDict(&scan->b)) == NULL)
        return NULL;

    if(scan->has_ap_addr) {
        iw_ether_ntop((struct ether_addr *)scan->ap_addr.sa_data, buffer);
        if((scan->b.has_mode) && (scan->b.mode == IW_MODE_ADHOC))
            SAFE_SETITEMSTRING(dict, "Cell", PY_FROMSTRING(buffer));
        else
            SAFE_SETITEMSTRING(dict, "Access Point", PY_FROMSTRING(buffer));
    }

    if(scan->has_maxbitrate) {
        iw_print_bitrate(buffer, sizeof(buffer), scan->maxbitrate.value);
        SAFE_SETITEMSTRING(dict, "BitRate", PY_FROMSTRING(buffer));
    }

    if (scan->has_stats) {
        add_wireless_stats_toPyDict(&scan->stats, dict);
    }

    return dict;
}

/* XXX All functions below are exported to iwlib.utils */

PyObject *
get_max_quality(PyObject *self, PyObject *args)
{
    const char *devname;
    char buffer[1024];
    int has_range;
    int sock;
    struct iw_range range;

    if (!PyArg_ParseTuple(args, "s", &devname)) {
        return NULL;
    }

    OPEN_IW(sock);

    has_range = (iw_get_range_info(sock, devname, &range) >= 0);

    if((!has_range) || (range.we_version_compiled < 14)) {
        sprintf(buffer, "'%s' Interface doesn't support scanning", devname);
        PyErr_SetString(PyExc_OSError, buffer);
        return NULL;
    }

    iw_sockets_close(sock);
    return Py_BuildValue("i", range.max_qual.qual);
}

PyObject *
supports_scanning(PyObject *self, PyObject *args)
{
    const char *devname;
    int has_range;
    int sock;
    struct iw_range range;
    PyObject *res;

    if (!PyArg_ParseTuple(args, "s", &devname)) {
        return NULL;
    }

    OPEN_IW(sock);

    has_range = (iw_get_range_info(sock, devname, &range) >= 0);
    iw_sockets_close(sock);

    if((!has_range) || (range.we_version_compiled < 14)) {
        res = Py_False;
    } else {
        res = Py_True;
    }

    Py_INCREF(res);
    return res;
}

static struct PyMethodDef PyEthModuleMethods[] = {
    { "get_max_quality",
        (PyCFunction) get_max_quality, METH_VARARGS,
        "Return max quality of an interface. Useful for \
\n\nworking out percentages of quality results from \
\niwconfig.scan" },
    { "supports_scanning",
        (PyCFunction) supports_scanning, METH_VARARGS,
        "Check if an interface supports scanning. \
\n\nReturns true if the device supports scanning, False otherwise." },
    { NULL, NULL, 0, NULL }
};

#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef utilsmodule = {
        PyModuleDef_HEAD_INIT,
        "utils",
        NULL, // Documentation
        -1,
        PyEthModuleMethods
};

PyMODINIT_FUNC
PyInit_utils(void) {
    return PyModule_Create(&utilsmodule);
}

#else // Python2

void initutils(void) {
    PyObject *m;
    m = Py_InitModule("utils", PyEthModuleMethods);
    PyModule_GetDict(m);
}
#endif
