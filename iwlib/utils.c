/*
 * Copyright (C) 2009-2012 Red Hat, Inc.
 * Copyright (C) 2013 Nathan Hoad.
 *
 * Interface with iwlib from rhpl by Harald Hoyer <harald@redhat.com>.
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

PyObject*
wireless_config_to_PyDict(struct wireless_config *basic)
{
    char buffer[1024];
    PyObject *dict = PyDict_New();
    if (basic->has_mode)
        PyDict_SetItemString(dict, "Mode",
                PyString_FromString(iw_operation_mode[basic->mode]));
    if (basic->essid_on) {
        PyDict_SetItemString(dict, "ESSID", PyString_FromString(basic->essid));
    } else {
        PyDict_SetItemString(dict, "ESSID", PyString_FromString("Auto"));
    }

    if (basic->has_nwid) {
        if(basic->nwid.disabled)
            PyDict_SetItemString(dict, "NWID", PyString_FromString("Auto"));
        else {
            PyDict_SetItemString(dict, "NWID", PyString_FromFormat("%X", basic->nwid.value));
        }
    }

    if(basic->has_freq) {
        iw_print_freq_value(buffer, sizeof(buffer), basic->freq);
        PyDict_SetItemString(dict, "Frequency", PyString_FromString(buffer));
    }

    // FIXME: This is known to be incorrect for wireless_scan structs.
    if(basic->has_key) {
        if((basic->key_flags & IW_ENCODE_DISABLED) || (basic->key_size == 0)) {
            PyDict_SetItemString(dict, "Key", PyString_FromString("off"));
        } else {
            iw_print_key(buffer, sizeof(buffer), basic->key, basic->key_size, basic->key_flags);
            PyDict_SetItemString(dict, "Key", PyString_FromString(buffer));
        }
    }
    return dict;
}

add_wireless_stats_toPyDict(iwstats *stats, PyObject *dict) {
    PyObject *quality = PyDict_New();

    PyDict_SetItemString(quality, "quality", Py_BuildValue("i", stats->qual.qual));
    PyDict_SetItemString(quality, "level", Py_BuildValue("i", stats->qual.level));
    PyDict_SetItemString(quality, "noise", Py_BuildValue("i", stats->qual.noise));
    PyDict_SetItemString(quality, "updated", Py_BuildValue("i", stats->qual.updated));

    PyDict_SetItemString(dict, "stats", quality);
}

PyObject*
wireless_info_to_PyDict(struct wireless_info *info)
{
    char buffer[1024];
    PyObject *dict = wireless_config_to_PyDict(&info->b);

    if(info->has_ap_addr) {
        iw_saether_ntop((struct sockaddr *)info->ap_addr.sa_data, buffer);
        if((info->b.has_mode) && (info->b.mode == IW_MODE_ADHOC))
            PyDict_SetItemString(dict, "Cell", PyString_FromString(buffer));
        else
            PyDict_SetItemString(dict, "Access Point", PyString_FromString(buffer));
    }

    if(info->has_bitrate) {
        iw_print_bitrate(buffer, sizeof(buffer), info->bitrate.value);
        PyDict_SetItemString(dict, "BitRate", PyString_FromString(buffer));
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
    PyObject *dict = wireless_config_to_PyDict(&scan->b);

    if(scan->has_ap_addr) {
        iw_saether_ntop((struct sockaddr *)scan->ap_addr.sa_data, buffer);
        if((scan->b.has_mode) && (scan->b.mode == IW_MODE_ADHOC))
            PyDict_SetItemString(dict, "Cell", PyString_FromString(buffer));
        else
            PyDict_SetItemString(dict, "Access Point", PyString_FromString(buffer));
    }

    if(scan->has_maxbitrate) {
        iw_print_bitrate(buffer, sizeof(buffer), scan->maxbitrate.value);
        PyDict_SetItemString(dict, "BitRate", PyString_FromString(buffer));
    }

    if (scan->has_stats) {
        add_wireless_stats_toPyDict(&scan->stats, dict);
    }

    return dict;
}

// XXX All functions below are exported to iwlib.utils

PyObject *
get_max_quality(PyObject *self, PyObject *args)
{
    char *devname;
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
    char *devname;
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
        (PyCFunction) get_max_quality, METH_VARARGS, NULL },
    { "supports_scanning",
        (PyCFunction) supports_scanning, METH_VARARGS, NULL },
    { NULL, NULL, 0, NULL }
};

void initutils(void) {
    PyObject *m, *d;

    m = Py_InitModule("utils", PyEthModuleMethods);
    d = PyModule_GetDict(m);
}
