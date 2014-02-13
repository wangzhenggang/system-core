/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* This file is used to define the properties of the filesystem
** images generated by build tools (mkbootfs and mkyaffs2image) and
** by the device side of adb.
*/

#ifndef _ANDROID_FILESYSTEM_CONFIG_H_
#define _ANDROID_FILESYSTEM_CONFIG_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <stdint.h>

#if defined(__ANDROID__)
#include <linux/capability.h>
#else
#include "android_filesystem_capability.h"
#endif

#define CAP_MASK_LONG(cap_name)  (1ULL << (cap_name))

/* This is the master Users and Groups config for the platform.
 * DO NOT EVER RENUMBER
 */

#define AID_ROOT             0  /* traditional unix root user */

#define AID_SYSTEM        1000  /* system server */

#define AID_RADIO         1001  /* telephony subsystem, RIL */
#define AID_BLUETOOTH     1002  /* bluetooth subsystem */
#define AID_GRAPHICS      1003  /* graphics devices */
#define AID_INPUT         1004  /* input devices */
#define AID_AUDIO         1005  /* audio devices */
#define AID_CAMERA        1006  /* camera devices */
#define AID_LOG           1007  /* log devices */
#define AID_COMPASS       1008  /* compass device */
#define AID_MOUNT         1009  /* mountd socket */
#define AID_WIFI          1010  /* wifi subsystem */
#define AID_ADB           1011  /* android debug bridge (adbd) */
#define AID_INSTALL       1012  /* group for installing packages */
#define AID_MEDIA         1013  /* mediaserver process */
#define AID_DHCP          1014  /* dhcp client */
#define AID_SDCARD_RW     1015  /* external storage write access */
#define AID_VPN           1016  /* vpn system */
#define AID_KEYSTORE      1017  /* keystore subsystem */
#define AID_USB           1018  /* USB devices */
#define AID_DRM           1019  /* DRM server */
#define AID_MDNSR         1020  /* MulticastDNSResponder (service discovery) */
#define AID_GPS           1021  /* GPS daemon */
#define AID_UNUSED1       1022  /* deprecated, DO NOT USE */
#define AID_MEDIA_RW      1023  /* internal media storage write access */
#define AID_MTP           1024  /* MTP USB driver access */
#define AID_UNUSED2       1025  /* deprecated, DO NOT USE */
#define AID_DRMRPC        1026  /* group for drm rpc */
#define AID_NFC           1027  /* nfc subsystem */
#define AID_SDCARD_R      1028  /* external storage read access */
#define AID_CLAT          1029  /* clat part of nat464 */
#define AID_LOOP_RADIO    1030  /* loop radio devices */
#define AID_MEDIA_DRM     1031  /* MediaDrm plugins */
#define AID_PACKAGE_INFO  1032  /* access to installed package details */
#define AID_SDCARD_PICS   1033  /* external storage photos access */
#define AID_SDCARD_AV     1034  /* external storage audio/video access */
#define AID_SDCARD_ALL    1035  /* access all users external storage */
#define AID_LOGD          1036  /* log daemon */
#define AID_SHARED_RELRO  1037  /* creator of shared GNU RELRO files */
#define AID_DBUS          1038  /* dbus-daemon IPC broker process */
#define AID_TLSDATE       1039  /* tlsdate unprivileged user */
#define AID_MEDIA_EX      1040  /* mediaextractor process */
#define AID_AUDIOSERVER   1041  /* audioserver process */
#define AID_METRICS_COLL  1042  /* metrics_collector process */
#define AID_METRICSD      1043  /* metricsd process */
#define AID_WEBSERV       1044  /* webservd process */
#define AID_DEBUGGERD     1045  /* debuggerd unprivileged user */
#define AID_MEDIA_CODEC   1046  /* mediacodec process */
#define AID_CAMERASERVER  1047  /* cameraserver process */
#define AID_FIREWALL      1048  /* firewalld process */
#define AID_TRUNKS        1049  /* trunksd process (TPM daemon) */
#define AID_NVRAM         1050  /* Access-controlled NVRAM */
#define AID_DNS           1051  /* DNS resolution daemon (system: netd) */
#define AID_DNS_TETHER    1052  /* DNS resolution daemon (tether: dnsmasq) */
/* Changes to this file must be made in AOSP, *not* in internal branches. */

#define AID_SHELL         2000  /* adb and debug shell user */
#define AID_CACHE         2001  /* cache access */
#define AID_DIAG          2002  /* access to diagnostic resources */

/* The range 2900-2999 is reserved for OEM, and must never be
 * used here */
#define AID_OEM_RESERVED_START 2900
#define AID_OEM_RESERVED_END   2999

/* The 3000 series are intended for use as supplemental group id's only.
 * They indicate special Android capabilities that the kernel is aware of. */
#define AID_NET_BT_ADMIN  3001  /* bluetooth: create any socket */
#define AID_NET_BT        3002  /* bluetooth: create sco, rfcomm or l2cap sockets */
#define AID_INET          3003  /* can create AF_INET and AF_INET6 sockets */
#define AID_NET_RAW       3004  /* can create raw INET sockets */
#define AID_NET_ADMIN     3005  /* can configure interfaces and routing tables. */
#define AID_NET_BW_STATS  3006  /* read bandwidth statistics */
#define AID_NET_BW_ACCT   3007  /* change bandwidth statistics accounting */
#define AID_NET_BT_STACK  3008  /* bluetooth: access config files */
#define AID_READPROC      3009  /* Allow /proc read access */
#define AID_WAKELOCK      3010  /* Allow system wakelock read/write access */
#define AID_QCOM_DIAG     3011  /* can read/write /dev/diag */
#define AID_IMS           3012  /* can read/write /dev/socket/imsrtp */

/* The range 5000-5999 is also reserved for OEM, and must never be used here. */
#define AID_OEM_RESERVED_2_START 5000
#define AID_OEM_RESERVED_2_END   5999

#define AID_MOT_ACCY      9000  /* access to accessory */
#define AID_MOT_PWRIC     9001  /* power IC */
#define AID_MOT_USB       9002  /* mot usb */
#define AID_MOT_DRM       9003  /* can access DRM resource. */
#define AID_MOT_TCMD      9004  /* mot_tcmd */
#define AID_MOT_SEC_RTC   9005  /* mot cpcap rtc */
#define AID_MOT_TOMBSTONE 9006
#define AID_MOT_TPAPI     9007  /* mot_tpapi */
#define AID_MOT_SECCLKD   9008  /* mot_secclkd */
#define AID_MOT_WHISPER   9009  /* Whisper Protocol access */
#define AID_MOT_CAIF      9010  /* can create CAIF sockets */
#define AID_MOT_DLNA      9011  /*DLNA native */

#define AID_EVERYBODY     9997  /* shared between all apps in the same profile */
#define AID_MISC          9998  /* access to misc storage */
#define AID_NOBODY        9999

#define AID_APP          10000  /* first app user */

#define AID_ISOLATED_START 99000 /* start of uids for fully isolated sandboxed processes */
#define AID_ISOLATED_END   99999 /* end of uids for fully isolated sandboxed processes */

#define AID_USER        100000  /* offset for uid ranges for each user */

#define AID_SHARED_GID_START 50000 /* start of gids for apps in each user to share */
#define AID_SHARED_GID_END   59999 /* start of gids for apps in each user to share */

#if !defined(EXCLUDE_FS_CONFIG_STRUCTURES)
/*
 * Used in:
 *  bionic/libc/bionic/stubs.cpp
 *  external/libselinux/src/android.c
 *  system/core/logd/LogStatistics.cpp
 *  system/core/init/ueventd.cpp
 *  system/core/init/util.cpp
 */
struct android_id_info {
    const char *name;
    unsigned aid;
};

static const struct android_id_info android_ids[] = {
    { "root",          AID_ROOT, },

    { "system",        AID_SYSTEM, },

    { "radio",         AID_RADIO, },
    { "bluetooth",     AID_BLUETOOTH, },
    { "graphics",      AID_GRAPHICS, },
    { "input",         AID_INPUT, },
    { "audio",         AID_AUDIO, },
    { "camera",        AID_CAMERA, },
    { "log",           AID_LOG, },
    { "compass",       AID_COMPASS, },
    { "mount",         AID_MOUNT, },
    { "wifi",          AID_WIFI, },
    { "adb",           AID_ADB, },
    { "install",       AID_INSTALL, },
    { "media",         AID_MEDIA, },
    { "dhcp",          AID_DHCP, },
    { "sdcard_rw",     AID_SDCARD_RW, },
    { "vpn",           AID_VPN, },
    { "keystore",      AID_KEYSTORE, },
    { "usb",           AID_USB, },
    { "drm",           AID_DRM, },
    { "mdnsr",         AID_MDNSR, },
    { "gps",           AID_GPS, },
    // AID_UNUSED1
    { "media_rw",      AID_MEDIA_RW, },
    { "mtp",           AID_MTP, },
    // AID_UNUSED2
    { "drmrpc",        AID_DRMRPC, },
    { "nfc",           AID_NFC, },
    { "sdcard_r",      AID_SDCARD_R, },
    { "clat",          AID_CLAT, },
    { "loop_radio",    AID_LOOP_RADIO, },
    { "mediadrm",      AID_MEDIA_DRM, },
    { "package_info",  AID_PACKAGE_INFO, },
    { "sdcard_pics",   AID_SDCARD_PICS, },
    { "sdcard_av",     AID_SDCARD_AV, },
    { "sdcard_all",    AID_SDCARD_ALL, },
    { "logd",          AID_LOGD, },
    { "shared_relro",  AID_SHARED_RELRO, },
    { "dbus",          AID_DBUS, },
    { "tlsdate",       AID_TLSDATE, },
    { "mediaex",       AID_MEDIA_EX, },
    { "audioserver",   AID_AUDIOSERVER, },
    { "metrics_coll",  AID_METRICS_COLL },
    { "metricsd",      AID_METRICSD },
    { "webserv",       AID_WEBSERV },
    { "debuggerd",     AID_DEBUGGERD, },
    { "mediacodec",    AID_MEDIA_CODEC, },
    { "cameraserver",  AID_CAMERASERVER, },
    { "firewall",      AID_FIREWALL, },
    { "trunks",        AID_TRUNKS, },
    { "nvram",         AID_NVRAM, },
    { "dns",           AID_DNS, },
    { "dns_tether",    AID_DNS_TETHER, },

    { "shell",         AID_SHELL, },
    { "cache",         AID_CACHE, },
    { "diag",          AID_DIAG, },

    { "net_bt_admin",  AID_NET_BT_ADMIN, },
    { "net_bt",        AID_NET_BT, },
    { "inet",          AID_INET, },
    { "net_raw",       AID_NET_RAW, },
    { "net_admin",     AID_NET_ADMIN, },
    { "net_bw_stats",  AID_NET_BW_STATS, },
    { "ims", AID_IMS, },
    { "net_bw_acct",   AID_NET_BW_ACCT, },
    { "net_bt_stack",  AID_NET_BT_STACK, },
    { "readproc",      AID_READPROC, },
    { "wakelock",      AID_WAKELOCK, },

    { "everybody",     AID_EVERYBODY, },
    { "misc",          AID_MISC, },
    { "nobody",        AID_NOBODY, },

    { "qcom_diag", AID_QCOM_DIAG, },

    { "mot_accy",	AID_MOT_ACCY, },
    { "mot_pwric",	AID_MOT_PWRIC, },
    { "mot_usb",	AID_MOT_USB, },
    { "mot_drm",	AID_MOT_DRM, },
    { "mot_tcmd",	AID_MOT_TCMD, },
    { "mot_sec_rtc",	AID_MOT_SEC_RTC, },
    { "mot_tombstone",	AID_MOT_TOMBSTONE, },
    { "mot_tpapi",	AID_MOT_TPAPI, },
    { "mot_secclkd",	AID_MOT_SECCLKD, },
    { "mot_whisper",	AID_MOT_WHISPER, },
    { "mot_caif",	AID_MOT_CAIF, },
    { "mot_dlna",	AID_MOT_DLNA, },
};

#define android_id_count \
    (sizeof(android_ids) / sizeof(android_ids[0]))

struct fs_path_config {
    unsigned mode;
    unsigned uid;
    unsigned gid;
    uint64_t capabilities;
    const char *prefix;
};

/* Rules for directories and files has moved to system/code/libcutils/fs_config.c */

__BEGIN_DECLS

/*
 * Used in:
 *  build/tools/fs_config/fs_config.c
 *  build/tools/fs_get_stats/fs_get_stats.c
 *  system/extras/ext4_utils/make_ext4fs_main.c
 *  external/squashfs-tools/squashfs-tools/android.c
 *  system/core/cpio/mkbootfs.c
 *  system/core/adb/file_sync_service.cpp
 *  system/extras/ext4_utils/canned_fs_config.c
 */
void fs_config(const char *path, int dir, const char *target_out_path,
               unsigned *uid, unsigned *gid, unsigned *mode, uint64_t *capabilities);

ssize_t fs_config_generate(char *buffer, size_t length, const struct fs_path_config *pc);

__END_DECLS

#endif
#endif
