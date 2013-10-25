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
#ifndef COMPAT_C

#ifndef HAVE_STRLCPY
/*
* Function strlcpy was originally developed by
* Todd C. Miller <Todd.Miller@courtesan.com> to simplify writing secure code.
* See ftp://ftp.openbsd.org/pub/OpenBSD/src/lib/libc/string/strlcpy.3
* for more information.
*
* Thank you Ulrich Drepper... not!
*
* Copy src to string dest of size dest_size.  At most dest_size-1 characters
* will be copied.  Always NUL terminates (unless dest_size == 0).  Returns
* strlen(src); if retval >= dest_size, truncation occurred.
*/
size_t strlcpy(char *dest, const char *src, size_t dest_size) {
	register char *d = dest;
	register const char *s = src;
	register size_t n = dest_size;

	/* Copy as many bytes as will fit */
	if ((n != 0) && (--n != 0)) {
		do {
			if ((*d++ = *s++) == 0) {
					break;
			}
		} while (--n != 0);
	}

	/* Not enough room in dest, add NUL and traverse rest of src */
	if (n == 0) {
		if (dest_size != 0) {
			*d = '\0'; /* NUL-terminate dest */
		}
		while (*s++) {
				;
		}
	}

	return (s - src - 1); /* count does not include NUL */
}
#endif

#endif
