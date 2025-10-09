/**
 * File              : rtf.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.12.2023
 * Last Modified Date: 03.10.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/* functions to handle RTF files */

/* IMPLIMATION */
#include "../include/rtf.h"

char *
rtf_from_utf8(const char *s)
{
	size_t len;
	char *out, *ptr, buf[8] = "";
	unsigned int c32;

	if (!s)
		return NULL;
	
	len = strlen(s);
	out = (char *)malloc(len * 6 + 1);
	if (!out)
		return NULL;
	strcpy(out, "\\uc0 ");

	ptr = (char *)s;
	while(*ptr){
		// get char
		unsigned char c = *ptr;
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

static void _rtf_table_add_row_column(
		struct str *s,
		int width_current,
		int width_total,
		const char *value)
{
		str_appendf(s, 
				"\\clbrdrt\\brdrs"
				"\\clbrdrl\\brdrs"
				"\\clbrdrb\\brdrs"
				"\\clbrdrr\\brdrs\n"
				"\\clwWidth%d\\clftsWidth3"
				"\\cellx%d\n", 
				width_current, width_total);		

		str_appendf(s, 
				"\\intbl %s \\cell\n",
				rtf_from_utf8(value));	
}

char *
rtf_table_row(
		int colc, const char *colv[], int *width)
{
	int i, w=0;
	struct str s;
	if (str_init(&s))
		return NULL;

	str_appendf(&s, 
				"\\trowd\n");
	
	for (i = 0; i < colc; ++i)
	{
		w += width[i];
		_rtf_table_add_row_column(
				&s, width[i], w, colv[i]);
	}
	
	 str_appendf(&s, 
				"\\row\n");
	
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
 char *rtf_from_image(
		const char *format, void *data, size_t len,
		int width, int height)
{
	struct str s;
	unsigned char *str;
	
	if (!format || !data || !len)
		return NULL;
	
	/* emf, png, jpeg */
	if (strcmp(format, "png") &&
			strcmp(format, "jpeg") &&
			strcmp(format, "emf"))
		return NULL;
	
	if (str_init(&s))
		return NULL;

	// append image header to rtf
	str_appendf(&s, 
			"{\\pict\\picw0\\pich0\\picwgoal%d"
			"\\pichgoal%d\\%sblip\n", width, height, format);
	
	// append image data to rtf
	_rtf_image_bin_to_strhex(
			(unsigned char *)data,
		len, &str);
	str_append(&s, (char*)str, strlen((char *)str));
	free(str);
	
	// append image close to rtf
	str_appendf(&s, "}\n");

	return s.str;
}
