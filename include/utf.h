/**
 * File              : utf.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 27.05.2022
 * Last Modified Date: 02.10.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/**
 * utf.h
 * Copyright (c) 2022 Igor V. Sementsov <ig.kuzm@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* convert betwin Unicode and UTF8 */

#ifndef UTF_H
#define UTF_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>

/* convert UTF-32 %c32 character to UTF-8 null-terminated 
 * multybite array, and return pointer to last 
 * character in it */ 
static char *c32tomb(char *s, unsigned int c32)
{
	char *p = s;
	if (c32 <= 0x7F) {
		// Plain single-byte ASCII.
		*p++ = (char) c32;
	}
	else if (c32 <= 0x7FF) {
		// Two bytes.
		*p++ = 0xC0 |  (c32 >> 6);
		*p++ = 0x80 | ((c32 >> 0) & 0x3F);
	}
	else if (c32 <= 0xFFFF) {
		// Three bytes.
		*p++ = 0xE0 |  (c32 >> 12);
		*p++ = 0x80 | ((c32 >> 6) & 0x3F);
		*p++ = 0x80 | ((c32 >> 0) & 0x3F);
	}
	else if (c32 <= 0x1FFFFF) {
		// Four bytes.
		*p++ = 0xF0 |  (c32 >> 18);
		*p++ = 0x80 | ((c32 >> 12) & 0x3F);
		*p++ = 0x80 | ((c32 >> 6)  & 0x3F);
		*p++ = 0x80 | ((c32 >> 0)  & 0x3F);
	}
	else if (c32 <= 0x3FFFFFF) {
		// Five bytes.
		*p++ = 0xF8 |  (c32 >> 24);
		*p++ = 0x80 | ((c32 >> 18) & 0x3F);
		*p++ = 0x80 | ((c32 >> 12) & 0x3F);
		*p++ = 0x80 | ((c32 >> 6)  & 0x3F);
		*p++ = 0x80 | ((c32 >> 0)  & 0x3F);
	}
	else if (c32 <= 0x7FFFFFFF) {
		// Six bytes.
		*p++ = 0xFC |  (c32 >> 30);
		*p++ = 0x80 | ((c32 >> 24) & 0x3F);
		*p++ = 0x80 | ((c32 >> 18) & 0x3F);
		*p++ = 0x80 | ((c32 >> 12) & 0x3F);
		*p++ = 0x80 | ((c32 >> 6)  & 0x3F);
		*p++ = 0x80 | ((c32 >> 0)  & 0x3F);
	}
	else{
		// Invalid char; don't encode anything.
	}	

	*p = 0;
	return p;
}

/* convert UTF-8 multybite character %s to UTF-32 %s32 
 * character and return pointer to next %s character */ 
static char *mbtoc32(unsigned int *s32, const char *s)
{
	int i=0;
	unsigned char *us;
	if (us[i] >= 252){/* 6-bytes */
		*s32  = (s[i++] & 0x1)  << 30;  // 0b00000001
		*s32 |= (s[i++] & 0x3F) << 24;  // 0b00111111	
		*s32 |= (s[i++] & 0x3F) << 18;  // 0b00111111
		*s32 |= (s[i++] & 0x3F) << 12;  // 0b00111111
		*s32 |= (s[i++] & 0x3F) << 6;   // 0b00111111
		*s32 |=  s[i++] & 0x3F;         // 0b00111111
	} 
	else if (us[i] >= 248){/* 5-bytes */
		*s32  = (s[i++] & 0x3)  << 24;  // 0b00000011
		*s32 |= (s[i++] & 0x3F) << 18;  // 0b00111111
		*s32 |= (s[i++] & 0x3F) << 12;  // 0b00111111
		*s32 |= (s[i++] & 0x3F) << 6;   // 0b00111111
		*s32 |=  s[i++] & 0x3F;         // 0b00111111
	}
	else if (us[i] >= 240){/* 4-bytes */
		*s32  = (s[i++] & 0x7)  << 18;  // 0b00000111
		*s32 |= (s[i++] & 0x3F) << 12;  // 0b00111111
		*s32 |= (s[i++] & 0x3F) << 6;   // 0b00111111
		*s32 |=  s[i++] & 0x3F;         // 0b00111111
	} 
	else if (us[i] >= 224){/* 3-bytes */
		*s32  = (s[i++] & 0xF)  << 12;  // 0b00001111
		*s32 |= (s[i++] & 0x3F) << 6;   // 0b00111111
		*s32 |=  s[i++] & 0x3F;         // 0b00111111
	}
	else if (us[i] >= 192){/* 2-bytes */
		*s32  = (s[i++] & 0x1F) << 6;   // 0b00011111
		*s32 |=  s[i++] & 0x3F;         // 0b00111111 
	} 
	else{/* 1-byte */
		*s32 = s[i++];
	} 

	return &((char *)s)[i];
}	

#ifdef __cplusplus
}
#endif

#endif // UTF_H
