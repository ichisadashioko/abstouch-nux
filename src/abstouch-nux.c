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

#include "input.h"
#include "calibrate.h"
#include "set_event.h"
#include "set_offset.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char **options = (char **) malloc(0 * sizeof(char *));
    size_t options_size = 0;
    char **otherArgs = (char **) malloc(0 * sizeof(char *));
    size_t otherArgs_size = 0;

    for (size_t i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "--", strlen("--"))) {
            memmove(argv[i], argv[i] + 2, strlen(argv[i]));
            options = (char **) realloc(options, (options_size + 1) * sizeof(char *));
            options[options_size] = argv[i];
            options_size += 1;
        } else if (!strncmp(argv[i], "-", strlen("-"))) {
            memmove(argv[i], argv[i] + 1, strlen(argv[i]));
            for (size_t j = 0; j < strlen(argv[i]); j++) {
                char *pChar = malloc(sizeof(char *));
                snprintf(pChar, sizeof(pChar), "%c", tolower(argv[i][j]));

                options = (char **) realloc(options, (options_size + 1) * sizeof(char *));
                options[options_size] = pChar;
                options_size += 1;
            }
        } else {
            otherArgs = (char **) realloc(otherArgs, (otherArgs_size + 1) * sizeof(char *));
            otherArgs[otherArgs_size] = argv[i];
            otherArgs_size += 1;
        }
    }

    if (otherArgs_size < 1) {
        printf("\x1b[1;31m => \x1b[1;37mPlease provide a command!\n");
        printf("\x1b[1;32m => \x1b[1;37mUsage: \x1b[;mabstouch <command> [options]\n");
        printf("\x1b[1;32m => \x1b[1;37mSee: \x1b[;mabstouch help\n");
        return EXIT_FAILURE;
    }

    char *command = otherArgs[0];
    for (size_t i = 0; i < strlen(command); i++) {
        command[i] = tolower(command[i]);
    }

    if (!strcmp(command, "help")) {
        printf("\x1b[1;32m---====\x1b[1;37mabstouch-nux\x1b[1;32m====---\n");
        printf("\x1b[1;32m => \x1b[1;37mUsage: \x1b[;mabstouch <command> [options]\n");
        printf("\n");
        printf("\x1b[1;32m---======\x1b[1;37mCommands\x1b[1;32m======---\n");
        printf("\x1b[1;32m => \x1b[;mhelp \x1b[1;32m=> \x1b[;mShows help text.\n");
        printf("\x1b[1;32m => \x1b[;mstart \x1b[1;32m=> \x1b[;mStarts abstouch-nux.\n");
        printf("\x1b[1;32m => \x1b[;mstop \x1b[1;32m=> \x1b[;mTerminates abstouch-nux daemon.\n");
        printf("\x1b[1;32m => \x1b[;mcalibrate \x1b[1;32m=> \x1b[;mCalibrate to match your touchpad.\n");
        printf("\x1b[1;32m => \x1b[;msetevent \x1b[1;32m=> \x1b[;mSet event to match touchpad.\n");
        printf("\x1b[1;32m => \x1b[;msetoffset \x1b[1;32m=> \x1b[;mSet offset manually to match your touchpad.\n");
        printf("\n");
        printf("\x1b[1;32m---=======\x1b[1;37mOptions\x1b[1;32m======---\n");
        printf("\x1b[1;32m => \x1b[;m-q\x1b[1;32m, \x1b[;m--quiet \x1b[1;32m=> \x1b[;mDisables output except err.\n");
        printf("\x1b[1;32m => \x1b[;m-d\x1b[1;32m, \x1b[;m--daemon \x1b[1;32m=> \x1b[;mRuns in background.\n");
        printf("\n");
        printf("\x1b[1;32m---====================---\n");
        printf("\x1b[1;32m => \x1b[;mAlso see \x1b[1;37mabstouch.1 \x1b[;mman page for examples and more.\n");
        return EXIT_SUCCESS;
    } else if (!strcmp(command, "setevent")) {
        char *setevent_argv[0];
        return set_event(0, setevent_argv);
    } else if (!strcmp(command, "setoffset")) {
        char *setoffset_argv[0];
        return set_event(0, setoffset_argv);
    }

    return EXIT_SUCCESS;
    //return input(argc, argv);
}
