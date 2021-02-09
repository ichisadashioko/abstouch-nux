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
#ifndef INPUT_CLIENT_H
#define INPUT_CLIENT_H

/*
 * Gets input fd from `eventStr`, get offsets from `x/yoffStr`, and get display
 * and screen ids from `displayStr` and `screenId` to take input from touchpad
 * and move cursor in display to the point where you touched in touchpad every
 * possible moment, according to the arguments. It forks the process itself and
 * kills this instance if `daemon` is true.
 */
int input_client(char *eventStr, char *xoffStr, char *yoffStr, char *displayStr,
    int screenId, int verbose, int daemon);

#endif /* INPUT_CLIENT_H */
