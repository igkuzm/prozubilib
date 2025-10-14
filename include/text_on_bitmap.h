#ifndef TEXT_ON_BITMAP_H
#define TEXT_ON_BITMAP_H

#ifdef _MSC_VER
#undef EXPORTDLL
#define EXPORTDLL __declspec(dllexport)
#else
#undef EXPORTDLL
#define EXPORTDLL
#endif

int EXPORTDLL text_on_bitmap(
		unsigned char *bitmap,
		int width, int height, int channels,
		const char *text, int color,
		const char *font, int fsize,
		int x, int y);
#endif /* ifndef TEXT_ON_BITMAP_H */
