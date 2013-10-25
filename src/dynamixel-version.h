/*
 * Copyright (C) 2013 Alexander Krause <alexander.krause@ed-solutions.de>
 * 
 * Dynamixel library - a fork from libmodbus (http://libmodbus.org)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _DYNAMIXEL_VERSION_H_
#define _DYNAMIXEL_VERSION_H_

/* The major version, (1, if %LIBDYNAMIXEL_VERSION is 1.2.3) */
#define LIBDYNAMIXEL_VERSION_MAJOR (@LIBDYNAMIXEL_VERSION_MAJOR@)

/* The minor version (2, if %LIBDYNAMIXEL_VERSION is 1.2.3) */
#define LIBDYNAMIXEL_VERSION_MINOR (@LIBDYNAMIXEL_VERSION_MINOR@)

/* The micro version (3, if %LIBDYNAMIXEL_VERSION is 1.2.3) */
#define LIBDYNAMIXEL_VERSION_MICRO (@LIBDYNAMIXEL_VERSION_MICRO@)

/* The full version, like 1.2.3 */
#define LIBDYNAMIXEL_VERSION        @LIBDYNAMIXEL_VERSION@

/* The full version, in string form (suited for string concatenation)
 */
#define LIBDYNAMIXEL_VERSION_STRING "@LIBDYNAMIXEL_VERSION@"

/* Numerically encoded version, like 0x010203 */
#define LIBDYNAMIXEL_VERSION_HEX ((LIBDYNAMIXEL_MAJOR_VERSION << 24) |        \
                               (LIBDYNAMIXEL_MINOR_VERSION << 16) |        \
                               (LIBDYNAMIXEL_MICRO_VERSION << 8))

/* Evaluates to True if the version is greater than @major, @minor and @micro
 */
#define LIBDYNAMIXEL_VERSION_CHECK(major,minor,micro)      \
    (LIBDYNAMIXEL_VERSION_MAJOR > (major) ||               \
     (LIBDYNAMIXEL_VERSION_MAJOR == (major) &&             \
      LIBDYNAMIXEL_VERSION_MINOR > (minor)) ||             \
     (LIBDYNAMIXEL_VERSION_MAJOR == (major) &&             \
      LIBDYNAMIXEL_VERSION_MINOR == (minor) &&             \
      LIBDYNAMIXEL_VERSION_MICRO >= (micro)))

#endif /* _DYNAMIXEL_VERSION_H_ */
