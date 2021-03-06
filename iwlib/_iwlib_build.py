# Copyright (C) 2009-2012 Red Hat, Inc.
# Copyright (C) 2013-2014 Nathan Hoad.
#
# Interface with iwlib by Nathan Hoad <nathan@getoffmalawn.com>.
#
# This application is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; version 2.
#
# This application is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

from cffi import FFI

ffibuilder = FFI()


funcs = """
    double iw_freq2float(const iwfreq *in);
    int iw_get_ext(int sock, char *interface, int flag, struct iwreq *req);
    int iw_get_kernel_we_version();
    int iw_get_range_info(int skfd, const char *ifname, iwrange *range);
    int iw_get_stats(int skfd, const char *ifname, iwstats *stats, iwrange *range, int has_range);
    int iw_scan(int sock, char *ifname, int we_version, wireless_scan_head *context);
    int iw_set_ext(int sock, char *interface, int flag, struct iwreq *req);
    int iw_sockets_open();
    void iw_ether_ntop(const struct ether_addr *eth, char *buf);
    void iw_print_bitrate(char *buffer, int buflen, int bitrate);
    void iw_print_freq_value(char *buffer, int buflen, double freq);
    void iw_sockets_close(int sock);
    int ioctl(int fildes, unsigned long int request, ...);
"""

externs = """
    extern const char * const iw_operation_mode[];
"""

defs = """
    #define IW_ESSID_MAX_SIZE ...
    #define SIOCSIWESSID ...
    #define SIOCGIWESSID ...
    #define SIOCSIWCOMMIT ...
    #define SIOCGIWNAME ...
    #define SIOCSIWNWID ...
    #define SIOCGIWNWID ...
    #define SIOCSIWFREQ ...
    #define SIOCGIWFREQ ...
    #define SIOCSIWMODE ...
    #define SIOCGIWMODE ...
    #define SIOCSIWSENS ...
    #define SIOCGIWSENS ...
    #define SIOCSIWRANGE ...
    #define SIOCGIWRANGE ...
    #define SIOCSIWPRIV ...
    #define SIOCGIWPRIV ...
    #define SIOCSIWSTATS ...
    #define SIOCGIWSTATS ...
    #define SIOCSIWSPY ...
    #define SIOCGIWSPY ...
    #define SIOCSIWTHRSPY ...
    #define SIOCGIWTHRSPY ...
    #define SIOCSIWAP ...
    #define SIOCGIWAP ...
    #define SIOCGIWAPLIST ...
    #define SIOCSIWSCAN ...
    #define SIOCGIWSCAN ...
    #define SIOCSIWESSID ...
    #define SIOCGIWESSID ...
    #define SIOCSIWNICKN ...
    #define SIOCGIWNICKN ...
    #define SIOCSIWRATE ...
    #define SIOCGIWRATE ...
    #define SIOCSIWRTS ...
    #define SIOCGIWRTS ...
    #define SIOCSIWFRAG ...
    #define SIOCGIWFRAG ...
    #define SIOCSIWTXPOW ...
    #define SIOCGIWTXPOW ...
    #define SIOCSIWRETRY ...
    #define SIOCGIWRETRY ...
    #define SIOCSIWENCODE ...
    #define SIOCGIWENCODE ...
    #define SIOCSIWPOWER ...
    #define SIOCGIWPOWER ...
    #define SIOCSIWMODUL ...
    #define SIOCGIWMODUL ...
    #define SIOCSIWGENIE ...
    #define SIOCGIWGENIE ...
    #define SIOCSIWMLME ...
    #define SIOCSIWAUTH ...
    #define SIOCGIWAUTH ...
    #define SIOCSIWENCODEEXT ...
    #define SIOCGIWENCODEEXT ...
    #define SIOCSIWPMKSA ...
    #define SIOCIWFIRSTPRIV ...
    #define SIOCIWLASTPRIV ...

    #define IW_MODE_AUTO ...
    #define IW_MODE_ADHOC ...
    #define IW_MODE_INFRA ...
    #define IW_MODE_MASTER ...
    #define IW_MODE_REPEAT ...
    #define IW_MODE_SECOND ...
    #define IW_MODE_MONITOR ...

    #define IW_NUM_OPER_MODE ...
    #define IW_ENCODING_TOKEN_MAX ...
    #define IW_ENCODE_DISABLED ...

    #define IFNAMSIZ ...
    #define SIOCGIFFLAGS ...
"""

structs = """
    typedef struct sockaddr {
        char sa_data[14];
        ...;
    } sockaddr;

    typedef struct iw_param {
        int value;
        unsigned char disabled;
        ...;
    } iwparam;

    struct wireless_config {
        int has_mode;
        int mode;
        int essid_on;
        char essid[];
        ...;
    };

    typedef struct iw_statistics {
        struct iw_quality qual;
        ...;
    } iwstats;

    typedef struct wireless_scan {
        struct wireless_scan *next;
        int has_ap_addr;
        int has_stats;
        int has_maxbitrate;
        iwparam maxbitrate;
        iwstats stats;
        struct wireless_config b;
        sockaddr ap_addr;
        ...;
    } wireless_scan;

    typedef struct wireless_scan_head {
        wireless_scan *result;
        int retry;
    } wireless_scan_head;

    struct iw_quality {
        unsigned char qual;
        unsigned char level;
        unsigned char noise;
        unsigned char updated;
        ...;
    };

    struct iw_range {
       unsigned char we_version_compiled;
       struct iw_quality max_qual;
       ...;
    };

    typedef struct iw_range iwrange;

    struct iw_point {
        void *pointer;
        unsigned short length;
        unsigned short flags;
    };

    typedef struct iw_freq {
        int m;
        short e;
        unsigned char i;
        unsigned char flags;
    } iwfreq;

    union iwreq_data {
        struct iw_point essid;
        struct iw_point data;
        struct iw_freq freq;
        sockaddr ap_addr;
        int mode;
        iwparam bitrate;
        iwparam power;
        iwparam nwid;
        ...;
    };

    struct iwreq {
        union iwreq_data u;
        char ifr_name[...];
        ...;
    };
"""

ffibuilder.set_source("iwlib._iwlib", "#include <iwlib.h>", libraries=['iw'])
ffibuilder.cdef(structs + externs + defs + funcs)

iwlib = ffibuilder.verify("#include <iwlib.h>", libraries=['iw'])

if __name__ == "__main__":
        ffibuilder.compile()
