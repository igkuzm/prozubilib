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
#include "prozubilib_conf.h"


/* rtf_from_utf8
 * return string with rtf code from utf8 multibite 
 * sting or NULL on error */
extern char *
rtf_from_utf8(const char *s);

/* rtf_table_row
 * return string with rtf code of table row
 * or NULL on error
 * %colc   - number of columns
 * %colv - columns values
 * %width  - array of columns width in twips*/
extern char *
rtf_table_row(int colc, const char *colv[], int *width);

/* convert image to RTF string 
 * valid formats: emf, png, jpeg
 * with and height are in twips (inches * 20) - 
 * for A4 13011x16838 */
extern char *
rtf_from_image(
		const char *format, void *data, size_t len,
		int width, int height);

#endif /* ifndef RTF_H_ */
