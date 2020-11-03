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

int main(int argc, char *argv[])
{
    int verbose = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0)
            verbose = 1;
    }
}
