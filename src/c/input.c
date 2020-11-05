#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>

#include <linux/version.h>
#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>

static void shift_string(char *str, size_t n)
{
    size_t len = strlen(str);
    if (n > len)
        return;
    memmove(str, str + n, len - n + 1);
}

int main(int argc, char *argv[])
{
    int verbose = 0, event = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0)
            verbose = 1;

        if (strncmp(argv[i], "-event", strlen("-event")) == 0)
        {
            char *p, *eventstr = argv[i];
            shift_string(eventstr, 6);
            event = strtol(eventstr, &p, 10);
        }
    }

    printf(" => input.c\n => Verbose: %d\n => Event: %d\n", verbose, event);
}
