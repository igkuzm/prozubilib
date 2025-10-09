/**
 * File              : strrep.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 19.05.2024
 * Last Modified Date: 21.05.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/**
 * strrep.h
 * Copyright (c) 2024 Igor V. Sementsov <ig.kuzm@gmail.com>
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

#ifndef STRREP_H
#define STRREP_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* return new allocated string with contents of haystack 
 * replaced all needle matches by replace string */
static char * strrep(const char *haystack, // string to scan
                     const char *needle,   // search pattern
                     const char *replace)  // replace string
{
  size_t hlen = strlen(haystack);
  size_t nlen = strlen(needle);
  size_t rlen = strlen(replace);

	// get size diffrence
	int diff = (rlen - nlen);
	if (diff < 0)
		diff = 0;

	size_t len = hlen + 1;
	char *res = (char *)malloc(len);
	if (!res)
		return NULL;

	int i = 0;
	char *ptr = (char *)haystack;
	while (*ptr){
		char *p = strstr(ptr, needle);
		if (p){
			// copy before string match
			while (ptr != p)
				res[i++] = *ptr++;

			// realloc if diff
			if (diff){
				len += rlen;
				res = (char *)realloc(res, len);
				if (!res)
					return NULL;
			}

			// copy replace string
			char *rp = (char *)replace;
			while (*rp)
				res[i++] = *rp++;

			// move ptr to end of needle
			ptr += nlen;

		// no match - copy string
		} else
				res[i++] = *ptr++;
	}

	//NULL-terminate
	res[i] = 0;

  return res;
}
#ifdef __cplusplus
}
#endif
#endif /* ifndef FSTRREP_H */
