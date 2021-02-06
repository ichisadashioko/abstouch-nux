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
#ifndef EVENT_H
#define EVENT_H

#include <dirent.h>

#include <linux/input.h>

#define DEV_INPUT_DIR "/dev/input"
#define EVENT_PREFIX "event"

extern const char *const events[EV_MAX + 1];

extern const int maxval[EV_MAX + 1];

extern const char *const absolutes[ABS_MAX + 1];

extern const char *const keys[KEY_MAX + 1];

extern const char *const *const names[EV_MAX + 1];

int is_event_device(const struct dirent *dir);

const char *typename(unsigned int type);

const char *codename(unsigned int type, unsigned int code);

int is_abs_device(int fd);

int get_event_by_name(char *ename);

#endif /* EVENT_H */
