#define _GNU_SOURCE
#include <stdio.h>

#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#define DEV_INPUT_DIR "/dev/input"
#define EVENT_PREFIX "event"

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)
#define NAME_ELEMENT(element) [element] = #element

static void logstr(char *str, int verbose)
{
    if (verbose == 1)
        printf("\x1b[1;32m => \x1b[;m%s\n", str);
}

static void shift_string(char *str, size_t n)
{
    size_t len = strlen(str);
    if (n > len)
        return;
    memmove(str, str + n, len - n + 1);
}

static const char* const events[EV_MAX + 1] = {
	[0 ... EV_MAX] = NULL,
	NAME_ELEMENT(EV_SYN),			NAME_ELEMENT(EV_KEY),
	NAME_ELEMENT(EV_REL),			NAME_ELEMENT(EV_ABS),
	NAME_ELEMENT(EV_MSC),			NAME_ELEMENT(EV_LED),
	NAME_ELEMENT(EV_SND),			NAME_ELEMENT(EV_REP),
	NAME_ELEMENT(EV_FF),			NAME_ELEMENT(EV_PWR),
	NAME_ELEMENT(EV_FF_STATUS),		NAME_ELEMENT(EV_SW),
};

static inline const char* typename(unsigned int type)
{
	return (type <= EV_MAX && events[type]) ? events[type] : "?";
}

static int is_abs_device(int fd)
{
    unsigned int type;
    unsigned long bit[EV_MAX][NBITS(KEY_MAX)];

    memset(bit, 0, sizeof(bit));
	ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);

    for (type = 0; type < EV_MAX; type++)
    {
        if (test_bit(type, bit[0]) && type != EV_REP)
        {
            if (type == 3)
                return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int verbose = 0, event = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0)
            verbose = 1;

        if (strncmp(argv[i], "-event", 6) == 0)
        {
            char *p, *eventstr = argv[i];
            shift_string(eventstr, 6);
            event = strtol(eventstr, &p, 10);
        }
    }

    int fd = -1;
    char fname[64];
    snprintf(fname, sizeof(fname),
         "%s/%s%d", DEV_INPUT_DIR, EVENT_PREFIX, event);
    fd = open(fname, O_RDONLY);

    if (is_abs_device(fd) == 0)
    {
        printf("\x1b[1;31m => \x1b[;mEvent has no absolute input!\n");
        printf("\x1b[1;32m => \x1b[;mTry using: \x1b[1;32m`\x1b[;mabstouch setevent\x1b[1;32m`\x1b[;m\n");
        return EXIT_FAILURE;
    }

    logstr("Found absolute input on event.", verbose);

    return EXIT_SUCCESS;
}
