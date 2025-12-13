/**
 * File              : fstrrep.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.05.2024
 * Last Modified Date: 13.12.2025
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/**
 * fstrrep.h
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

/* find and replace string in file */

#ifndef FSTRREP_H
#define FSTRREP_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

/* copy %in stream to %out with replaced %needle to 
 * %replace - return number of replaced strings */
static int fstrrep(FILE *in, FILE *out, 
		const char *needle, const char *replace) 
{

  int nlen = strlen(needle);
  int rlen = strlen(replace);

  char buf[nlen];

  int i, cnt = 0;

  // clear buf
  for (int i = 0; i < nlen; i++)
    buf[i] = 0;

  i = 0;

  while (1) {
    int ch = fgetc(in);
    if (ch == EOF) {
      break;
    }

    if (ch == needle[i]) {
      buf[i] = ch;
      i++;
      if (i == nlen) {
        // found string matches
        cnt++;

        // replace string
        for (i = 0; i < rlen; ++i) {
          fputc(replace[i], out);
        }
        i = 0;
      }
    } else {
      if (i > 0) {
        // print buff
        int c = i;
        for (i = 0; i < c; ++i) {
          fputc(buf[i], out);
          // clear buf
          buf[i] = 0;
        }
      }
      i = 0;
      fputc(ch, out);
    }
  }

  return cnt;
}
#ifdef __cplusplus
}
#endif
#endif /* ifndef FSTRREP_H */
