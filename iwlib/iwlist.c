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

#include <Python.h>

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iwlib.h>
#include <linux/sockios.h>
#include <sys/time.h>

#include "utils.h"

static PyObject *
scan(PyObject *self, PyObject *args)
{
    PyObject *dict, *list;
    char *devname;
    char buffer[1024];
    int has_range;
    int sock;
    struct iw_range range;
    struct wireless_scan *cur, *prev = NULL;
    wireless_scan_head head;

    if (!PyArg_ParseTuple(args, "s", &devname)) {
        return NULL;
    }

    OPEN_IW(sock);

    has_range = (iw_get_range_info(sock, devname, &range) >= 0);

    if((!has_range) || (range.we_version_compiled < 14)) {
        sprintf(buffer, "'%s' Interface doesn't support scanning", devname);
        goto error;
    }

    if (iw_scan(sock, devname, range.we_version_compiled, &head) != 0) {
        sprintf(buffer, "Error while scanning: %s", strerror(errno));
        goto error;
    }

    if ((list = PyList_New(0)) == NULL) {
        /* we can't goto error and wipe out a possible MemoryError. */
        iw_sockets_close(sock);
        return NULL;
    };

    cur = head.result;
    while (cur != NULL) {
        if ((dict = wireless_scan_to_PyDict(cur)) == NULL) {
            iw_sockets_close(sock);
            return list;
        };

        PyList_Append(list, dict);

        prev = cur;
        cur = cur->next;
        free(prev);
    }

    iw_sockets_close(sock);
    return list;

    error:
    iw_sockets_close(sock);
    PyErr_SetString(PyExc_OSError, buffer);
    return NULL;
}

static struct PyMethodDef PyEthModuleMethods[] = {
    { "scan",
        (PyCFunction) scan, METH_VARARGS,
        "Perform a scan for access points in the area. \
\n\nArguments:\
\n  - device to use for scanning on (e.g. eth1, wlan0)." },

    { NULL, NULL, 0, NULL }
};

#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef iwlist = {
        PyModuleDef_HEAD_INIT,
        "iwlist",
        NULL, // Documentation
        -1,
        PyEthModuleMethods
};

PyMODINIT_FUNC
PyInit_iwlist(void) {
    return PyModule_Create(&iwlist);
}

#else

void initiwlist(void) {
    PyObject *m;

    m = Py_InitModule("iwlist", PyEthModuleMethods);
    PyModule_GetDict(m);
}

#endif
