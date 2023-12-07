/**
 * File              : str.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.12.2023
 * Last Modified Date: 04.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/* simple dynamic size string */
#ifndef STR_H_
#define STR_H_

/* str structure */
struct str {
	char *str; // null-terminated c string
	int len;   // string length
	int size;  // allocated size
};

/* init string */
static int str_init(struct str *s);

/* append string */
static void str_cat(
		struct str *s, const char *str);

/* append formated string */
#define str_catf(s, fmt, ...)\
	({\
		char str[BUFSIZ];\
		sprintf(str, fmt, __VA_ARGS__);\
		str_cat(s, str);\
	 })

/* IMPLIMATION */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int str_init(
		struct str *s)
{
	// init string
	s->str = (char*)malloc(1);
	if (s->str == NULL)
		return -1;
	s->str[0] = 0;
	s->size = 1;
	s->len = 0;
	return 0;
};

static void str_cat(
		struct str *s, const char *str)
{
	int len = strlen(str);
	// realloc string to have enough space
	int new_size = s->len + len + 1;
	if (s->size < new_size){
		char *ptr = 
			(char *)realloc(s->str, new_size);
		if (ptr == NULL)
			return;
		s->str = ptr;
		s->size= new_size;
	}

	// cat string
	int i;
	for (i = 0; i < len; ++i)
		s->str[s->len++] = str[i];	
	
	// terminate string
	s->str[s->len] = 0;
}

#endif /* ifndef STR_H_ */
