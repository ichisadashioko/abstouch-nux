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
#include "input.h"

#include <stdio.h>
#include <stdlib.h>

#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>

#include <X11/Xlib.h>

#define DEV_INPUT_DIR "/dev/input"
#define EVENT_PREFIX "event"
#define EVENT_CONF_PATH "/usr/local/share/abstouch-nux/event.conf"
#define ENAME_CONF_PATH "/usr/local/share/abstouch-nux/ename.conf"

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)
#define NAME_ELEMENT(element) [element] = #element

static void logstr(char *str, int verbose)
{
    if (verbose)
        printf("   \x1b[1;32m- \x1b[;m%s\x1b[;m\n", str);
}

static void shift_string(char *str, size_t n)
{
    size_t len = strlen(str);
    if (n > len)
        return;
    memmove(str, str + n, len - n + 1);
}

static char *str_replace(char *orig, char *rep, char *with)
{
    char *result;
    char *ins;
    char *tmp;
    int len_rep;
    int len_with;
    int len_front;
    int count;

    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL;
    if (!with)
        with = "";
    len_with = strlen(with);

    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count)
        ins = tmp + len_rep;

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep;
    }
    strcpy(tmp, orig);
    return result;
}

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

static Display *dpy;
static Window root_window;
static XWindowAttributes xw_attrs;

static inline const char* typename(unsigned int type)
{
    return (type <= EV_MAX && events[type]) ? events[type] : "?";
}

static inline const char* codename(unsigned int type, unsigned int code)
{
    return (type <= EV_MAX && code <= maxval[type] && names[type] && names[type][code]) ? names[type][code] : "?";
}

static void map_input_to_display(int *xval, int xmin, int xmax,
    int *yval, int ymin, int ymax)
{
    *xval = (*xval - xmin) / ((xmax - xmin) / xw_attrs.width);
    *yval = (*yval - ymin) / ((ymax - ymin) / xw_attrs.height);
}

static void apply_cursor_position(int x, int y)
{
    XSelectInput(dpy, root_window, KeyReleaseMask);
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, x, y);
    XFlush(dpy);
}

static int input_to_display(int fd, int verbose, int xoff, int yoff)
{
    struct input_event ev[64];
    int i, rd;
    fd_set rdfs;

    FD_ZERO(&rdfs);
    FD_SET(fd, &rdfs);

    while (!stop) {
        select(fd + 1, &rdfs, NULL, NULL, NULL);
        if (stop)
            break;
        rd = read(fd, ev, sizeof(ev));

        if (rd < (int) sizeof(struct input_event)) {
            printf("\n \x1b[1;31m=> \x1b[;mError reading input!\n");
            printf(" \x1b[1;32m=> \x1b[;mExpected \x1b[1;37m%d\x1b[;m bytes, got \x1b[1;37m%d\n", (int) sizeof(struct input_event), rd);
            return EXIT_FAILURE;
        }

        int xmin, xmax, ymin, ymax;

        int absx[6] = {0}, absy[6] = {0};
        ioctl(fd, EVIOCGABS(ABS_X), absx);
        ioctl(fd, EVIOCGABS(ABS_Y), absy);
        xmin = absx[1];
        xmax = absx[2];
        ymin = absy[1];
        ymax = absy[2];

        for (i = 0; i < rd / sizeof(struct input_event); i++) {
            unsigned int type, code;
            type = ev[i].type;
            code = ev[i].code;


            if (type != EV_ABS)
                continue;

            int xval, yval;
            if (code == ABS_X)
                xval = ev[i].value;
            else if (code == ABS_Y)
                yval = ev[i].value;

            int cx = xval, cy = yval;
            map_input_to_display(&cx, xmin, xmax, &cy, ymin, ymax);
            cx += xoff;
            cy += yoff;
            apply_cursor_position(cx, cy);
            if (verbose)
                printf("\x1b[A\x1b[K   \x1b[1;32m- \x1b[;mMoved cursor to \x1b[1;37m%d\x1b[1;32mx\x1b[1;37m%d\x1b[;m.\n", cx, cy);
        }
    }

    ioctl(fd, EVIOCGRAB, (void*)0);
    return EXIT_SUCCESS;
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

        if (strcmp(ename, name) == 0)
            return i;
    }

    return -1;
}

int input(int argc, char *argv[])
{
    int verbose = 0, event = 0;
    int xoff = 0, yoff = 0;
    char *p;

    char *displayName = malloc(sizeof(char *));
    int screenId = 0;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v"))
            verbose = 1;

        if (!strncmp(argv[i], "-event", 6)) {
            char *eventstr = argv[i];
            shift_string(eventstr, 6);
            event = strtol(eventstr, &p, 10);
        }

        if (!strncmp(argv[i], "-xoff", 5)) {
            char *xoffstr = argv[i];
            shift_string(xoffstr, 5);
            xoff = strtol(xoffstr, &p, 10);
        }

        if (!strncmp(argv[i], "-yoff", 5)) {
            char *yoffstr = argv[i];
            shift_string(yoffstr, 5);
            yoff = strtol(yoffstr, &p, 10);
        }

        if (!strncmp(argv[i], "-display", 8)) {
            char *dpystr = argv[i];
            shift_string(dpystr, 8);
            char *dpystrBuffer = str_replace(dpystr, "\n", "");
            snprintf(displayName, sizeof(displayName), "%s", dpystrBuffer);
        }

        if (!strncmp(argv[i], "-screen", 7)) {
            char *scrstr = argv[i];
            shift_string(scrstr, 7);
            screenId = strtol(scrstr, &p, 10);
        }

        if (!strcmp(argv[i], "-d")) {
            pid_t childPid;
            childPid = fork();
            if (childPid >= 0) {
                if (childPid != 0)
                    return EXIT_SUCCESS;
            } else {
                printf(" \x1b[1;31m=> \x1b[;mCouldn't start abstouch-nux daemon!");
                return EXIT_FAILURE;
            }
        }
    }

    int fd = -1;
    char fname[64];
    snprintf(fname, sizeof(fname),
         "%s/%s%d", DEV_INPUT_DIR, EVENT_PREFIX, event);
    fd = open(fname, O_RDONLY);

    logstr("Enabled verbose output.", verbose);

    if (!is_abs_device(fd)) {
        if (verbose) {
            printf(" \x1b[1;31m=> \x1b[;mEvent \x1b[1;37m%d \x1b[;mhas no absolute input!\n", event);
            printf(" \x1b[1;32m=> \x1b[;mChecking for past events.\n");
        }

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

        if (strcmp(buffer, "") == 0) {
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

        if (verbose) {
            printf(" \x1b[1;32m=> \x1b[;mFound moved past event \x1b[1;37m%d\x1b[;m.\n", newevent);
            printf(" \x1b[1;32m=> \x1b[;mSetting event...\n");
        }

        event = newevent;
        snprintf(fname, sizeof(fname),
             "%s/%s%d", DEV_INPUT_DIR, EVENT_PREFIX, event);
        fd = open(fname, O_RDONLY);

        if (!is_abs_device(fd)) {
            printf(" \x1b[1;31m=> \x1b[;mPast event doesn't have absolute input!\n");
            printf(" \x1b[1;32m=> \x1b[;mIf you are sure your device supports absolute input, try following:\n");
            printf(" \x1b[1;32m=> \x1b[1;37mabstouch setevent\n\x1b[;m");
            return EXIT_FAILURE;
        }

        fp = fopen(EVENT_CONF_PATH, "w");
        fprintf(fp, "%d", event);
        fclose(fp);
    }

    if (verbose) {
      printf("   \x1b[1;32m- \x1b[;mFound absolute input on event \x1b[1;37m%d\x1b[;m.\n", event);
      printf("   \x1b[1;32m- \x1b[;mBound to display \x1b[1;37m%s\x1b[;m.\x1b[1;37m%d\x1b[;m.\n", displayName, screenId);
      printf("   \x1b[1;32m- \x1b[;mSet offset to \x1b[1;37m%d\x1b[1;32mx\x1b[1;37m%d\x1b[;m.\n", xoff, yoff);
      printf("   \x1b[1;32m- \x1b[;mWaiting for input...\n");
    }

    dpy = XOpenDisplay(displayName);
    root_window = XRootWindow(dpy, screenId);
    XGetWindowAttributes(dpy, root_window, &xw_attrs);

    return input_to_display(fd, verbose, xoff, yoff);
}
