#ifndef TEXT_ON_BITMAP_H
#define TEXT_ON_BITMAP_H
int text_on_bitmap(
		unsigned char *bitmap,
		int width, int height, int channels,
		const char *text, int color,
		const char *font, int fsize,
		int x, int y);
#endif /* ifndef TEXT_ON_BITMAP_H */
