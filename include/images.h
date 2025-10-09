/**
 * File              : images.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 24.08.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef IMAGES_H
#define IMAGES_H

#include "prozubilib_conf.h"

#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"

//#include "image2ascii.h"

#define IMAGES_TABLENAME "ZIMAGES"

/*
 * IMAGES_COLUMN_DATE(struct member, enum number, SQLite column title)
 * IMAGES_COLUMN_TEXT(struct member, enum number, SQLite column title, size)
 * IMAGES_COLUMN_DATA(struct member, enum number, SQLite column title, type)
 */
#define IMAGES_COLUMNS \
	IMAGES_COLUMN_DATE(date, IMAGEDATE,   "ZLASTMODIFIED")\
	IMAGES_COLUMN_TEXT(title, IMAGETITLE,  "ZTITLE"       )\
	IMAGES_COLUMN_TEXT(caseid, IMAGECASEID, "ZCASEID"      )\
	IMAGES_COLUMN_DATA(data, IMAGEDATA,   "ZIMAGEDATA", void)

#define IMAGES_DATA_TYPES\
	IMAGES_DATA_TYPE(void)

enum image_data_types {
#define IMAGES_DATA_TYPE(type) IMAGES_DATA_TYPE_##type, 
	IMAGES_DATA_TYPES
#undef IMAGES_DATA_TYPE

	IMAGES_DATA_TYPES_NUM,
};

struct image_t {
	uuid4_str id;         /* uuid of the image */

#define IMAGES_COLUMN_DATE(member, number, title      )\
	time_t member; 
#define IMAGES_COLUMN_TEXT(member, number, title      )\
	char * member; size_t len_##member; 
#define IMAGES_COLUMN_DATA(member, number, title, type)\
	type * member; size_t len_##member; 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA
};


BEGIN_ENUM(IMAGES) 
#define IMAGES_COLUMN_DATE(member, number, title      )\
		DECL_ENUM_ELEMENT(number), 
#define IMAGES_COLUMN_TEXT(member, number, title      )\
		DECL_ENUM_ELEMENT(number), 
#define IMAGES_COLUMN_DATA(member, number, title, type)\
		DECL_ENUM_ELEMENT(number), 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA

	IMAGES_COLS_NUM,
END_ENUM(IMAGES)

BEGIN_ENUM_STRING(IMAGES) 
#define IMAGES_COLUMN_DATE(member, number, title      )\
		DECL_ENUM_STRING_ELEMENT(number), 
#define IMAGES_COLUMN_TEXT(member, number, title      )\
		DECL_ENUM_STRING_ELEMENT(number), 
#define IMAGES_COLUMN_DATA(member, number, title, type)\
		DECL_ENUM_STRING_ELEMENT(number), 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA
END_ENUM_STRING(IMAGES)	

struct prozubi_image_jpg_write_s {
	void *data;
	size_t len;
	prozubi_t *p;
};
	
extern void	
prozubi_images_table_init(struct kdata2_table **images); 

extern int
prozubi_image_set_image_raw(
		prozubi_t *p,
		struct image_t *i,
		unsigned char * raw_image, 
		int width, int height, int channels);

extern int
prozubi_image_set_image_from_mem(
		prozubi_t *p,
		struct image_t *i,
		void * data, int len);

extern int
prozubi_image_set_image_from_file(
		prozubi_t *p,
		struct image_t *i,
		const char *filename);

#define IMAGES_COLUMN_DATE(member, number, title)\
extern int prozubi_image_set_##number(\
		kdata2_t *p, struct image_t *c, time_t t);

#define IMAGES_COLUMN_DATA(member, number, title, type)\
extern int prozubi_image_set_##number(\
		kdata2_t *p, struct image_t *c,\
	   	void *data, size_t len);

#define IMAGES_COLUMN_TEXT(member, number, title)\
extern int prozubi_image_set_##number(\
		kdata2_t *p, struct image_t *c, const char *text);
		IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT			
#undef IMAGES_COLUMN_DATA			

/* allocate and init new image */
extern struct image_t *
prozubi_image_new(
		prozubi_t *p,
#define IMAGES_COLUMN_DATE(member, number, title      )\
	 	time_t member, 
#define IMAGES_COLUMN_TEXT(member, number, title      )\
	 	const char * member, 
#define IMAGES_COLUMN_DATA(member, number, title, type)\
	 	type * member, size_t len_##member, 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA		
		const char *id
		);

extern struct image_t *
prozubi_image_from_sql(
		prozubi_t *p,
		sqlite3_stmt *stmt);

/* callback all images with case id; set caseid to NULL 
 * to get all images in database */
extern void 
prozubi_image_foreach(
		prozubi_t  *p,
		const char *caseid,
		const char *predicate,
		void       *user_data,
		int        (*callback)(
			void *user_data, struct image_t *i));

extern void
prozubi_image_free(struct image_t *i);

extern int prozubi_image_set_text(
		IMAGES key, 
		kdata2_t *p, 
		struct image_t *c, 
		const char *text);

extern int prozubi_image_set_date(
		IMAGES key, kdata2_t *p, struct image_t *c, time_t t);

extern int prozubi_image_set_data(
		IMAGES key, 
		kdata2_t *p, 
		struct image_t *c, 
		void *data, 
		size_t len);

extern int prozubi_image_remove(
		kdata2_t *p, struct image_t *c);

/* convert image to RTF string */
extern size_t prozubi_image_to_rtf(
		prozubi_t *p, struct image_t *image, char **rtf);

extern void 
_prozubi_image_jpg_write_func(
		void *context, void *data, int size);

#endif /* ifndef IMAGES_H */
