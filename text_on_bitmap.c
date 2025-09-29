#include <stddef.h>
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "utf.h"

int text_on_bitmap(
		unsigned char *bitmap,
		int width, int height, int channels,
		const char *text, int color,
		const char *font, int fsize,
		int x, int y)
{
	stbtt_fontinfo fnt;
  int i,j,ascent,baseline,size, l=0, ch=0;
  float scale, xpos=(float)x;
	unsigned char *buffer, **canvas;
	unsigned int *str;
	char *p;

	FILE *fp = fopen(font, "rb");
	if (!fp)
		return 1;
	
	// get size
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	// read font
	buffer = malloc(size);
	if (buffer == NULL)
		return 1;
	fread(buffer, size, 1, fp);
  if (stbtt_InitFont(
			&fnt, buffer, 0) == 0)
		return 1;

	// create bitmap for text
	canvas = malloc(2*sizeof(char *));
	if (canvas == NULL)
		return 1;
	canvas[0] = malloc(height);
	canvas[1] = malloc(width);
	if (canvas[0] == NULL || canvas[1] == NULL)
		return 1;
	memset(canvas[0], 0, height);
	memset(canvas[2], 0, width);

	scale = stbtt_ScaleForPixelHeight(
			&fnt, fsize);
	stbtt_GetFontVMetrics(&fnt, &ascent,
			0,0);
	baseline = (int) (ascent*scale);

	// get unicode
	str = malloc((strlen(text)+1)*sizeof(int));
	if (str == NULL)
		return 1;
	p = (char *)text;
	while (*p)
		p = mbtoc32(&str[l++], p);
	str[l] = 0;

	// print for each char
	while (str[ch]) {
      int advance,lsb,x0,y0,x1,y1;
      float x_shift = xpos - (float) floor(xpos);
      stbtt_GetCodepointHMetrics(
					&fnt, 
					str[ch], 
					&advance, 
					&lsb);
      stbtt_GetCodepointBitmapBoxSubpixel(
					&fnt, 
					str[ch], 
					scale,
					scale,
					x_shift,
					y, 
					&x0,&y0,
					&x1,&y1);
      stbtt_MakeCodepointBitmapSubpixel(
					&fnt, 
					&canvas[baseline + y0][(int) xpos + x0], 
					x1-x0, y1-y0, width, 
					scale, scale,
					x_shift, y, str[ch]);
      // note that this stomps the old data, so where character boxes overlap (e.g. 'lj') it's wrong
      // because this API is really for baking character bitmaps into textures. if you want to render
      // a sequence of characters, you really need to render each bitmap to a temp buffer, then
      // "alpha blend" that into the working buffer
      xpos += (advance * scale);
      if (str[ch+1])
         xpos += 
					 scale*stbtt_GetCodepointKernAdvance(
							 &fnt, 
							 str[ch],
							 str[ch+1]);
      ++ch;
	}
	for (j = 0; j < height; ++j) {
		for (i = 0; i < width; ++i) {
			// get letter pixel
			if (canvas[j][i] > 1){
				*(int*)&(bitmap[j*width*channels + i*channels]) = color;
				bitmap[j*width*channels + i*channels + channels - 1] 
					= canvas[j][i];
			}
		}
	}

	free(buffer);
	free(canvas[0]);
	free(canvas[1]);
	free(canvas);
	free(str);

	return 0;
}
