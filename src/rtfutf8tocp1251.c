#include "../include/rtfutf8tocp1251.h"
#include "../include/translit.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLEN 7

static void parse_buffer(const char *buf, FILE *out)
{
	char *bufp = (char *)buf;
	int translit_table_size = 
		sizeof(cprecode_table) / sizeof(cprecode_t);

	if (bufp[1] == 'u' && bufp[2] > 0x2F && bufp[2] < 0x3A)
	{
		// this is unicode char
    cprecode_t key = {0};
		bufp += 2;
		key.utf_code = atoi(bufp);
		
		// convert unicode to cp1251
		cprecode_t *translit = (cprecode_t *)bsearch(
					&key, cprecode_table,
				 	translit_table_size,
					sizeof(cprecode_t), translit_compare);

		if (translit){
			// put to out
			char str[5];
			sprintf(str, "\\\'%x", translit->cp1251);
			fputs(str, out);
			printf("STR: %s\n", str);
		} else {
			// no cp1251 simbol
			fprintf(stderr, 
					"no cp1251 symbol for unicode: 0x%0.4x\n", key.utf_code);
		}

	} else {
		// put buffer to out 
		fputs(buf, out);
	}
}

int rtfutf8tocp1251(const char *from, const char *to)
{
	char buf[BLEN];
	int i, ch, blen, err = 1, ifbuf = 0;
	FILE *in, *out;

	assert(from);
	assert(to);

	in = fopen(from, "r");
	if (in == NULL)
		goto on_end;

	out = fopen(to, "w");
	if (out == NULL)
		goto on_end;

	while ((ch = fgetc(in)) != EOF) {
		if (ch == '\\'){ // start service word
			// start buffer
			ifbuf = 1;
			blen = 0;
			memset(buf, 0, 7);
			buf[blen++] = ch;
			continue;
		}
		if (ifbuf && ch == 'u'){ // start of unicode
			buf[blen++] = ch;
			continue;
		}
		if (ifbuf && ch > 0x2F && ch < 0x3A){ // numbers
			buf[blen++] = ch;
			continue;
		}

		if (ifbuf){ // end of service word
			ifbuf = 0;
			parse_buffer(buf, out);
		}

		ifbuf = 0;
		fputc(ch, out);
	}

on_end:
	if (err)
		perror("rtfutf8tocp1251");
	if (in)
		fclose(in);
	if (out)
		fclose(out);
	return err;
}
