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
#ifndef STR_FUNCTIONS_H
#define STR_FUNCTIONS_H

#include <stdlib.h>

/*
 * Prints the `str` in a nice format if `verbose` is true.
 */
void logstr(char *str, int verbose);

/*
 * Shifts the `str` to left by `n` characters.
 * Original code by Jonathan Leffler in Stack Overflow.
 * https://stackoverflow.com/a/4761840
 */
void shift_string(char *str, size_t n);

/*
 * Replaces the `rep` in `orig` with `with`.
 * Original code by jmucchiello in Stack Overflow.
 * https://stackoverflow.com/a/779960
 */
char *str_replace(char *orig, char *rep, char *with);

#endif /* STR_FUNCTIONS_H */
