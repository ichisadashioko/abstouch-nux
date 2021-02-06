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
#ifndef SET_CONFIG_H
#define SET_CONFIG_H

/*
 * Get X and Y offset from user manually and write to configuration.
 */
int set_offset(void);

/*
 * Get input event from user and write to configuration.
 */
int set_event(void);

/*
 * Get display and screen id from user and write to configuration.
 */
int set_display(void);

#endif /* SET_CONFIG_H */
