#include <stdio.h>
#include <stdlib.h>

#define XOFF_CONF_PATH "/usr/share/abstouch-nux/xoff.conf"
#define YOFF_CONF_PATH "/usr/share/abstouch-nux/yoff.conf"

int main(int argc, char *argv[])
{
    int xoff, yoff;
    char *p, xoffs[100], yoffs[100];

    printf("\x1b[1;32m => \x1b[1;37mEnter new x offset: \x1b[;m");
    while (fgets(xoffs, sizeof(xoffs), stdin))
    {
        xoff = strtol(xoffs, &p, 10);
        if (p == xoffs || *p != '\n')
            printf("\x1b[A\x1b[2K\x1b[1;32m => \x1b[1;37mEnter new x offset: \x1b[;m");
        else
            break;
    }

    printf("\x1b[1;32m => \x1b[1;37mEnter new y offset: \x1b[;m");
    while (fgets(yoffs, sizeof(yoffs), stdin))
    {
        yoff = strtol(yoffs, &p, 10);
        if (p == yoffs || *p != '\n')
            printf("\x1b[A\x1b[2K\x1b[1;32m => \x1b[1;37mEnter new y offset: \x1b[;m");
        else
            break;
    }

    printf("\x1b[1;31m => \x1b[;mCouldn't set offset!\n");

    FILE *xfp = fopen(XOFF_CONF_PATH, "w");
    fprintf(xfp, "%d", xoff);
    fclose(xfp);
    FILE *yfp = fopen(YOFF_CONF_PATH, "w");
    fprintf(yfp, "%d", yoff);
    fclose(yfp);

    printf("\x1b[A\x1b[2K\x1b[1;32m => \x1b[;mSuccessfully set offsets as \x1b[1;37m%d \x1b[;mand \x1b[1;37m%d\x1b[;m.\n", xoff, yoff);
    return EXIT_SUCCESS;
}
