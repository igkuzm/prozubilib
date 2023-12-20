/**
 * File              : sfind.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 12.12.2023
 * Last Modified Date: 12.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/* Function to find and replace words in string */
#ifndef S_FIND_H_
#define S_FIND_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* sfind
 * read string, callback for each word that 
 * starts with one of the markers and write to other 
 * allocated string. return out string len or -1 on error
 * %in   - string to read
 * %out  - string to write
 * %size - size of data to read. Set 0 to read till null
 * %markers  - null-terminated array of markers
 * %userdata - data to transfer through callback
 * %callback - return non-null to stop function
 */
static int sfind(
		const char *in, char **out, size_t size, char *markers,
		void *userdata,
		int (*callback)(char **found, void *userdata));

/* IMPLIMATION */
/* dynamic string structure */
struct _sfind_str {
	char *str;   //null-terminated c string
	int   len;   //length of string (without last null char)
	int   size;  //allocated size
};

/* init string - return non-null on error */
static int _sfind_str_init(struct _sfind_str *s)
{
	// allocate data
	s->str = (char*)malloc(BUFSIZ);
	if (!s->str)
		return -1;

	// set dafaults
	s->str[0] = 0;
	s->len     = 0;
	s->size    = BUFSIZ;

	return 0;
}

static int _sfind_str_realloc(
		struct _sfind_str *s, int new_size)
{
	if (s->size < new_size){
		// do realloc
		size_t new_alloc_size = new_size + BUFSIZ;
		void *p = realloc(s->str, new_alloc_size);
		if (!p)
			return -1;
		s->str = (char*)p;
		s->size = new_alloc_size;
	}
	return 0;
}

static void _sfind_str_puts(
		struct _sfind_str *s, const char *str)
{
	if (!str)
		return;

	int len, new_size, i;
	
	len = strlen(str);
	new_size = s->len + len + 1;
	// realloc if not enough size
	if (_sfind_str_realloc(s, new_size))
		return;

	// append string
	for (i = 0; i < len; ++i)
		s->str[s->len++] = str[i];
  
	s->str[s->len] = 0;
}

static void _sfind_str_putc(
		struct _sfind_str *s, char c)
{
	int len=1, new_size, i;
	
	new_size = s->len + len + 1;
	// realloc if not enough size
	if (_sfind_str_realloc(s, new_size))
		return;

	// append char
	s->str[s->len++] = c;
	s->str[s->len] = 0;
}


int sfind(
		const char *in, char **out, size_t size, char *markers,
		void *userdata,
		int (*callback)(char **found, void *userdata))
{
	if (!out)
		return -1;

	struct _sfind_str s;
	if (_sfind_str_init(&s))
		return -1;

	*out = s.str;

	char ch;
	int i=0, stop=0;
	for (ch=in[i++];
			 ch && i < size && !stop;
			 ch=in[i++])
	{
		// check if word starts with marker
		char *m, marker, match=0;
		for (m=markers; *m; ++m)
			if (ch == marker)
				match = 1;

		if (match){
			// get word
			char buf[BUFSIZ];
			int blen=0;
			while (
					ch &&
					ch != ' '  && ch != '\\' && 
					ch != '.'  && ch != '\t' && 
					ch != '{'  && ch != '}'  && 
					ch != '\n' && ch != '\r' && 
					ch != ','  &&
					blen < BUFSIZ)
			{
				buf[blen++] = ch;
				ch = in[i++];
			}
			// terminate buffer
			buf[blen++] = 0;

			// callback
			char *word = buf;
			if (callback)
				if (callback(&word, userdata))
					stop=1;

			// write word to stream
			if (word)
				_sfind_str_puts(&s, word);
			
			// return last symbol to out
			_sfind_str_putc(&s, ch);
		
		} else
			_sfind_str_putc(&s, ch);
	}
	return s.len;
}
#endif /* ifndef S_FIND_H_ */
