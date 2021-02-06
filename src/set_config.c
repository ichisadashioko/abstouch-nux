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
#include "set_config.h"
#include "event.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <X11/Xlib.h>

#define XOFF_CONF_PATH "/usr/local/share/abstouch-nux/xoff.conf"
#define YOFF_CONF_PATH "/usr/local/share/abstouch-nux/yoff.conf"
#define EVENT_CONF_PATH "/usr/local/share/abstouch-nux/event.conf"
#define ENAME_CONF_PATH "/usr/local/share/abstouch-nux/ename.conf"
#define DISPLAY_CONF_PATH "/usr/local/share/abstouch-nux/display.conf"
#define SCREEN_CONF_PATH "/usr/local/share/abstouch-nux/screen.conf"

/*
 * Get X and Y offset from user manually and write to configuration.
 */
int set_offset(void)
{
    int xoff, yoff;
    char *p, xoffs[100], yoffs[100];

    printf(" \x1b[1;32m=> \x1b[1;37mEnter new x offset: \x1b[;m");
    while (fgets(xoffs, sizeof(xoffs), stdin)) {
        xoff = strtol(xoffs, &p, 10);
        if (p == xoffs || *p != '\n')
            printf("\x1b[A\x1b[2K \x1b[1;32m=> \x1b[1;37mEnter new x offset: \x1b[;m");
        else
            break;
    }

    printf(" \x1b[1;32m=> \x1b[1;37mEnter new y offset: \x1b[;m");
    while (fgets(yoffs, sizeof(yoffs), stdin)) {
        yoff = strtol(yoffs, &p, 10);
        if (p == yoffs || *p != '\n')
            printf("\x1b[A\x1b[2K \x1b[1;32m=> \x1b[1;37mEnter new y offset: \x1b[;m");
        else
            break;
    }

    FILE *fp = fopen(XOFF_CONF_PATH, "w");
    fprintf(fp, "%d", xoff);
    fclose(fp);
    fp = fopen(YOFF_CONF_PATH, "w");
    fprintf(fp, "%d", yoff);
    fclose(fp);

    printf(" \x1b[1;32m=> \x1b[;mSuccessfully set offset as \x1b[1;37m%d \x1b[;mand \x1b[1;37m%d\x1b[;m.\n", xoff, yoff);
    return EXIT_SUCCESS;
}

static char device_names[256][257];

/*
 * Scans for devices and returns the max device count.
 * Writes the device names to `device_names` array above.
 */
static int scan_devices(void)
{
    struct dirent **namelist;
    int i, ndev, devnum;
    char *filename;
    int max_device = 0;

    ndev = scandir(DEV_INPUT_DIR, &namelist, is_event_device, versionsort);
    if (ndev < 1)
        return -1;

    printf(" \x1b[1;32m=> \x1b[1;37mEvents:\n");
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

        printf("   \x1b[1;32m=> \x1b[1;37m%s \x1b[1;32m=> \x1b[;m%s\n", fname, name);
        strcpy(device_names[i], name);
        close(fd);

        sscanf(namelist[i]->d_name, "event%d", &devnum);
        if (devnum > max_device)
            max_device = devnum;

        free(namelist[i]);
    }

    return max_device;
}

/*
 * Get input event from user and write to configuration.
 */
int set_event(void)
{
    int event;
    int max_input = scan_devices();

    if (max_input < 0) {
        printf(" \x1b[1;31m=> \x1b[;mNo event found!\n");
        printf(" \x1b[1;32m=> \x1b[;mTry running as root.\n");
        return EXIT_FAILURE;
    }

    printf(" \x1b[1;32m=> \x1b[1;37mEnter your event's id\x1b[1;32m (\x1b[;m0\x1b[1;32m-\x1b[;m%d\x1b[1;32m): \x1b[;m", max_input);
    char *p, s[100];
    while (fgets(s, sizeof(s), stdin)) {
        event = strtol(s, &p, 10);
        if ((p == s || *p != '\n') || (event < 0 || event > max_input))
            printf("\x1b[A\x1b[2K \x1b[1;32m=> \x1b[1;37mEnter your event's id\x1b[1;32m (\x1b[;m0\x1b[1;32m-\x1b[;m%d\x1b[1;32m): \x1b[;m", max_input);
        else
            break;
    }

    FILE *fp = fopen(EVENT_CONF_PATH, "w");
    fprintf(fp, "%d", event);
    fclose(fp);
    fp = fopen(ENAME_CONF_PATH, "w");
    fprintf(fp, "%s", device_names[event]);
    fclose(fp);

    printf(" \x1b[1;32m=> \x1b[;mSuccessfully set event id as \x1b[1;37m%d\x1b[;m.\n", event);
    return EXIT_SUCCESS;
}

/*
 * Get display and screen id from user and write to configuration.
 */
int set_display(void)
{
    DIR *d = opendir("/tmp/.X11-unix");
    char **displays = (char **) malloc(0 * sizeof(char *));
    size_t displays_size = 0;
    int *screenCounts = (int *) malloc(0 * sizeof(int));
    size_t screenCounts_size = 0;

    if (d != NULL) {
        struct dirent *dr;
        while ((dr = readdir(d)) != NULL) {
            if (dr->d_name[0] != 'X')
                continue;

            char display_name[64] = ":";
            strcat(display_name, dr->d_name + 1);

            Display *dp = XOpenDisplay(display_name);
            if (dp != NULL) {
                char *nameBuffer = malloc(sizeof(display_name) / sizeof(char));
                snprintf(nameBuffer, sizeof(nameBuffer), "%s", display_name);
                displays = (char **) realloc(displays, (displays_size + 1) * sizeof(char *));
                displays[displays_size] = nameBuffer;
                displays_size += 1;
                free(nameBuffer);

                printf("   \x1b[1;32m=> \x1b[1;37mDisplay %s\n", display_name);
                int count = XScreenCount(dp);
                screenCounts = (int *) realloc(screenCounts, (screenCounts_size + 1) * sizeof(int));
                screenCounts[screenCounts_size] = count;
                screenCounts_size += 1;

                for (int i = 0; i < count; i++)
                    printf("     \x1b[1;32m=> \x1b[1;37mScreen %d \x1b[1;32m=> \x1b[1;37m%d\x1b[1;32mx\x1b[1;37m%d\n",
                        i, XDisplayWidth(dp, i), XDisplayHeight(dp, i));
                XCloseDisplay(dp);
            }
        }
        closedir(d);
        if (displays_size < 1) {
            printf(" \x1b[1;31m=> \x1b[;mNo available display found!\n");
            printf(" \x1b[1;32m=> \x1b[;mTry running as root.\n");
            return EXIT_FAILURE;
        }

        char displayName[256];
        int displayId, screenId;
        char *p, s[256];

        printf(" \x1b[1;32m=> \x1b[1;37mEnter your display's name: \x1b[;m:");
        while (fgets(s, sizeof(s), stdin)) {
            displayId = strtol(s, &p, 10);
            if (p == s || *p != '\n')
                printf("\x1b[A\x1b[2K \x1b[1;32m=> \x1b[1;37mEnter your display's name: \x1b[;m:");
            else {
                snprintf(displayName, (sizeof(displayName) / sizeof(char)), ":%d", displayId);
                int available = 0;
                for (size_t i = 0; i < displays_size; i++) {
                    if (!strcmp(displayName, displays[i])) {
                        available = 1;
                        break;
                    }
                }
                if (available)
                    break;
                else
                    printf("\x1b[A\x1b[2K \x1b[1;32m=> \x1b[1;37mEnter your display's name: \x1b[;m:");
            }
        }

        printf(" \x1b[1;32m=> \x1b[1;37mEnter your screen's id: \x1b[;m");
        while (fgets(s, sizeof(s), stdin)) {
            screenId = strtol(s, &p, 10);
            if (p == s || *p != '\n')
                printf("\x1b[A\x1b[2K \x1b[1;32m=> \x1b[1;37mEnter your screen's id: \x1b[;m");
            else {
                if (screenCounts[displayId] > screenId)
                    break;
                else
                    printf("\x1b[A\x1b[2K \x1b[1;32m=> \x1b[1;37mEnter your screen's id: \x1b[;m");
            }
        }

        FILE *fp = fopen(DISPLAY_CONF_PATH, "w");
        fprintf(fp, "%s", displayName);
        fclose(fp);
        fp = fopen(SCREEN_CONF_PATH, "w");
        fprintf(fp, "%d", screenId);
        fclose(fp);

        printf(" \x1b[1;32m=> \x1b[;mSuccessfully set display and screen as \x1b[1;37m%s\x1b[;m.\x1b[1;37m%d\x1b[;m.\n", displayName, screenId);
        return EXIT_SUCCESS;
    } else {
        printf(" \x1b[1;31m=> \x1b[;mCouldn't get displays!\n");
        printf(" \x1b[1;32m=> \x1b[;mTry running as root.\n");
        return EXIT_FAILURE;
    }

    free(displays);
    free(screenCounts);
    return EXIT_SUCCESS;
}
