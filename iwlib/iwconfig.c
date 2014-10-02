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

#include "utils.h"

/*------------------------------------------------------------------*/
/*
 * Get wireless informations & config from the device driver
 * We will call all the classical wireless ioctl on the driver through
 * the socket to know what is supported and to get the settings...
 */
static int
get_info(int skfd, char *ifname, struct wireless_info *info)
{
    struct iwreq wrq;

    memset((char *) info, 0, sizeof(struct wireless_info));

    /* Get wireless name */
    if(iw_get_ext(skfd, ifname, SIOCGIWNAME, &wrq) < 0) {
        /* If no wireless name : no wireless extensions */
        /* But let's check if the interface exists at all */
        struct ifreq ifr;

        strncpy(&ifr.ifr_name[0], ifname, IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ-1] = 0;
        if(ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
            return(-ENODEV);
        else
            return(-ENOTSUP);
    } else {
        strncpy(info->b.name, wrq.u.name, sizeof(info->b.name));
        info->b.name[sizeof(info->b.name)-1] = 0;
    }

    /* Get ranges */
    if(iw_get_range_info(skfd, ifname, &(info->range)) >= 0)
        info->has_range = 1;

    /* Get network ID */
    if(iw_get_ext(skfd, ifname, SIOCGIWNWID, &wrq) >= 0) {
        info->b.has_nwid = 1;
        memcpy(&(info->b.nwid), &(wrq.u.nwid), sizeof(iwparam));
    }

    /* Get frequency / channel */
    if(iw_get_ext(skfd, ifname, SIOCGIWFREQ, &wrq) >= 0) {
        info->b.has_freq = 1;
        info->b.freq = iw_freq2float(&(wrq.u.freq));
    }

    /* Get sensitivity */
    if(iw_get_ext(skfd, ifname, SIOCGIWSENS, &wrq) >= 0) {
        info->has_sens = 1;
        memcpy(&(info->sens), &(wrq.u.sens), sizeof(iwparam));
    }

    /* Get encryption information */
    wrq.u.data.pointer = (caddr_t) info->b.key;
    wrq.u.data.length = IW_ENCODING_TOKEN_MAX;
    wrq.u.data.flags = 0;
    if(iw_get_ext(skfd, ifname, SIOCGIWENCODE, &wrq) >= 0) {
        info->b.has_key = 1;
        info->b.key_size = wrq.u.data.length;
        info->b.key_flags = wrq.u.data.flags;
    }

    /* Get ESSID */
    wrq.u.essid.pointer = (caddr_t) info->b.essid;
    wrq.u.essid.length = IW_ESSID_MAX_SIZE;
    wrq.u.essid.flags = 0;
    if(iw_get_ext(skfd, ifname, SIOCGIWESSID, &wrq) >= 0) {
        info->b.has_essid = 1;
        info->b.essid_on = wrq.u.data.flags;
    }

    /* Get AP address */
    if(iw_get_ext(skfd, ifname, SIOCGIWAP, &wrq) >= 0) {
        info->has_ap_addr = 1;
        memcpy(&(info->ap_addr), &(wrq.u.ap_addr), sizeof (sockaddr));
    }

    /* Get NickName */
    wrq.u.essid.pointer = (caddr_t) info->nickname;
    wrq.u.essid.length = IW_ESSID_MAX_SIZE;
    wrq.u.essid.flags = 0;
    if(iw_get_ext(skfd, ifname, SIOCGIWNICKN, &wrq) >= 0)
        if(wrq.u.data.length > 1)
            info->has_nickname = 1;

    /* Get bit rate */
    if(iw_get_ext(skfd, ifname, SIOCGIWRATE, &wrq) >= 0) {
        info->has_bitrate = 1;
        memcpy(&(info->bitrate), &(wrq.u.bitrate), sizeof(iwparam));
    }

    /* Get RTS threshold */
    if(iw_get_ext(skfd, ifname, SIOCGIWRTS, &wrq) >= 0) {
        info->has_rts = 1;
        memcpy(&(info->rts), &(wrq.u.rts), sizeof(iwparam));
    }

    /* Get fragmentation threshold */
    if(iw_get_ext(skfd, ifname, SIOCGIWFRAG, &wrq) >= 0) {
        info->has_frag = 1;
        memcpy(&(info->frag), &(wrq.u.frag), sizeof(iwparam));
    }

    /* Get operation mode */
    if(iw_get_ext(skfd, ifname, SIOCGIWMODE, &wrq) >= 0) {
        info->b.mode = wrq.u.mode;
        if((info->b.mode < IW_NUM_OPER_MODE) && (info->b.mode >= 0))
            info->b.has_mode = 1;
    }

    /* Get Power Management settings */
    wrq.u.power.flags = 0;
    if(iw_get_ext(skfd, ifname, SIOCGIWPOWER, &wrq) >= 0) {
        info->has_power = 1;
        memcpy(&(info->power), &(wrq.u.power), sizeof(iwparam));
    }

#if WIRELESS_EXT > 9
    /* Get Transmit Power */
    if(iw_get_ext(skfd, ifname, SIOCGIWTXPOW, &wrq) >= 0) {
        info->has_txpower = 1;
        memcpy(&(info->txpower), &(wrq.u.txpower), sizeof(iwparam));
    }
#endif

#if WIRELESS_EXT > 10
    /* Get retry limit/lifetime */
    if(iw_get_ext(skfd, ifname, SIOCGIWRETRY, &wrq) >= 0) {
        info->has_retry = 1;
        memcpy(&(info->retry), &(wrq.u.retry), sizeof(iwparam));
    }
#endif  /* WIRELESS_EXT > 10 */

    /* Get stats */
    if(iw_get_stats(skfd, ifname, &(info->stats), &(info->range), info->has_range) >= 0) {
        info->has_stats = 1;
    }

    return(0);
}

static PyObject *
get_iwconfig (PyObject * self, PyObject * args)
{
    char *devname;
    int skfd;
    char buffer[1024];
    int eno = 0;
    struct wireless_info info;

    if (!PyArg_ParseTuple(args, "s", &devname)) {
        return NULL;
    }

    OPEN_IW(skfd);
    eno = get_info(skfd, devname, &info);
    iw_sockets_close(skfd);

    if (eno < 0) {
        eno = -eno;
        sprintf(buffer, "get_info [Errno %d] %s", eno, strerror(eno));
        PyErr_SetString(PyExc_IOError, buffer);
        return NULL;
    }

    return wireless_info_to_PyDict(&info);
}

static PyObject *
set_essid(PyObject *self, PyObject *args)
{
    char buffer[1024];
    char essid_buf[IW_ESSID_MAX_SIZE+1];
    const char *devname, *essid;
    int essid_len;
    int sock;
    int we_kernel_version;
    struct iwreq wrq;

    if (!PyArg_ParseTuple(args, "ss", &devname, &essid)) {
        return NULL;
    }

    essid_len = strlen(essid);

    OPEN_IW(sock);

    if((!strcasecmp(essid, "off")) || (!strcasecmp(essid, "any"))) {
        wrq.u.essid.flags = 0;
        essid_buf[0] = '\0';
    } else if (!strcasecmp(essid, "on")) {
        memset(essid_buf, '\0', sizeof(essid_buf));
        wrq.u.essid.pointer = (caddr_t) essid_buf;
        wrq.u.essid.length = IW_ESSID_MAX_SIZE + 1;
        wrq.u.essid.flags = 0;
        if(iw_get_ext(sock, devname, SIOCGIWESSID, &wrq) < 0) {
            sprintf(buffer, "Error retrieving previous ESSID: %s",
                strerror(errno));
            goto error;
        }
        wrq.u.essid.flags = 1;
    } else if(essid_len > IW_ESSID_MAX_SIZE) {
        sprintf(buffer, "ESSID is longer than the maximum %d",
            IW_ESSID_MAX_SIZE);
        goto error;
    } else {
        memcpy(essid_buf, essid, essid_len);
        essid_buf[essid_len] = '\0';
        wrq.u.essid.flags = 1;
    }

    we_kernel_version = iw_get_kernel_we_version();

    wrq.u.essid.pointer = (caddr_t) essid_buf;
    wrq.u.essid.length = strlen(essid_buf);
    if(we_kernel_version < 21)
        wrq.u.essid.length++;

    if(iw_set_ext(sock, devname, SIOCSIWESSID, &wrq) < 0) {
        sprintf(buffer, "Couldn't set essid on device '%s': %s", devname,
            strerror(errno));
        goto error;
    }

    iw_sockets_close(sock);
    Py_INCREF(Py_None);
    return Py_None;

    error:
    iw_sockets_close(sock);
    PyErr_SetString(PyExc_OSError, buffer);
    return NULL;
}

static struct PyMethodDef PyEthModuleMethods[] = {
    { "get_iwconfig",
        (PyCFunction) get_iwconfig, METH_VARARGS,
        "Retrieve the current configuration of an interface. \
\n\nArguments:\
\n  - device to work on (e.g. eth1, wlan0)." },
    { "set_essid",
        (PyCFunction) set_essid, METH_VARARGS,
        "Set the ESSID of an interface. \
\n\nArguments:\
\n  - device to work on (e.g. eth1, wlan0).\
\n  - essid to set device to." },
    { NULL, NULL, 0, NULL }
};

#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef iwconfig = {
        PyModuleDef_HEAD_INIT,
        "iwconfig",
        NULL, // Documentation
        -1,
        PyEthModuleMethods
};

PyMODINIT_FUNC
PyInit_iwconfig(void) {
    return PyModule_Create(&iwconfig);
}

#else // Python2

void initiwconfig(void) {
    PyObject *m;
    m = Py_InitModule("iwconfig", PyEthModuleMethods);
    PyModule_GetDict(m);
}

#endif
