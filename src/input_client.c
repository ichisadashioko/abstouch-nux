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
#include "input_client.h"
#include "event.h"
#include "str_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>

#include <linux/input.h>
#include <X11/Xlib.h>

#define EVENT_CONF_PATH "/usr/local/share/abstouch-nux/event.conf"
#define ENAME_CONF_PATH "/usr/local/share/abstouch-nux/ename.conf"

static volatile sig_atomic_t stop = 0;
static void interrupt_handler(int sig)
{
    stop = 1;
}

static Display *dpy;
static Window root_window;
static XWindowAttributes xw_attrs;

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

    ioctl(fd, EVIOCGRAB, (void *)0);
    return EXIT_SUCCESS;
}

int input_client(char *eventStr, char *xoffStr, char *yoffStr, char *displayStr,
    int screenId, int verbose, int daemon)
{
    int event = 0;
    int xoff = 0, yoff = 0;
    char *p;

    event = strtol(eventStr, &p, 10);
    xoff = strtol(xoffStr, &p, 10);
    yoff = strtol(yoffStr, &p, 10);

    char *dpystrBuffer = str_replace(displayStr, "\n", "");
    char *displayName = malloc(sizeof(char *));
    snprintf(displayName, sizeof(displayName), "%s", dpystrBuffer);
    free(dpystrBuffer);

    if (daemon) {
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
        free(buffer);

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

    free(displayName);
    return input_to_display(fd, verbose, xoff, yoff);
}
