#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

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
        printf("  \x1b[1;32m - \x1b[;m%s\x1b[;m\n", str);
}

static void shift_string(char *str, size_t n)
{
    size_t len = strlen(str);
    if (n > len)
        return;
    memmove(str, str + n, len - n + 1);
}

static volatile sig_atomic_t stop = 0;
static void interrupt_handler(int sig)
{
    stop = 1;
}

static const char* const events[EV_MAX + 1] = {
    [0 ... EV_MAX] = NULL,
    NAME_ELEMENT(EV_KEY),
    NAME_ELEMENT(EV_ABS)
};

static const int maxval[EV_MAX + 1] = {
    [0 ... EV_MAX] = -1,
    [EV_KEY] = KEY_MAX,
    [EV_ABS] = ABS_MAX
};

static const char * const absolutes[ABS_MAX + 1] = {
    [0 ... ABS_MAX] = NULL,
    NAME_ELEMENT(ABS_X),            NAME_ELEMENT(ABS_Y),
    NAME_ELEMENT(ABS_Z),            NAME_ELEMENT(ABS_RX),
    NAME_ELEMENT(ABS_RY),           NAME_ELEMENT(ABS_RZ),
    NAME_ELEMENT(ABS_THROTTLE),     NAME_ELEMENT(ABS_RUDDER),
    NAME_ELEMENT(ABS_WHEEL),        NAME_ELEMENT(ABS_GAS),
    NAME_ELEMENT(ABS_BRAKE),        NAME_ELEMENT(ABS_HAT0X),
    NAME_ELEMENT(ABS_HAT0Y),        NAME_ELEMENT(ABS_HAT1X),
    NAME_ELEMENT(ABS_HAT1Y),        NAME_ELEMENT(ABS_HAT2X),
    NAME_ELEMENT(ABS_HAT2Y),        NAME_ELEMENT(ABS_HAT3X),
    NAME_ELEMENT(ABS_HAT3Y),        NAME_ELEMENT(ABS_PRESSURE),
    NAME_ELEMENT(ABS_DISTANCE),     NAME_ELEMENT(ABS_TILT_X),
    NAME_ELEMENT(ABS_TILT_Y),       NAME_ELEMENT(ABS_TOOL_WIDTH),
    NAME_ELEMENT(ABS_VOLUME),       NAME_ELEMENT(ABS_MISC),
#ifdef ABS_MT_BLOB_ID
    NAME_ELEMENT(ABS_MT_TOUCH_MAJOR),
    NAME_ELEMENT(ABS_MT_TOUCH_MINOR),
    NAME_ELEMENT(ABS_MT_WIDTH_MAJOR),
    NAME_ELEMENT(ABS_MT_WIDTH_MINOR),
    NAME_ELEMENT(ABS_MT_ORIENTATION),
    NAME_ELEMENT(ABS_MT_POSITION_X),
    NAME_ELEMENT(ABS_MT_POSITION_Y),
    NAME_ELEMENT(ABS_MT_TOOL_TYPE),
    NAME_ELEMENT(ABS_MT_BLOB_ID),
#endif
#ifdef ABS_MT_TRACKING_ID
    NAME_ELEMENT(ABS_MT_TRACKING_ID),
#endif
#ifdef ABS_MT_PRESSURE
    NAME_ELEMENT(ABS_MT_PRESSURE),
#endif
#ifdef ABS_MT_SLOT
    NAME_ELEMENT(ABS_MT_SLOT),
#endif
#ifdef ABS_MT_TOOL_X
    NAME_ELEMENT(ABS_MT_TOOL_X),
    NAME_ELEMENT(ABS_MT_TOOL_Y),
    NAME_ELEMENT(ABS_MT_DISTANCE)
#endif
};

static const char * const keys[KEY_MAX + 1] = {
    [0 ... KEY_MAX] = NULL,
    NAME_ELEMENT(BTN_0),                NAME_ELEMENT(BTN_1),
    NAME_ELEMENT(BTN_2),                NAME_ELEMENT(BTN_3),
    NAME_ELEMENT(BTN_4),                NAME_ELEMENT(BTN_5),
    NAME_ELEMENT(BTN_6),                NAME_ELEMENT(BTN_7),
    NAME_ELEMENT(BTN_8),                NAME_ELEMENT(BTN_9),
    NAME_ELEMENT(BTN_LEFT),             NAME_ELEMENT(BTN_RIGHT),
    NAME_ELEMENT(BTN_MIDDLE),           NAME_ELEMENT(BTN_SIDE),
    NAME_ELEMENT(BTN_EXTRA),            NAME_ELEMENT(BTN_FORWARD),
    NAME_ELEMENT(BTN_BACK),             NAME_ELEMENT(BTN_TASK),
    NAME_ELEMENT(BTN_TRIGGER),          NAME_ELEMENT(BTN_THUMB),
    NAME_ELEMENT(BTN_THUMB2),           NAME_ELEMENT(BTN_TOP),
    NAME_ELEMENT(BTN_TOP2),             NAME_ELEMENT(BTN_PINKIE),
    NAME_ELEMENT(BTN_BASE),             NAME_ELEMENT(BTN_BASE2),
    NAME_ELEMENT(BTN_BASE3),            NAME_ELEMENT(BTN_BASE4),
    NAME_ELEMENT(BTN_BASE5),            NAME_ELEMENT(BTN_BASE6),
    NAME_ELEMENT(BTN_DEAD),             NAME_ELEMENT(BTN_C),

    NAME_ELEMENT(BTN_Z),                NAME_ELEMENT(BTN_TL),
    NAME_ELEMENT(BTN_TR),               NAME_ELEMENT(BTN_TL2),
    NAME_ELEMENT(BTN_TR2),              NAME_ELEMENT(BTN_SELECT),
    NAME_ELEMENT(BTN_START),            NAME_ELEMENT(BTN_MODE),
    NAME_ELEMENT(BTN_THUMBL),           NAME_ELEMENT(BTN_THUMBR),
    NAME_ELEMENT(BTN_TOOL_PEN),         NAME_ELEMENT(BTN_TOOL_RUBBER),
    NAME_ELEMENT(BTN_TOOL_BRUSH),       NAME_ELEMENT(BTN_TOOL_PENCIL),
    NAME_ELEMENT(BTN_TOOL_AIRBRUSH),    NAME_ELEMENT(BTN_TOOL_FINGER),
    NAME_ELEMENT(BTN_TOOL_MOUSE),       NAME_ELEMENT(BTN_TOOL_LENS),
    NAME_ELEMENT(BTN_TOUCH),            NAME_ELEMENT(BTN_STYLUS),
    NAME_ELEMENT(BTN_STYLUS2),          NAME_ELEMENT(BTN_TOOL_DOUBLETAP),
    NAME_ELEMENT(BTN_TOOL_TRIPLETAP)
};

static const char * const * const names[EV_MAX + 1] = {
    [0 ... EV_MAX] = NULL,
    [EV_KEY] = keys,
    [EV_ABS] = absolutes
};

static inline const char* typename(unsigned int type)
{
    return (type <= EV_MAX && events[type]) ? events[type] : "?";
}

static inline const char* codename(unsigned int type, unsigned int code)
{
    return (type <= EV_MAX && code <= maxval[type] && names[type] && names[type][code]) ? names[type][code] : "?";
}

static int print_events(int fd, int verbose)
{
    struct input_event ev[64];
    int i, rd;
    fd_set rdfs;

    FD_ZERO(&rdfs);
    FD_SET(fd, &rdfs);

    while (!stop)
    {
        select(fd + 1, &rdfs, NULL, NULL, NULL);
        if (stop)
            break;
        rd = read(fd, ev, sizeof(ev));

        if (rd < (int) sizeof(struct input_event))
        {
            perror("\n\x1b[1;31m => \x1b[;mError reading input!\n");
            printf("\x1b[1;32m => \x1b[;mExpected \x1b[1;37m%d\x1b[;m bytes, got \x1b[1;37m%d\n", (int) sizeof(struct input_event), rd);
            return EXIT_FAILURE;
        }

        for (i = 0; i < rd / sizeof(struct input_event); i++)
        {
            unsigned int type, code;
            type = ev[i].type;
            code = ev[i].code;

            if (type != EV_SYN && verbose == 1)
            {
                printf("  \x1b[1;32m - \x1b[;mType %s\x1b[1;32m/\x1b[;m%d \x1b[1;32m- \x1b[;mcode %s\x1b[1;32m/\x1b[;m%d",
                    typename(type), type, codename(type, code), code);
                if (type != EV_MSC && (code != MSC_RAW || code != MSC_SCAN))
                    printf("\x1b[1;32m - \x1b[;mValue %d\n", ev[i].value);
            }
        }
    }

    ioctl(fd, EVIOCGRAB, (void*)0);
    return EXIT_SUCCESS;
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
        printf("\x1b[1;32m => \x1b[;mIf you are sure your device supports absolute input, try following:\n");
        printf("\x1b[1;32m => \x1b[;mabstouch setevent\n");
        return EXIT_FAILURE;
    }

    logstr("Found absolute input on event.", verbose);
    return print_events(fd, verbose);
}
