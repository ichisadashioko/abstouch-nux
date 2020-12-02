#define _GNU_SOURCE
#include <stdio.h>

#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#define DEV_INPUT_DIR "/dev/input"
#define EVENT_PREFIX "event"
#define XOFF_CONF_PATH "/usr/share/abstouch-nux/xoff.conf"
#define YOFF_CONF_PATH "/usr/share/abstouch-nux/yoff.conf"

static void shift_string(char *str, size_t n)
{
    size_t len = strlen(str);
    if (n > len)
        return;
    memmove(str, str + n, len - n + 1);
}

int main(int argc, char *argv[])
{
    int event = 0;
    char *p;

    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "-event", 6) == 0)
        {
            char *eventstr = argv[i];
            shift_string(eventstr, 6);
            event = strtol(eventstr, &p, 10);
        }
    }

    printf("\x1b[1;32m => \x1b[1;37mTouch the top left corner of your touchpad\n");
    printf("\x1b[1;32m => \x1b[1;37mthen press enter.\n");

    getchar();
    // Handle offset by top left

    printf("\x1b[1;32m => \x1b[1;37mTouch the bottom right corner of your touchpad\n");
    printf("\x1b[1;32m => \x1b[1;37mthen press enter.\n");

    getchar();
    // Handle offset by bottom right

    return EXIT_SUCCESS;
}
