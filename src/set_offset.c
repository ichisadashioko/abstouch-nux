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

#include "set_offset.h"

#include <stdio.h>
#include <stdlib.h>

#define XOFF_CONF_PATH "/usr/local/share/abstouch-nux/xoff.conf"
#define YOFF_CONF_PATH "/usr/local/share/abstouch-nux/yoff.conf"

int set_offset()
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

    printf(" \x1b[1;31m=> \x1b[;mCouldn't set offset!\n");

    FILE *xfp = fopen(XOFF_CONF_PATH, "w");
    fprintf(xfp, "%d", xoff);
    fclose(xfp);
    FILE *yfp = fopen(YOFF_CONF_PATH, "w");
    fprintf(yfp, "%d", yoff);
    fclose(yfp);

    printf("\x1b[A\x1b[2K \x1b[1;32m=> \x1b[;mSuccessfully set offsets as \x1b[1;37m%d \x1b[;mand \x1b[1;37m%d\x1b[;m.\n", xoff, yoff);
    return EXIT_SUCCESS;
}
