/**
 * File              : utf.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 27.05.2022
 * Last Modified Date: 01.10.2024
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

/* convert utf32 null-terminated string to utf8 multybite 
 * null-terminated string and return number of bytes */ 
static int c32tomb(char *s, const uint32_t *c32){
	int i = 0;
	uint32_t *ptr = (uint32_t *)c32;
	while (*ptr) {
		if (*ptr <= 0x7F) {
			// Plain single-byte ASCII.
			s[i++] = (char) *ptr;
		}
		else if (*ptr <= 0x7FF) {
			// Two bytes.
			s[i++] = 0xC0 |  (*ptr >> 6);
			s[i++] = 0x80 | ((*ptr >> 0) & 0x3F);
		}
		else if (*ptr <= 0xFFFF) {
			// Three bytes.
			s[i++] = 0xE0 |  (*ptr >> 12);
			s[i++] = 0x80 | ((*ptr >> 6) & 0x3F);
			s[i++] = 0x80 | ((*ptr >> 0) & 0x3F);
		}
		else if (*ptr <= 0x1FFFFF) {
			// Four bytes.
			s[i++] = 0xF0 |  (*ptr >> 18);
			s[i++] = 0x80 | ((*ptr >> 12) & 0x3F);
			s[i++] = 0x80 | ((*ptr >> 6)  & 0x3F);
			s[i++] = 0x80 | ((*ptr >> 0)  & 0x3F);
		}
		else if (*ptr <= 0x3FFFFFF) {
			// Five bytes.
			s[i++] = 0xF8 |  (*ptr >> 24);
			s[i++] = 0x80 | ((*ptr >> 18) & 0x3F);
			s[i++] = 0x80 | ((*ptr >> 12) & 0x3F);
			s[i++] = 0x80 | ((*ptr >> 6)  & 0x3F);
			s[i++] = 0x80 | ((*ptr >> 0)  & 0x3F);
		}
		else if (*ptr <= 0x7FFFFFFF) {
			// Six bytes.
			s[i++] = 0xFC |  (*ptr >> 30);
			s[i++] = 0x80 | ((*ptr >> 24) & 0x3F);
			s[i++] = 0x80 | ((*ptr >> 18) & 0x3F);
			s[i++] = 0x80 | ((*ptr >> 12) & 0x3F);
			s[i++] = 0x80 | ((*ptr >> 6)  & 0x3F);
			s[i++] = 0x80 | ((*ptr >> 0)  & 0x3F);
		}
		else{
			// Invalid char; don't encode anything.
		}	
		// iterate
		ptr++;
	}

	// null-terminate
	*ptr = 0;

	return i;
}

/* convert utf8 multybite null-terminated string 
 * to utf32 null-terminated string and return it's len */ 
static size_t mbtoc32(uint32_t *s32, const char *s){
	char *ptr = (char *)s;
	size_t i = 0;
	while (*ptr){
		uint8_t c = *ptr;
		if (c >= 252){/* 6-bytes */
			s32[i]  = (*ptr++ & 0x1)  << 30;  // 0b00000001
			s32[i] |= (*ptr++ & 0x3F) << 24;  // 0b00111111	
			s32[i] |= (*ptr++ & 0x3F) << 18;  // 0b00111111
			s32[i] |= (*ptr++ & 0x3F) << 12;  // 0b00111111
			s32[i] |= (*ptr++ & 0x3F) << 6;   // 0b00111111
			s32[i] |=  *ptr++ & 0x3F;         // 0b00111111
			i++;
		} 
		else if (c >= 248){/* 5-bytes */
			s32[i]  = (*ptr++ & 0x3)  << 24;  // 0b00000011
			s32[i] |= (*ptr++ & 0x3F) << 18;  // 0b00111111
			s32[i] |= (*ptr++ & 0x3F) << 12;  // 0b00111111
			s32[i] |= (*ptr++ & 0x3F) << 6;   // 0b00111111
			s32[i] |=  *ptr++ & 0x3F;         // 0b00111111
			i++;
		}
		else if (c >= 240){/* 4-bytes */
			s32[i]  = (*ptr++ & 0x7)  << 18;  // 0b00000111
			s32[i] |= (*ptr++ & 0x3F) << 12;  // 0b00111111
			s32[i] |= (*ptr++ & 0x3F) << 6;   // 0b00111111
			s32[i] |=  *ptr++ & 0x3F;         // 0b00111111
			i++;
		} 
		else if (c >= 224){/* 3-bytes */
			s32[i]  = (*ptr++ & 0xF)  << 12;  // 0b00001111
			s32[i] |= (*ptr++ & 0x3F) << 6;   // 0b00111111
			s32[i] |=  *ptr++ & 0x3F;         // 0b00111111
			i++;                
		}
		else if (c >= 192){/* 2-bytes */
			s32[i]  = (*ptr++ & 0x1F) << 6;   // 0b00011111
			s32[i] |=  *ptr++ & 0x3F;         // 0b00111111 
			i++; 
		} 
		else{/* 1-byte */
			s32[i++] = *ptr++;
		} 
	}

	// null-terminate string
	s32[i] = 0;
	return i;
}	

#ifdef __cplusplus
}
#endif

#endif // UTF_H
