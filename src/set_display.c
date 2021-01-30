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

#include "set_display.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <X11/Xlib.h>

#define DISPLAY_CONF_PATH "/usr/local/share/abstouch-nux/display.conf"
#define SCREEN_CONF_PATH "/usr/local/share/abstouch-nux/screen.conf"

/*
 * Get display and screen id and write to configuration.
 */
int set_display()
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
                char *nameBuffer = malloc(sizeof(char *));
                snprintf(nameBuffer, sizeof(nameBuffer), "%s", display_name);
                displays = (char **) realloc(displays, (displays_size + 1) * sizeof(char *));
                displays[displays_size] = nameBuffer;
                displays_size += 1;

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

        printf(" \x1b[1;31m=> \x1b[;mCouldn't set display!\n");

        FILE *dpyfp = fopen(DISPLAY_CONF_PATH, "w");
        fprintf(dpyfp, "%s", displayName);
        fclose(dpyfp);
        FILE *scrfp = fopen(SCREEN_CONF_PATH, "w");
        fprintf(scrfp, "%d", screenId);
        fclose(scrfp);

        printf("\x1b[A\x1b[2K \x1b[1;32m=> \x1b[;mSuccessfully set display and screen as \x1b[1;37m%s\x1b[;m.\x1b[1;37m%d\x1b[;m.\n", displayName, screenId);
        return EXIT_SUCCESS;
    } else {
        printf(" \x1b[1;31m=> \x1b[;mCouldn't get displays!\n");
        printf(" \x1b[1;32m=> \x1b[;mTry running as root.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
