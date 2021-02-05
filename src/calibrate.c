/****************************************************************************
** abstouch-nux - An absolute touchpad input client for GNU/Linux.
** Copyright (C) 2021  acedron <acedrons@yahoo.co.jp>
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <https://www.gnu.org/licenses/>.
****************************************************************************/
#define _GNU_SOURCE
#include "calibrate.h"
#include "str.h"

#include <stdio.h>
#include <stdlib.h>

#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define DEV_INPUT_DIR "/dev/input"
#define EVENT_PREFIX "event"
#define EVENT_CONF_PATH "/usr/local/share/abstouch-nux/event.conf"
#define ENAME_CONF_PATH "/usr/local/share/abstouch-nux/ename.conf"
#define XOFF_CONF_PATH "/usr/local/share/abstouch-nux/xoff.conf"
#define YOFF_CONF_PATH "/usr/local/share/abstouch-nux/yoff.conf"

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)
#define NAME_ELEMENT(element) [element] = #element

static volatile sig_atomic_t stop = 0;
static void interrupt_handler(int sig)
{
    stop = 1;
}

static const char* const events[EV_MAX + 1] = {
    [0 ... EV_MAX] = NULL,
    NAME_ELEMENT(EV_KEY),
    NAME_ELEMENT(EV_ABS)
};

static const int maxval[EV_MAX + 1] = {
    [0 ... EV_MAX] = -1,
    [EV_KEY] = KEY_MAX,
    [EV_ABS] = ABS_MAX
};

static const char * const absolutes[ABS_MAX + 1] = {
    [0 ... ABS_MAX] = NULL,
    NAME_ELEMENT(ABS_X),            NAME_ELEMENT(ABS_Y),
    NAME_ELEMENT(ABS_Z),            NAME_ELEMENT(ABS_RX),
    NAME_ELEMENT(ABS_RY),           NAME_ELEMENT(ABS_RZ),
    NAME_ELEMENT(ABS_THROTTLE),     NAME_ELEMENT(ABS_RUDDER),
    NAME_ELEMENT(ABS_WHEEL),        NAME_ELEMENT(ABS_GAS),
    NAME_ELEMENT(ABS_BRAKE),        NAME_ELEMENT(ABS_HAT0X),
    NAME_ELEMENT(ABS_HAT0Y),        NAME_ELEMENT(ABS_HAT1X),
    NAME_ELEMENT(ABS_HAT1Y),        NAME_ELEMENT(ABS_HAT2X),
    NAME_ELEMENT(ABS_HAT2Y),        NAME_ELEMENT(ABS_HAT3X),
    NAME_ELEMENT(ABS_HAT3Y),        NAME_ELEMENT(ABS_PRESSURE),
    NAME_ELEMENT(ABS_DISTANCE),     NAME_ELEMENT(ABS_TILT_X),
    NAME_ELEMENT(ABS_TILT_Y),       NAME_ELEMENT(ABS_TOOL_WIDTH),
    NAME_ELEMENT(ABS_VOLUME),       NAME_ELEMENT(ABS_MISC),
#ifdef ABS_MT_BLOB_ID
    NAME_ELEMENT(ABS_MT_TOUCH_MAJOR),
    NAME_ELEMENT(ABS_MT_TOUCH_MINOR),
    NAME_ELEMENT(ABS_MT_WIDTH_MAJOR),
    NAME_ELEMENT(ABS_MT_WIDTH_MINOR),
    NAME_ELEMENT(ABS_MT_ORIENTATION),
    NAME_ELEMENT(ABS_MT_POSITION_X),
    NAME_ELEMENT(ABS_MT_POSITION_Y),
    NAME_ELEMENT(ABS_MT_TOOL_TYPE),
    NAME_ELEMENT(ABS_MT_BLOB_ID),
#endif
#ifdef ABS_MT_TRACKING_ID
    NAME_ELEMENT(ABS_MT_TRACKING_ID),
#endif
#ifdef ABS_MT_PRESSURE
    NAME_ELEMENT(ABS_MT_PRESSURE),
#endif
#ifdef ABS_MT_SLOT
    NAME_ELEMENT(ABS_MT_SLOT),
#endif
#ifdef ABS_MT_TOOL_X
    NAME_ELEMENT(ABS_MT_TOOL_X),
    NAME_ELEMENT(ABS_MT_TOOL_Y),
    NAME_ELEMENT(ABS_MT_DISTANCE)
#endif
};

static const char * const keys[KEY_MAX + 1] = {
    [0 ... KEY_MAX] = NULL,
    NAME_ELEMENT(BTN_0),                NAME_ELEMENT(BTN_1),
    NAME_ELEMENT(BTN_2),                NAME_ELEMENT(BTN_3),
    NAME_ELEMENT(BTN_4),                NAME_ELEMENT(BTN_5),
    NAME_ELEMENT(BTN_6),                NAME_ELEMENT(BTN_7),
    NAME_ELEMENT(BTN_8),                NAME_ELEMENT(BTN_9),
    NAME_ELEMENT(BTN_LEFT),             NAME_ELEMENT(BTN_RIGHT),
    NAME_ELEMENT(BTN_MIDDLE),           NAME_ELEMENT(BTN_SIDE),
    NAME_ELEMENT(BTN_EXTRA),            NAME_ELEMENT(BTN_FORWARD),
    NAME_ELEMENT(BTN_BACK),             NAME_ELEMENT(BTN_TASK),
    NAME_ELEMENT(BTN_TRIGGER),          NAME_ELEMENT(BTN_THUMB),
    NAME_ELEMENT(BTN_THUMB2),           NAME_ELEMENT(BTN_TOP),
    NAME_ELEMENT(BTN_TOP2),             NAME_ELEMENT(BTN_PINKIE),
    NAME_ELEMENT(BTN_BASE),             NAME_ELEMENT(BTN_BASE2),
    NAME_ELEMENT(BTN_BASE3),            NAME_ELEMENT(BTN_BASE4),
    NAME_ELEMENT(BTN_BASE5),            NAME_ELEMENT(BTN_BASE6),
    NAME_ELEMENT(BTN_DEAD),             NAME_ELEMENT(BTN_C),

    NAME_ELEMENT(BTN_Z),                NAME_ELEMENT(BTN_TL),
    NAME_ELEMENT(BTN_TR),               NAME_ELEMENT(BTN_TL2),
    NAME_ELEMENT(BTN_TR2),              NAME_ELEMENT(BTN_SELECT),
    NAME_ELEMENT(BTN_START),            NAME_ELEMENT(BTN_MODE),
    NAME_ELEMENT(BTN_THUMBL),           NAME_ELEMENT(BTN_THUMBR),
    NAME_ELEMENT(BTN_TOOL_PEN),         NAME_ELEMENT(BTN_TOOL_RUBBER),
    NAME_ELEMENT(BTN_TOOL_BRUSH),       NAME_ELEMENT(BTN_TOOL_PENCIL),
    NAME_ELEMENT(BTN_TOOL_AIRBRUSH),    NAME_ELEMENT(BTN_TOOL_FINGER),
    NAME_ELEMENT(BTN_TOOL_MOUSE),       NAME_ELEMENT(BTN_TOOL_LENS),
    NAME_ELEMENT(BTN_TOUCH),            NAME_ELEMENT(BTN_STYLUS),
    NAME_ELEMENT(BTN_STYLUS2),          NAME_ELEMENT(BTN_TOOL_DOUBLETAP),
    NAME_ELEMENT(BTN_TOOL_TRIPLETAP)
};

static const char * const * const names[EV_MAX + 1] = {
    [0 ... EV_MAX] = NULL,
    [EV_KEY] = keys,
    [EV_ABS] = absolutes
};

static inline const char* typename(unsigned int type)
{
    return (type <= EV_MAX && events[type]) ? events[type] : "?";
}

static inline const char* codename(unsigned int type, unsigned int code)
{
    return (type <= EV_MAX && code <= maxval[type] && names[type] && names[type][code]) ? names[type][code] : "?";
}

static int is_abs_device(int fd)
{
    unsigned int type;
    unsigned long bit[EV_MAX][NBITS(KEY_MAX)];

    memset(bit, 0, sizeof(bit));
    ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);

    for (type = 0; type < EV_MAX; type++) {
        if (test_bit(type, bit[0]) && type != EV_REP) {
            if (type == EV_ABS)
                return 1;
        }
    }

    return 0;
}

static int is_event_device(const struct dirent *dir)
{
    return !strncmp(EVENT_PREFIX, dir->d_name, 5);
}

static int get_event_by_name(char *ename)
{
    struct dirent **namelist;
    int i, ndev, devnum;
    char *filename;
    int max_device = 0;

    ndev = scandir(DEV_INPUT_DIR, &namelist, is_event_device, versionsort);
    if (ndev < 1)
        return -1;

    for (i = 0; i < ndev; i++) {
        char fname[64];
        int fd = -1;
        char name[256] = "\x1b[1;31mUnknown\x1b[;m";

        snprintf(fname, sizeof(fname),
            "%s/%s", DEV_INPUT_DIR, namelist[i]->d_name);
        fd = open(fname, O_RDONLY);
        if (fd < 0)
            continue;
        ioctl(fd, EVIOCGNAME(sizeof(name)), name);
        close(fd);
        free(namelist[i]);

        if (!strcmp(ename, name))
            return i;
    }
}

static void get_touch_limits(int fd, int *xoff, int *yoff)
{
    struct input_event ev[64];
    int i, rd;
    fd_set rdfs;

    FD_ZERO(&rdfs);
    FD_SET(fd, &rdfs);

    int input_xmin = 99999, input_xmax = -99999;
    int input_ymin = 99999, input_ymax = -99999;

    while (!stop) {
        select(fd + 1, &rdfs, NULL, NULL, NULL);
        if (stop)
            break;
        rd = read(fd, ev, sizeof(ev));

        if (rd < (int) sizeof(struct input_event)) {
            printf("\n \x1b[1;31m=> \x1b[;mError reading input!\n");
            printf(" \x1b[1;32m=> \x1b[;mExpected \x1b[1;37m%d\x1b[;m bytes, got \x1b[1;37m%d\n", (int) sizeof(struct input_event), rd);
            return;
        }

        int absx[6] = {0}, absy[6] = {0};
        ioctl(fd, EVIOCGABS(ABS_X), absx);
        ioctl(fd, EVIOCGABS(ABS_Y), absy);
        int xmin = absx[1];
        int xmax = absx[2];
        int ymin = absy[1];
        int ymax = absy[2];

        for (i = 0; i < rd / sizeof(struct input_event); i++) {
            unsigned int type, code;
            type = ev[i].type;
            code = ev[i].code;

            if (type == EV_ABS) {
                if (code == ABS_X) {
                    if (ev[i].value < input_xmin) input_xmin = ev[i].value;
                    if (ev[i].value > input_xmax) input_xmax = ev[i].value;
                } else if (code == ABS_Y) {
                    if (ev[i].value < input_ymin) input_ymin = ev[i].value;
                    if (ev[i].value > input_ymax) input_ymax = ev[i].value;
                }
            } else if (type == EV_KEY && (code == BTN_LEFT || code == BTN_RIGHT) && ev[i].value == 1) {
                *xoff = (input_xmin - xmin) + (input_xmax - xmax);
                *yoff = (input_ymin - ymin) + (input_ymax - ymax);
                stop = 1;
            }
        }
    }

    ioctl(fd, EVIOCGRAB, (void*)0);
}

int calibrate(int argc, char *argv[])
{
    int event = 0;
    char *p;

    for (int i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "-event", 6)) {
            char *eventstr = argv[i];
            shift_string(eventstr, 6);
            event = strtol(eventstr, &p, 10);
        }
    }

    int fd = -1;
    char fname[64];
    snprintf(fname, sizeof(fname), "%s/%s%d", DEV_INPUT_DIR, EVENT_PREFIX, event);
    fd = open(fname, O_RDONLY);

    if (!is_abs_device(fd)) {
        printf(" \x1b[1;31m=> \x1b[;mEvent \x1b[1;37m%d \x1b[;mhas no absolute input!\n", event);
        printf(" \x1b[1;32m=> \x1b[;mChecking for past events.\n");

        char *buffer;
        long length;
        FILE *fp = fopen(ENAME_CONF_PATH, "rb");

        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer)
            fread(buffer, 1, length, fp);
        fclose(fp);
        buffer[length] = '\0';

        if (!strcmp(buffer, "")) {
            printf(" \x1b[1;32m=> \x1b[;mNo past events found.\n");
            printf(" \x1b[1;32m=> \x1b[;mIf you are sure your device supports absolute input, try following:\n");
            printf(" \x1b[1;32m=> \x1b[1;37mabstouch setevent\n\x1b[;m");
            return EXIT_FAILURE;
        }

        int newevent = get_event_by_name(buffer);
        if (newevent == -1) {
            printf(" \x1b[1;32m=> \x1b[;mCouldn't get new event from old event.\n");
            printf(" \x1b[1;32m=> \x1b[;mIf you are sure your device supports absolute input, try following:\n");
            printf(" \x1b[1;32m=> \x1b[1;37mabstouch setevent\n\x1b[;m");
            return EXIT_FAILURE;
        }

        printf(" \x1b[1;32m=> \x1b[;mFound moved past event \x1b[1;37m%d\x1b[;m.\n", newevent);
        printf(" \x1b[1;32m=> \x1b[;mSetting event...\n");
        event = newevent;
        snprintf(fname, sizeof(fname),
             "%s/%s%d", DEV_INPUT_DIR, EVENT_PREFIX, event);
        fd = open(fname, O_RDONLY);
    }

    int xmin = 0, xmax = 0;
    int ymin = 0, ymax = 0;
    int inxmin = 0, inxmax = 0;
    int inymin = 0, inymax = 0;

    printf(" \x1b[1;32m=> \x1b[1;37mRub your finger over the edges and corners of\n");
    printf(" \x1b[1;32m=> \x1b[1;37myour touchpad for like 30 seconds or more and then\n");
    printf(" \x1b[1;32m=> \x1b[1;37mclose as possible to 0 and then left click on your touchpad.\n\n");

    int xoff = 0, yoff = 0;
    get_touch_limits(fd, &xoff, &yoff);

    printf(" \x1b[1;31m=> \x1b[;mCouldn't set offset!\n");

    FILE *xfp = fopen(XOFF_CONF_PATH, "w");
    fprintf(xfp, "%d", xoff);
    fclose(xfp);
    FILE *yfp = fopen(YOFF_CONF_PATH, "w");
    fprintf(yfp, "%d", yoff);
    fclose(yfp);

    printf("\x1b[A\x1b[2K\x1b[1;32m= > \x1b[1;37mSuccessfully set offset as \x1b[;m%d\x1b[1;37mx\x1b[;m%d\x1b[1;37m.\x1b[;m\n",
        xoff, yoff);

    return EXIT_SUCCESS;
}
