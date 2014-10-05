#include <Python.h>
#include <iwlib.h>

#define OPEN_IW(sock) { \
        char _buf[1024]; \
        if((sock = iw_sockets_open()) < 0) { \
            snprintf(_buf, 1024, "Error opening iwlib socket [Errno %d] %s", errno, strerror(errno)); \
            PyErr_SetString(PyExc_OSError, _buf); \
            return NULL; \
        } \
    }

#define SAFE_SETITEMSTRING(dict, key, value) do { \
    PyObject *__tmp; \
    __tmp = value; \
    if (__tmp) \
        PyDict_SetItemString(dict, key, __tmp); \
} while(0)


PyObject *wireless_info_to_PyDict(struct wireless_info *info);
PyObject *wireless_config_to_PyDict(struct wireless_config *info);
PyObject *wireless_scan_to_PyDict(struct wireless_scan *scan);


/* Python 2/3 compatibility helpers **********************************/

#if PY_MAJOR_VERSION >= 3
#define PY_FROMSTRING(X) PyBytes_FromString( (X) )
#define PY_FROMFORMAT(X, Y) PyBytes_FromFormat( (X), (Y) )
#else
#define PY_FROMSTRING(X) PyString_FromString( (X) )
#define PY_FROMFORMAT(X, Y) PyString_FromFormat( (X), (Y) )
#endif
