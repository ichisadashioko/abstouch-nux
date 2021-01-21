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

#define EVENT_CONF_PATH "/usr/local/share/abstouch-nux/event.conf"
#define XOFF_CONF_PATH "/usr/local/share/abstouch-nux/xoff.conf"
#define YOFF_CONF_PATH "/usr/local/share/abstouch-nux/yoff.conf"

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
        char *setevent_argv[1] = {"setevent"};
        return set_event(1, setevent_argv);
    } else if (!strcmp(command, "setoffset")) {
        char *setoffset_argv[1] = {"setoffset"};
        return set_offset(1, setoffset_argv);
    }

    int verbose = 1;
    int daemon = 0;
    for (size_t i = 0; i < options_size; i++) {
        if (!strcmp(options[i], "q") || !strcmp(options[i], "quiet"))
            verbose = 0;
        else if (!strcmp(options[i], "d") || !strcmp(options[i], "daemon"))
            daemon = 1;
    }

    char *event = 0;
    long event_length;
    FILE *event_f = fopen(EVENT_CONF_PATH, "rb");
    if (event_f) {
        fseek(event_f, 0, SEEK_END);
        event_length = ftell(event_f);
        fseek(event_f, 0, SEEK_SET);
        event = malloc(event_length + 1);
        if (event)
            fread(event, 1, event_length, event_f);
        fclose(event_f);
        event[event_length] = '\0';
    }

    if (event && (!strcmp(event, "-1") || !strcmp(event, "-1\n"))) {
        printf("\x1b[1;31m => \x1b[;mEvent not set!\n");
        char *setevent_argv[1] = {"setevent"};
        if (set_event(1, setevent_argv)) {
            FILE *event_ff = fopen(EVENT_CONF_PATH, "rb");
            if (event_ff) {
                fseek(event_ff, 0, SEEK_END);
                event_length = ftell(event_ff);
                fseek(event_ff, 0, SEEK_SET);
                event = malloc(event_length + 1);
                if (event)
                    fread(event, 1, event_length, event_ff);
                fclose(event_ff);
                event[event_length] = '\0';
            }
        }

        if (event && (!strcmp(event, "-1") || !strcmp(event, "-1\n"))) {
            printf("\x1b[1;31m => \x1b[;mEvent still not set!\n");
            return EXIT_FAILURE;
        }
    } else if (!event) {
        printf("\x1b[1;31m => \x1b[;mCouldn't get event!\n");
        return EXIT_FAILURE;
    }

    if (!strcmp(command, "calibrate")) {
        char *calibrate_argv[2] = {"calibrate"};
        char event_arg[256];
        snprintf(event_arg, (sizeof(event_arg) / sizeof(char)), "-event%s", event);
        calibrate_argv[1] = event_arg;
        return calibrate(2, calibrate_argv);
    }

    char *xoff = 0;
    long xoff_length;
    FILE *xoff_f = fopen(XOFF_CONF_PATH, "rb");
    if (xoff_f) {
        fseek(xoff_f, 0, SEEK_END);
        xoff_length = ftell(xoff_f);
        fseek(xoff_f, 0, SEEK_SET);
        xoff = malloc(xoff_length + 1);
        if (xoff)
            fread(xoff, 1, xoff_length, xoff_f);
        fclose(xoff_f);
        xoff[xoff_length] = '\0';
    }

    char *yoff = 0;
    long yoff_length;
    FILE *yoff_f = fopen(YOFF_CONF_PATH, "rb");
    if (yoff_f) {
        fseek(yoff_f, 0, SEEK_END);
        yoff_length = ftell(yoff_f);
        fseek(yoff_f, 0, SEEK_SET);
        yoff = malloc(yoff_length + 1);
        if (yoff)
            fread(yoff, 1, yoff_length, yoff_f);
        fclose(yoff_f);
        yoff[yoff_length] = '\0';
    }

    if (!strcmp(command, "start")) {
        if (daemon) {
            printf("\x1b[1;31m => \x1b[;mDaemon not implemented yet!\n");
            return EXIT_FAILURE;
        } else {
            char *input_argv[5] = {"input"};
            char event_arg[256];
            char xoff_arg[256];
            char yoff_arg[256];
            snprintf(event_arg, (sizeof(event_arg) / sizeof(char)), "-event%s", event);
            snprintf(xoff_arg, (sizeof(xoff_arg) / sizeof(char)), "-xoff%s", xoff);
            snprintf(yoff_arg, (sizeof(yoff_arg) / sizeof(char)), "-yoff%s", yoff);
            input_argv[1] = event_arg;
            input_argv[2] = xoff_arg;
            input_argv[3] = yoff_arg;
            if (verbose)
                input_argv[4] = "-v";
            else
                input_argv[4] = "";
            return input(5, input_argv);
        }
    } else if (!strcmp(command, "stop")) {
        printf("\x1b[1;31m => \x1b[;mDaemon not implemented yet!\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
