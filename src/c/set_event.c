#define _GNU_SOURCE
#include <stdio.h>

#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

static int is_event_device(const struct dirent *dir)
{
    return strncmp("event", dir->d_name, 5) == 0;
}

static int scan_devices(void)
{
    struct dirent **namelist;
    int i, ndev, devnum;
    char *filename;
    int max_device = 0;

    ndev = scandir("/dev/input", &namelist, is_event_device, versionsort);
    if (ndev < 1)
    {
        return -1;
    }

    printf("\x1b[1;32m => \x1b[1;37mEvents:\n");
    for (i = 0; i < ndev; i++)
    {
        char fname[64];
        int fd = -1;
        char name[256] = "\x1b[1;31mUnknown\x1b[;m";

        snprintf(fname, sizeof(fname),
			 "%s/%s", "/dev/input", namelist[i]->d_name);
		fd = open(fname, O_RDONLY);
		if (fd < 0)
			continue;
		ioctl(fd, EVIOCGNAME(sizeof(name)), name);

		printf("\x1b[1;32m   => \x1b[1;37m%s \x1b[1;32m=> \x1b[;m%s\n", fname, name);
		close(fd);

		sscanf(namelist[i]->d_name, "event%d", &devnum);
		if (devnum > max_device)
			max_device = devnum;

		free(namelist[i]);
    }

    return max_device;
}

int main(int argc, char *argv[])
{
    int event;
    int max_input = scan_devices();

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

    FILE *fp = fopen("/usr/share/abstouch-nux/event.conf", "w");
    fprintf(fp, "%d", event);
    fclose(fp);

    printf("\x1b[1;32m => \x1b[;mSuccessfully set event id as \x1b[1;37m%d\x1b[;m.", event);
    return 0;
}
