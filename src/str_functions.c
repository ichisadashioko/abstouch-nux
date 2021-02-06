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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Prints the `str` in a nice format if `verbose` is true.
 */
void logstr(char *str, int verbose)
{
    if (verbose)
        printf("   \x1b[1;32m- \x1b[;m%s\x1b[;m\n", str);
}

/*
 * Shifts the `str` to left by `n` characters.
 * Original code by Jonathan Leffler in Stack Overflow.
 * https://stackoverflow.com/a/4761840
 */
void shift_string(char *str, size_t n)
{
    size_t len = strlen(str);
    if (n > len)
        return;
    memmove(str, str + n, len - n + 1);
}

/*
 * Replaces the `rep` in `orig` with `with`.
 * Original code by jmucchiello in Stack Overflow.
 * https://stackoverflow.com/a/779960
 */
char *str_replace(char *orig, char *rep, char *with)
{
    char *result;
    char *ins;
    char *tmp;
    int len_rep;
    int len_with;
    int len_front;
    int count;

    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL;
    if (!with)
        with = "";
    len_with = strlen(with);

    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count)
        ins = tmp + len_rep;

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep;
    }
    strcpy(tmp, orig);
    return result;
}
