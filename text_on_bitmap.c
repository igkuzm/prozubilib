#include <stddef.h>
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "utf.h"

unsigned char buffer[24<<20];

int text_on_bitmap(
		unsigned char *bitmap,
		int width, int height, int channels,
		const char *text, int color,
		const char *font, int fsize,
		int x, int y)
{
	stbtt_fontinfo fnt;
  int i,j,ascent,baseline,ch=0;
  float scale, xpos=x;

	FILE *fp = fopen(font, "rb");
	if (!fp)
		return 1;
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(buffer, 1, 1000000, fp);
  if (stbtt_InitFont(
			&fnt, buffer, 0) == 0)
		return 1;

	unsigned char canvas[height][width];
	memset(canvas, 0, width * height);

	scale = stbtt_ScaleForPixelHeight(
			&fnt, fsize);
	stbtt_GetFontVMetrics(&fnt, &ascent,
			0,0);
	baseline = (int) (ascent*scale);

	// get unicode
	unsigned int str[strlen(text)+1];
	mbtoc32(str, text);

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
				bitmap[j*width*channels + i*channels + 3] = canvas[j][i];
			}
		}
	}

	return 0;
}
