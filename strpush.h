/**
 * File              : strpush.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 05.12.2023
 * Last Modified Date: 05.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
/**
 * strpush.h
 * Copyright (c) 2023 Igor V. Sementsov <ig.kuzm@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/* simple dynamic size string 
 * USAGE:
 * char *str = NULL; // should be NULL on init
 * strpush(&str, "Hello ");
 * strpush(&str, "World!");
 * printf("%s\n", str);
 * strfree(&str); 
 */
#ifndef STR_H_
#define STR_H_

/* strpush
 * append string to dynamic allocated string
 */
static void strpush(char **str1, const char *str2);

/* strpushf
 * append formated string to dynamic allocated string
 */
#define strpushf(str1, ...) _strpushf(str1, __VA_ARGS__)

/* strfree
 * free dynamic allocated string
 */
static void strfree(char **str);

/* IMPLIMATION */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* str structure */
struct _str {
	int32_t size;   // allocated size
	int32_t len;    // string length
	char buf[];     // string data
};
#define _str_meta(str)\
	((struct _str *)((char *)(str) - 8))

/* append formated string */
#define _strpushf(s, fmt, ...)\
	({\
		char str[BUFSIZ];\
		sprintf(str, fmt, __VA_ARGS__);\
		strpush(s, str);\
	 })

static struct _str *_str_new()
{
	// init string
	int size = sizeof(struct _str) + 1;
	struct _str *s = 
		(struct _str*)malloc(size);
	if (s == NULL)
		return NULL;
	s->size   = size;
	s->len    = 0;
	s->buf[0] = 0;
	return s;
};

static void strpush(
		char **str1, const char *str2)
{
	if (str2 == NULL)
		return;

	struct _str *s;
	if (*str1 == NULL){
		s = _str_new();
	} else {
		s = _str_meta(*str1);
	}
	if (s == NULL)
		return;

	*str1 = s->buf;
	
	int len = strlen(str2);
	// realloc string to have enough space
	int new_size = 
		s->len + len + sizeof(struct _str) + 1;
	if (s->size < new_size){
		void *ptr = realloc(s, new_size);
		if (ptr == NULL)
			return;
		s = (struct _str *)ptr;
		s->size = new_size;
	}

	// cat string
	int i;
	for (i = 0; i < len; ++i)
		s->buf[s->len++] = str2[i];	
	
	// terminate string
	s->buf[s->len] = 0;
}

void strfree(char **str)
{
	if (*str == NULL)
		return;

	struct _str *s = 
		_str_meta(*str);
	free(s);
	*str = NULL;
}
#endif /* ifndef STR_H_ */
