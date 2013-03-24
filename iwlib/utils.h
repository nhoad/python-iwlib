#include <Python.h>
#include <iwlib.h>

#define OPEN_IW(sock) { \
        char buffer[1024]; \
        if((sock = iw_sockets_open()) < 0) { \
            sprintf(buffer, "Error opening iwlib socket [Errno %d] %s", errno, strerror(errno)); \
            PyErr_SetString(PyExc_OSError, buffer); \
            return NULL; \
        } \
    }

//static PyObject *wireless_info_to_PyDict(struct wireless_info *info);
//static PyObject *wireless_config_to_PyDict(struct wireless_config *info);
