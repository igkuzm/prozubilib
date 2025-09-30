/**
 * File              : image2ascii.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 17.05.2023
 * Last Modified Date: 26.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef IMAGE_TO_ASCII_H
#define IMAGE_TO_ASCII_H

#include <stdarg.h>
#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"

static char _igm2ascii_buf[BUFSIZ];

static char *_STR(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsprintf(_igm2ascii_buf, fmt, args);
	va_end(args);
	return _igm2ascii_buf;
}

/* 
 * image2ascii
 * return length of allocated ascii c string 
 * (with cols and rows) converted from 
 * image data (jpg, png, gif, bmp ...)
 * @image_data - pointer to image data
 * @len - size of image data
 * @cols - number of columns of ascii string 
 * (0 - to have original width of image)
 * @rows - number of rows of ascii string 
 * (0 to have original resolution)
 * @ascii - pointer to ascii string
 */
static size_t
image2ascii(
		unsigned char * image_data,
		size_t len,
		int cols,
		int rows,
		char **ascii,
		void *userdata,
		void (*on_error)(void *userdata, const char * error)
		)
{
	const char map[11] = " .,:;ox%#@";// map of image convert
	stbi_uc *image, *buf, *p, *pg, *ptr;             
	int w, h, c, x, y, index, gc;
	size_t i = 0;
	
	/*read image */
	image = stbi_load_from_memory(
			image_data, len, &w, &h,
			&c, 0);
	if (!image){
		if (on_error)
			on_error(userdata, 
					_STR(
						"can't get image from data:"
						" %p with len %ld", image_data, len));
		return -1;
	}

	/* resize image */
	if (cols < 1)
		cols = w;
	if (rows < 1)
		rows = h * cols / w;

	buf = (stbi_uc *)malloc(cols*rows*c);
	if (!buf){
		if (on_error)
			on_error(userdata, 
				_STR("can't allocate buffer with size: %d", 
				cols * rows * c)); 
		return -1;
	}

	stbir_resize_uint8(image, w,
			h, 0,
			buf, cols,
			rows, 0, 
			c);
	stbi_image_free(image);	

	// set image to gray 
	gc = c == 4 ? 2 : 1; // set channels for gray
	image = (stbi_uc *)malloc(cols*rows*gc);
	if (!image){
		if (on_error)
			on_error(userdata, 
				_STR("can't allocate image with size: %d", 
				cols * rows * gc)); 
		return -1;
	}
	for (p  = buf, pg = image; 
			 p != buf + (cols * rows * c);
			 p += c, pg += gc)
	{
		*pg = (uint8_t)((*p + *(p + 1) + *(p + 2)) / 3.0);
		if (c == 4)
			*(pg + 1) = *(p + 3);
	}
	stbi_image_free(buf);	

	//convert to ascii 
	buf = 
		(stbi_uc *)malloc((cols * rows * 2) + rows + 1);
	if (!buf){
		if (on_error)
			on_error(userdata, 
				_STR("can't allocate buffer with size: %d", 
				(cols * rows * 2) + rows + 1)); 
		return -1;
	}
	ptr = image;
	for (y = 0; y < rows; y++)
	{
			for (x = 0; x < cols; x++)
			{
				index = 
					(int)(*(ptr) / 
							(255 / (sizeof(map) / sizeof(map[0]))));
				index > 9 ? index = 9 : 1;
				index < 0 ? index = 0 : 1;
				buf[i++] = map[index];
				buf[i++] = map[index];
				ptr++;
			}
			buf[i++] = '\n';
	}
	buf[i] = '\0';

	free(image);
	
	if (ascii)
		*ascii = (char *)buf;

	return i;
}

#endif /* ifndef IMAGE_TO_ASCII_H */
