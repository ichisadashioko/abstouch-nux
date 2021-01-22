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
#include "set_event.h"

#include <stdio.h>

#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#define DEV_INPUT_DIR "/dev/input"
#define EVENT_PREFIX "event"
#define EVENT_CONF_PATH "/usr/local/share/abstouch-nux/event.conf"
#define ENAME_CONF_PATH "/usr/local/share/abstouch-nux/ename.conf"

static int is_event_device(const struct dirent *dir)
{
    return strncmp(EVENT_PREFIX, dir->d_name, 5) == 0;
}

static char device_names[256][257];

static int scan_devices(void)
{
    struct dirent **namelist;
    int i, ndev, devnum;
    char *filename;
    int max_device = 0;

    ndev = scandir(DEV_INPUT_DIR, &namelist, is_event_device, versionsort);
    if (ndev < 1)
        return -1;

    printf("\x1b[1;32m => \x1b[1;37mEvents:\n");
    for (i = 0; i < ndev; i++)
    {
        char fname[64];
        int fd = -1;
        char name[256] = "\x1b[1;31mUnknown\x1b[;m";

        snprintf(fname, sizeof(fname),
            "%s/%s", DEV_INPUT_DIR, namelist[i]->d_name);
        fd = open(fname, O_RDONLY);
        if (fd < 0)
            continue;
        ioctl(fd, EVIOCGNAME(sizeof(name)), name);

        printf("\x1b[1;32m   => \x1b[1;37m%s \x1b[1;32m=> \x1b[;m%s\n", fname, name);
        strcpy(device_names[i], name);
        close(fd);

        sscanf(namelist[i]->d_name, "event%d", &devnum);
        if (devnum > max_device)
            max_device = devnum;

        free(namelist[i]);
    }

    return max_device;
}

int set_event(int argc, char *argv[])
{
    int event;
    int max_input = scan_devices();

    if (max_input < 0)
    {
        printf("\x1b[1;31m => \x1b[;mNo event found!\n");
        printf("\x1b[1;32m => \x1b[;mTry running as root.\n");
        return EXIT_FAILURE;
    }

    printf("\x1b[1;32m => \x1b[1;37mEnter your event's id\x1b[1;32m (\x1b[;m0\x1b[1;32m-\x1b[;m%d\x1b[1;32m): \x1b[;m", max_input);
    char *p, s[100];
    while (fgets(s, sizeof(s), stdin))
    {
        event = strtol(s, &p, 10);
        if ((p == s || *p != '\n') || (event < 0 || event > max_input))
            printf("\x1b[A\x1b[2K\x1b[1;32m => \x1b[1;37mEnter your event's id\x1b[1;32m (\x1b[;m0\x1b[1;32m-\x1b[;m%d\x1b[1;32m): \x1b[;m", max_input);
        else
            break;
    }

    printf("\x1b[1;31m => \x1b[;mCouldn't set event!\n");

    FILE *fp = fopen(EVENT_CONF_PATH, "w");
    fprintf(fp, "%d", event);
    fclose(fp);

    fp = fopen(ENAME_CONF_PATH, "w");
    fprintf(fp, "%s", device_names[event]);
    fclose(fp);

    printf("\x1b[A\x1b[2K\x1b[1;32m => \x1b[;mSuccessfully set event id as \x1b[1;37m%d\x1b[;m.\n", event);
    return EXIT_SUCCESS;
}
