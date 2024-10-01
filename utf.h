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
#include <stdint.h>

/* convert %len items of UTF-32 %c32 array to UTF-8 multybite 
 * null-terminated string and return number of bytes. 
 * set len to -1 to handle c32 as null-terminated string*/ 
static int c32tomb(char *s, const uint32_t *c32, int len)
{
	int i, l=0;
	for(i = 0; i < len || (len == -1 && c32[i]); ++i) 
	{
		if (c32[i] <= 0x7F) {
			// Plain single-byte ASCII.
			s[l++] = (char) c32[i];
		}
		else if (c32[i] <= 0x7FF) {
			// Two bytes.
			s[l++] = 0xC0 |  (c32[i] >> 6);
			s[l++] = 0x80 | ((c32[i] >> 0) & 0x3F);
		}
		else if (c32[i] <= 0xFFFF) {
			// Three bytes.
			s[l++] = 0xE0 |  (c32[i] >> 12);
			s[l++] = 0x80 | ((c32[i] >> 6) & 0x3F);
			s[l++] = 0x80 | ((c32[i] >> 0) & 0x3F);
		}
		else if (c32[i] <= 0x1FFFFF) {
			// Four bytes.
			s[l++] = 0xF0 |  (c32[i] >> 18);
			s[l++] = 0x80 | ((c32[i] >> 12) & 0x3F);
			s[l++] = 0x80 | ((c32[i] >> 6)  & 0x3F);
			s[l++] = 0x80 | ((c32[i] >> 0)  & 0x3F);
		}
		else if (c32[i] <= 0x3FFFFFF) {
			// Five bytes.
			s[l++] = 0xF8 |  (c32[i] >> 24);
			s[l++] = 0x80 | ((c32[i] >> 18) & 0x3F);
			s[l++] = 0x80 | ((c32[i] >> 12) & 0x3F);
			s[l++] = 0x80 | ((c32[i] >> 6)  & 0x3F);
			s[l++] = 0x80 | ((c32[i] >> 0)  & 0x3F);
		}
		else if (c32[i] <= 0x7FFFFFFF) {
			// Six bytes.
			s[l++] = 0xFC |  (c32[i] >> 30);
			s[l++] = 0x80 | ((c32[i] >> 24) & 0x3F);
			s[l++] = 0x80 | ((c32[i] >> 18) & 0x3F);
			s[l++] = 0x80 | ((c32[i] >> 12) & 0x3F);
			s[l++] = 0x80 | ((c32[i] >> 6)  & 0x3F);
			s[l++] = 0x80 | ((c32[i] >> 0)  & 0x3F);
		}
		else{
			// Invalid char; don't encode anything.
		}	
	}

	// null-terminate
	s[l] = 0;

	return l;
}

/* convert %len UTF-8 %s multybite array to UTF-32 %s32 
 * null-terminated string and return it's len 
 * set len to -1 to handle %s as null-terminated string */ 
static size_t mbtoc32(uint32_t *s32, const char *s, int len)
{
	int i, l=0;
	for (i = 0; i < len || (len == -1 && s[i]);)
	{
		if (s[i] >= 252){/* 6-bytes */
			s32[l]  = (s[i++] & 0x1)  << 30;  // 0b00000001
			s32[l] |= (s[i++] & 0x3F) << 24;  // 0b00111111	
			s32[l] |= (s[i++] & 0x3F) << 18;  // 0b00111111
			s32[l] |= (s[i++] & 0x3F) << 12;  // 0b00111111
			s32[l] |= (s[i++] & 0x3F) << 6;   // 0b00111111
			s32[l] |=  s[i++] & 0x3F;         // 0b00111111
			l++;
		} 
		else if (s[i] >= 248){/* 5-bytes */
			s32[l]  = (s[i++] & 0x3)  << 24;  // 0b00000011
			s32[l] |= (s[i++] & 0x3F) << 18;  // 0b00111111
			s32[l] |= (s[i++] & 0x3F) << 12;  // 0b00111111
			s32[l] |= (s[i++] & 0x3F) << 6;   // 0b00111111
			s32[l] |=  s[i++] & 0x3F;         // 0b00111111
			l++;
		}
		else if (s[i] >= 240){/* 4-bytes */
			s32[l]  = (s[i++] & 0x7)  << 18;  // 0b00000111
			s32[l] |= (s[i++] & 0x3F) << 12;  // 0b00111111
			s32[l] |= (s[i++] & 0x3F) << 6;   // 0b00111111
			s32[l] |=  s[i++] & 0x3F;         // 0b00111111
			l++;
		} 
		else if (s[i] >= 224){/* 3-bytes */
			s32[l]  = (s[i++] & 0xF)  << 12;  // 0b00001111
			s32[l] |= (s[i++] & 0x3F) << 6;   // 0b00111111
			s32[l] |=  s[i++] & 0x3F;         // 0b00111111
			l++;                
		}
		else if (s[i] >= 192){/* 2-bytes */
			s32[l]  = (s[i++] & 0x1F) << 6;   // 0b00011111
			s32[l] |=  s[i++] & 0x3F;         // 0b00111111 
			l++; 
		} 
		else{/* 1-byte */
			s32[l++] = s[i++];
		} 
	}

	// null-terminate string
	s32[l] = 0;
	return l;
}	

#ifdef __cplusplus
}
#endif

#endif // UTF_H
