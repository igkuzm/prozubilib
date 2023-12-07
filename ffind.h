/**
 * File              : ffind.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.12.2023
 * Last Modified Date: 04.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/* Function to find and replace words in file */
#ifndef F_FIND_H_
#define F_FIND_H_
#include <stdio.h>

/* ffind
 * read text from stream, callback for each word that 
 * starts with one of the markers and write to other 
 * stream.
 * %in   - stream to read
 * %out  - stream to write
 * %size - size of data to read. Set 0 to read till EOF
 * %markers  - null-terminated array of markers
 * %userdata - data to transfer through callback
 * %callback - return non-null to stop function
 */
static void ffind(
		FILE *in, FILE *out, size_t size, char *markers,
		void *userdata,
		int (*callback)(char **found, void *userdata))
{
	int i, ch, stop=0;
	for (ch=getc(in), i=0;
			 ch != EOF && i < size && !stop;
			 ch=getc(in), i++)
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
					ch != ' '  && ch != '\\' && 
					ch != '.'  && ch != '\t' && 
					ch != '{'  && ch != '}'  && 
					ch != '\n' && ch != '\r' && 
					ch != ','  && ch != EOF  &&
					blen < BUFSIZ)
			{
				buf[blen++] = ch;
				ch = fgetc(in);
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
				fputs(word, out);
			
			// return last symbol to out
			fputc(ch, out);
		
		} else
			fputc(ch, out);
	}
}
#endif /* ifndef F_FIND_H_ */
