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

#include "str_functions.h"
#include "input_client.h"
#include "calibrate.h"
#include "set_config.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define DISPLAY_CONF_PATH "/usr/local/share/abstouch-nux/display.conf"
#define SCREEN_CONF_PATH "/usr/local/share/abstouch-nux/screen.conf"
#define EVENT_CONF_PATH "/usr/local/share/abstouch-nux/event.conf"
#define XOFF_CONF_PATH "/usr/local/share/abstouch-nux/xoff.conf"
#define YOFF_CONF_PATH "/usr/local/share/abstouch-nux/yoff.conf"

/*
 * The process name of abstouch-nux to check the pid of.
 */
static char *process_name;

/*
 * Main CLI of abstouch-nux. Gets the config values, parses the commands and
 * options and runs the functions according to them.
 */
int main(int argc, char *argv[])
{
    process_name = argv[0];

    char **options = (char **) malloc(0 * sizeof(char *));
    size_t options_size = 0;
    char **otherArgs = (char **) malloc(0 * sizeof(char *));
    size_t otherArgs_size = 0;

    for (size_t i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "--", 2)) {
            memmove(argv[i], argv[i] + 2, strlen(argv[i]));
            options = (char **) realloc(options, (options_size + 1) * sizeof(char *));
            options[options_size] = argv[i];
            options_size += 1;
        } else if (!strncmp(argv[i], "-", 1)) {
            memmove(argv[i], argv[i] + 1, strlen(argv[i]));
            for (size_t j = 0; j < strlen(argv[i]); j++) {
                char *pChar = malloc(sizeof(char));
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
        printf(" \x1b[1;31m=> \x1b[1;37mPlease provide a command!\n");
        printf(" \x1b[1;32m=> \x1b[1;37mUsage: \x1b[;mabstouch <command> [options?]\n");
        printf(" \x1b[1;32m=> \x1b[1;37mSee: \x1b[;mabstouch help\n");
        return EXIT_FAILURE;
    }

    char *command = otherArgs[0];
    for (size_t i = 0; i < strlen(command); i++)
        command[i] = tolower(command[i]);

    if (!strcmp(command, "help")) {
        printf("\x1b[1;32m---====\x1b[1;37mabstouch-nux\x1b[1;32m====---\n");
        printf(" \x1b[1;32m=> \x1b[1;37mUsage: \x1b[;mabstouch <command> [options?]\n");
        printf("\n");
        printf("\x1b[1;32m---======\x1b[1;37mCommands\x1b[1;32m======---\n");
        printf(" \x1b[1;32m=> \x1b[;mhelp \x1b[1;32m=> \x1b[;mShows help text.\n");
        printf(" \x1b[1;32m=> \x1b[;mstart \x1b[1;32m=> \x1b[;mStarts abstouch-nux.\n");
        printf(" \x1b[1;32m=> \x1b[;mstop \x1b[1;32m=> \x1b[;mTerminates abstouch-nux daemon.\n");
        printf(" \x1b[1;32m=> \x1b[;mcalibrate \x1b[1;32m=> \x1b[;mCalibrate to match your touchpad.\n");
        printf(" \x1b[1;32m=> \x1b[;msetevent \x1b[1;32m=> \x1b[;mSet event to match touchpad.\n");
        printf(" \x1b[1;32m=> \x1b[;msetoffset \x1b[1;32m=> \x1b[;mSet offset manually to match your touchpad.\n");
        printf(" \x1b[1;32m=> \x1b[;msetdisplay \x1b[1;32m=> \x1b[;mSet display and screen to match your setup.\n");
        printf("\n");
        printf("\x1b[1;32m---=======\x1b[1;37mOptions\x1b[1;32m======---\n");
        printf(" \x1b[1;32m=> \x1b[;m-q\x1b[1;32m, \x1b[;m--quiet \x1b[1;32m=> \x1b[;mDisables output except err.\n");
        printf(" \x1b[1;32m=> \x1b[;m-d\x1b[1;32m, \x1b[;m--daemon \x1b[1;32m=> \x1b[;mRuns in background.\n");
        printf("\n");
        printf("\x1b[1;32m---====================---\n");
        printf(" \x1b[1;32m=> \x1b[;mAlso see \x1b[1;37mabstouch.1 \x1b[;mman page for examples and more.\n");
        return EXIT_SUCCESS;
    } else if (!strcmp(command, "setevent"))
        return set_event();
    else if (!strcmp(command, "setoffset"))
        return set_offset();
    else if (!strcmp(command, "setdisplay"))
        return set_display();

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
        printf(" \x1b[1;31m=> \x1b[;mEvent not set!\n");
        if (!set_event()) {
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
            printf(" \x1b[1;31m=> \x1b[;mCouldn't set event!\n");
            return EXIT_FAILURE;
        }
    } else if (!event) {
        printf(" \x1b[1;31m=> \x1b[;mCouldn't get event!\n");
        return EXIT_FAILURE;
    }

    if (!strcmp(command, "calibrate"))
        return calibrate(event);

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

    char pidofCommand[256];
    snprintf(pidofCommand, sizeof(pidofCommand), "pidof -x %s", process_name);

    pid_t pid = getpid();
    int pidS_length = snprintf(NULL, 0, "%d", pid);
    char *pidstring = malloc(pidS_length + 1);
    snprintf(pidstring, pidS_length + 1, "%d", pid);
    char line[256];
    FILE *cmd = popen(pidofCommand, "r");
    fgets(line, 256, cmd);
    pclose(cmd);
    char *otherRaw = str_replace(line, pidstring, "");
    if (otherRaw == NULL)
        printf(" \x1b[1;31m=> \x1b[;mCouldn't get running processes!\n");
    char *other = str_replace(otherRaw, "\n", "");
    free(pidstring);
    free(otherRaw);

    char *display = 0;
    long display_length;
    FILE *display_f = fopen(DISPLAY_CONF_PATH, "rb");
    if (display_f) {
        fseek(display_f, 0, SEEK_END);
        display_length = ftell(display_f);
        fseek(display_f, 0, SEEK_SET);
        display = malloc(display_length + 1);
        if (display)
            fread(display, 1, display_length, display_f);
        fclose(display_f);
        display[display_length] = '\0';
    }

    int screen = 0;
    char *p;
    char *screenBuff = 0;
    long screenBuff_length;
    FILE *screenBuff_f = fopen(DISPLAY_CONF_PATH, "rb");
    if (screenBuff_f) {
        fseek(screenBuff_f, 0, SEEK_END);
        screenBuff_length = ftell(screenBuff_f);
        fseek(screenBuff_f, 0, SEEK_SET);
        screenBuff = malloc(screenBuff_length + 1);
        if (screenBuff)
            fread(screenBuff, 1, screenBuff_length, screenBuff_f);
        fclose(screenBuff_f);
        screenBuff[screenBuff_length] = '\0';
        screen = strtol(screenBuff, &p, 10);
        free(screenBuff);
    }

    if (!strcmp(command, "start")) {
        if (!strcmp(other, "")) {
            printf(" \x1b[1;32m=> \x1b[;mStarting abstouch-nux.");
            return input_client(event, xoff, yoff, display, screen, (verbose && !daemon), daemon);
        }
        else {
            printf(" \x1b[1;31m=> \x1b[;mAn abstouch-nux daemon is already running!\n");
            return EXIT_FAILURE;
        }
    } else if (!strcmp(command, "stop")) {
        if (!strcmp(other, "")) {
            printf(" \x1b[1;31m=> \x1b[;mNo abstouch-nux daemon is running!\n");
            return EXIT_FAILURE;
        } else {
            char *p;
            pid_t daemonPid = strtol(other, &p, 10);
            int killRes = kill(daemonPid, SIGKILL);
            if (killRes == 0)
                return EXIT_SUCCESS;
            else {
                printf(" \x1b[1;31m=> \x1b[;mCouldn't stop the abstouch-nux daemon!\n");
                return EXIT_FAILURE;
            }
        }
    }

    free(other);

    free(display);
    free(event);
    free(xoff);
    free(yoff);
    free(options);
    free(otherArgs);
    return EXIT_SUCCESS;
}
