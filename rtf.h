/**
 * File              : rtf.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.12.2023
 * Last Modified Date: 03.10.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/* functions to handle RTF files */

#ifndef RTF_H_
#define RTF_H_
#include <stdio.h>

/* rtf_from_utf8
 * return string with rtf code from utf8 multibite 
 * sting or NULL on error */
static char *
rtf_from_utf8(const char *s);

/* rtf_table_header
 * return string with rtf code of table header
 * or NULL on error
 * %coln   - number of columns
 * %titles - array of columns titles
 * %width  - array of columns width */
static char *
rtf_table_header(int coln, const char *titles[], int *width);

/* rtf_table_row
 * return string with rtf code of table row
 * or NULL on error
 * %coln  - number of columns
 * %colv  - columns values */
static char *
rtf_table_row(int coln, char *colv[]);

/* rtf_table_row_from_string
 * return string with rtf code of table row
 * or NULL on error
 * %colv  - string with columns values separeted by delim
 * %delim - string with delim chars */
static char *
rtf_table_row_from_string(
		const char *colv, const char *delim);

/* convert image to RTF string 
 * valid formats: emf, png, jpeg
 * with and height are in twips (inches * 20) - 
 * for A4 13011x16838 */
static char *
rtf_from_image(
		const char *format, void *data, size_t len,
		int width, int height);

/* IMPLIMATION */
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

char *
rtf_from_utf8(const char *s)
{
	if (!s)
		return NULL;
	
	size_t len = strlen(s);
	char *out = (char *)malloc(len * 6 + 1);
	if (!out)
		return NULL;
	strcpy(out, "");

	char *ptr = (char *)s, buf[8] = "";
	uint32_t c32;
	while(*ptr){
		// get char
		uint8_t c = *ptr;
		if (c >= 252){/* 6-bytes */
			c32  = (*ptr++ & 0x1)  << 30;  // 0b00000001
			c32 |= (*ptr++ & 0x3F) << 24;  // 0b00111111
			c32 |= (*ptr++ & 0x3F) << 18;  // 0b00111111
			c32 |= (*ptr++ & 0x3F) << 12;  // 0b00111111
			c32 |= (*ptr++ & 0x3F) << 6;   // 0b00111111
			c32 |=  *ptr++ & 0x3F;         // 0b00111111
			sprintf(buf, "\\u%u ", c32);
			strcat(out, buf);
		}	
		else if (c >= 248){/* 5-bytes */
			c32  = (*ptr++ & 0x3)  << 24;  // 0b00000011
			c32 |= (*ptr++ & 0x3F) << 18;  // 0b00111111
			c32 |= (*ptr++ & 0x3F) << 12;  // 0b00111111
			c32 |= (*ptr++ & 0x3F) << 6;   // 0b00111111
			c32 |=  *ptr++ & 0x3F;         // 0b00111111
			sprintf(buf, "\\u%u ", c32);
			strcat(out, buf);
		}
		else if (c >= 240){/* 4-bytes */
			c32  = (*ptr++ & 0x7)  << 18;  // 0b00000111
			c32 |= (*ptr++ & 0x3F) << 12;  // 0b00111111
			c32 |= (*ptr++ & 0x3F) << 6;   // 0b00111111
			c32 |=  *ptr++ & 0x3F ;        // 0b00111111
			sprintf(buf, "\\u%u ", c32);
			strcat(out, buf);
		}
		else if (c >= 224){/* 3-bytes */
			c32  = (*ptr++ & 0xF)  << 12;  // 0b00001111
			c32 |= (*ptr++ & 0x3F) << 6;   // 0b00111111
			c32 |=  *ptr++ & 0x3F;         // 0b00111111
			sprintf(buf, "\\u%u ", c32);
			strcat(out, buf);
		}
		else if (c >= 192){/* 2-bytes */
			c32  = (*ptr++ & 0x1F) << 6;   // 0b00011111
			c32 |=  *ptr++ & 0x3F;         // 0b00111111
			sprintf(buf, "\\u%u ", c32);
			strcat(out, buf);
		}
		else{/* 1-byte */
			sprintf(buf, "%c", *ptr++);
			strcat(out, buf);
		}
	}
	// add space after end of string
	strcat(out, " ");
	return out;
}

struct _rtf_str{
	char *str;
	int len;
};

static int 
_rtf_str_init(struct _rtf_str *s)
{
	s->len  = 0;
	s->str  = (char*)malloc(1);
	if (s->str == NULL)
		return -1;
	return 0;
}

static void
_rtf_str_append(struct _rtf_str *s, const char *str)
{
	int len = strlen(str);
	int new_size = s->len + len + 1; 
	void *ptr = realloc(s->str, new_size);
	if (ptr == NULL)
		return;
	s->str = (char*)ptr;
	int i;
	for (i = 0; i < len; ++i)
		s->str[s->len++] = str[i];
	s->str[s->len] = 0;
}

#define _rtf_str_appendf(s, ...)\
	({char str[BUFSIZ];sprintf(str, __VA_ARGS__);\
			_rtf_str_append(s, str);});

char *
rtf_table_header(
		int coln, const char *titles[], int *width)
{
	int i, w=0;
	struct _rtf_str s;
	_rtf_str_init(&s);
	
	_rtf_str_append(&s, 
			  "\\pard\\par\\trowd\n");
	for (i = 0; i < coln; ++i) {
		w += width[i];
		_rtf_str_appendf(&s, 
				"\\clbrdrt\\brdrs"
				"\\clbrdrl\\brdrs"
				"\\clbrdrb\\brdrs"
				"\\clbrdrr\\brdrs\n"
				"\\cellx%d\n", 
				w);
	}
	for (i = 0; i < coln; ++i) {
		char *title = rtf_from_utf8(titles[i]);
		_rtf_str_appendf(&s, 
				"\\intbl %s \\cell\n", 
				title);
		free(title);
	}

	return s.str;
}

char *
rtf_table_row(
		int coln, char *colv[])
{
	int i;
	struct _rtf_str s;
	if (_rtf_str_init(&s))
		return NULL;

	_rtf_str_append(&s, 
				"\\trowd\n");
	
	for (i = 0; i < coln; ++i)
		_rtf_str_appendf(&s, 
				"\\intbl %s \\cell\n",
				rtf_from_utf8(colv[i]));
	
	_rtf_str_appendf(&s, 
				"\\row\n");
	
	return s.str;
}

char *
rtf_table_row_from_string(
		const char *colv, const char *delim)
{
	int i;
	struct _rtf_str s;
	if (_rtf_str_init(&s))
		return NULL;

	_rtf_str_append(&s, 
				"\\trowd\n");
	
	// do safe strtok
	char *str = strdup(colv);
	if (str == NULL)
		return NULL;

	// loop through the string to extract 
	// tokens
	char *t;
	for(t=strtok(str, delim), i=0; 
			t; 
			t=strtok(NULL, delim), ++i) 
		_rtf_str_appendf(&s, 
				"\\intbl %s \\cell\n",
				rtf_from_utf8(t));
	
	_rtf_str_appendf(&s, 
				"\\row\n");
	
	free(str);
	return s.str;
}

static unsigned char * _rtf_image_bin_to_strhex(
		const unsigned char *bin,
		unsigned int binsz,
		unsigned char **result)
{
	unsigned char hex_str[] = "0123456789abcdef";
	unsigned int  i;

	if (!binsz)
		return NULL;
	
	if (!(*result = 
				(unsigned char *)malloc(binsz * 2 + 1)))
		return NULL;

	(*result)[binsz * 2] = 0;

	for (i = 0; i < binsz; ++i)
	{
		(*result)[i * 2 + 0] = hex_str[(bin[i] >> 4) & 0x0F];
		(*result)[i * 2 + 1] = hex_str[(bin[i]     ) & 0x0F];
	}

	return (*result);
}

/* convert image to RTF string */
static char *rtf_from_image(
		const char *format, void *data, size_t len,
		int width, int height)
{
	if (!format || !data || !len)
		return NULL;
	
	/* emf, png, jpeg */
	if (strcmp(format, "png") &&
			strcmp(format, "jpeg") &&
			strcmp(format, "emf"))
		return NULL;
	
	int i;
	struct _rtf_str s;
	if (_rtf_str_init(&s))
		return NULL;

	// append image header to rtf
	_rtf_str_appendf(&s, 
			"{\\pict\\picw0\\pich0\\picwgoal%d"
			"\\pichgoal%d\\%sblip\n", width, height, format);
	
	// append image data to rtf
	unsigned char *str;
	_rtf_image_bin_to_strhex(
			(unsigned char *)data,
		len, &str);
	_rtf_str_append(&s, (char*)str);
	free(str);
	
	// append image close to rtf
	_rtf_str_appendf(&s, "}\n");

	return s.str;
}

#endif /* ifndef RTF_H_ */
