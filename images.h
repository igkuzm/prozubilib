/**
 * File              : images.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 13.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef IMAGES_H
#define IMAGES_H

#include "prozubilib_conf.h"

#include "enum.h"
#include "alloc.h"

#include <string.h>
#include <stdlib.h>
#include <tiffio.h>

#include "stb_image.h"
#include "stb_image_resize.h"
#include "stb_image_write.h"

#include "image2ascii.h"
#include "str.h"

#define IMAGES_TABLENAME "ZIMAGES"

/*
 * IMAGES_COLUMN_DATE(struct member, enum number, SQLite column title)
 * IMAGES_COLUMN_TEXT(struct member, enum number, SQLite column title, size)
 * IMAGES_COLUMN_DATA(struct member, enum number, SQLite column title, type)
 */
#define IMAGES_COLUMNS \
	IMAGES_COLUMN_DATE(date,   IMAGEDATE,   "ZLASTMODIFIED"   )\
	IMAGES_COLUMN_TEXT(title,  IMAGETITLE,  "ZTITLE"          )\
	IMAGES_COLUMN_TEXT(caseid, IMAGECASEID, "ZCASEID"         )\
	IMAGES_COLUMN_DATA(data,   IMAGEDATA,   "ZIMAGEDATA", void)

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

#define IMAGES_COLUMN_DATE(member, number, title      ) time_t member; 
#define IMAGES_COLUMN_TEXT(member, number, title      ) char * member; size_t len_##member; 
#define IMAGES_COLUMN_DATA(member, number, title, type) type * member; size_t len_##member; 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA
};


BEGIN_ENUM(IMAGES) 
#define IMAGES_COLUMN_DATE(member, number, title      ) DECL_ENUM_ELEMENT(number), 
#define IMAGES_COLUMN_TEXT(member, number, title      ) DECL_ENUM_ELEMENT(number), 
#define IMAGES_COLUMN_DATA(member, number, title, type) DECL_ENUM_ELEMENT(number), 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA

	IMAGES_COLS_NUM,
END_ENUM(IMAGES)

BEGIN_ENUM_STRING(IMAGES) 
#define IMAGES_COLUMN_DATE(member, number, title      ) DECL_ENUM_STRING_ELEMENT(number), 
#define IMAGES_COLUMN_TEXT(member, number, title      ) DECL_ENUM_STRING_ELEMENT(number), 
#define IMAGES_COLUMN_DATA(member, number, title, type) DECL_ENUM_STRING_ELEMENT(number), 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA
END_ENUM_STRING(IMAGES)	

static void	
prozubi_images_table_init(struct kdata2_table **images){
	kdata2_table_init(images, IMAGES_TABLENAME,

#define IMAGES_COLUMN_DATE(member, number, title      ) KDATA2_TYPE_NUMBER, title, 
#define IMAGES_COLUMN_TEXT(member, number, title      ) KDATA2_TYPE_TEXT, title, 
#define IMAGES_COLUMN_DATA(member, number, title, type) KDATA2_TYPE_DATA, title, 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA
			
			NULL); 
} 

struct prozubi_image_jpg_write_s {
	void *data;
	size_t len;
	prozubi_t *p;
};

static void 
_prozubi_image_jpg_write_func(
		void *context, void *data, int size)
{
	struct prozubi_image_jpg_write_s *s = 
		(struct prozubi_image_jpg_write_s *)context; 
	
	// realloc data
	s->data = realloc(s->data, s->len + size);
	if (!s->data){
		if (s->p->on_error)
			s->p->on_error(s->p->on_log_data,
				"can't realloc");
		return;
	}
	
	// copy
	memcpy(&(((char *)s->data)[s->len]), data, size);
	s->len += size;
}

static int
prozubi_image_set_image_raw(
		prozubi_t *p,
		struct image_t *i,
		unsigned char * raw_image, 
		int width, int height, int channels)
{
	struct prozubi_image_jpg_write_s s;
	s.data = malloc(1);
	if (!s.data){
		perror("allocate");
		exit(EXIT_FAILURE);
	}
	s.len = 0;
	s.p = p;

	int res = stbi_write_jpg_to_func(
			_prozubi_image_jpg_write_func,
			&s, 
			width, height, channels, 
			raw_image, 80); 

	if (res == 0){
		if (p->on_error)
			p->on_error(p->on_error_data,			
		STR_ERR("%s", "can't open image"));
		return -1;
	}

	if (i->data)
		free(i->data);
	i->data = s.data;
	i->len_data = s.len;

	if (p->on_log)
		p->on_log(p->on_log_data,
				STR("set image data with %ld bytes", i->len_data));

	if (!kdata2_set_data_for_uuid(p, IMAGES_TABLENAME, 
				"ZIMAGEDATA", i->data, i->len_data, i->id))
		return -1;
	return 0;
}

static int
prozubi_image_set_image_from_mem(
		prozubi_t *p,
		struct image_t *i,
		void * data, int len)
{
	if (!p)
		return -1;
	
	if (!i)
		return -1;
	
	int w, h, c;
	stbi_uc *image = 
			stbi_load_from_memory(data, len, &w, &h, &c, 0);
	if (!image){
		if (p->on_error)
			p->on_error(p->on_error_data,			
		STR_ERR("%s", "can't open image"));
		return -1;
	}
	
	int ret = prozubi_image_set_image_raw(
			p, i, image, w, h, c);

	stbi_image_free(image);
	return ret;
}

static int
prozubi_image_set_image_from_file(
		prozubi_t *p,
		struct image_t *i,
		const char *filename)
{
	// try to load file
	int w=0, h=0, c;
	stbi_uc *image = 
		stbi_load(filename, &w, &h, &c, 0);

	if (!image){
		// maybe TIFF?
		TIFF *tif =
			TIFFOpen(filename, "r");
		if (tif){
			c = 4;
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
			if (w > 1 && h > 1){
				image = (unsigned char *)_TIFFmalloc(w * h * w);
				if (image){
					if (!TIFFReadRGBAImage(
								tif, w, h, (uint32_t *)image, 0))
					{
						free(image);
						image = NULL;
					}
				}
			}
		}
	}
	if (!image){
		if (p->on_error)
			p->on_error(p->on_error_data,			
				STR_ERR("%s", "can't open image"));
		return -1;
	}

	int ret = prozubi_image_set_image_raw(
			p, i, image, w, h, c);
	stbi_image_free(image);
	return ret;
}

static struct image_t *_prozubi_image_new(
		prozubi_t *p)
{
	/* allocate image_t */
	struct image_t *i = NEW(struct image_t, 
			if (p->on_error)
				p->on_error(p->on_error_data,			
			STR_ERR("%s", "can't allocate struct image_t")), 
				return NULL);

//set values to NULL
#define IMAGES_COLUMN_DATE(member, number, title      ) 
#define IMAGES_COLUMN_TEXT(member, number, title      )\
 	i->member = NULL; 
#define IMAGES_COLUMN_DATA(member, number, title, type)\
 	i->member = NULL; 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA		
	
	return i;
}

/* allocate and init new image */
static struct image_t *
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
		)
{
	if (!p)
		return NULL;

	/* allocate image_t */
	struct image_t *i = _prozubi_image_new(p);
	if (!i)
		return NULL;

	if (!id){
		/* create new uuid */
		UUID4_STATE_T state; UUID4_T identifier;
		uuid4_seed(&state);
		uuid4_gen(&state, &identifier);
		if (!uuid4_to_s(identifier, i->id, 37)){
			if (p->on_error)
				p->on_error(p->on_error_data,			
			STR_ERR("%s", "can't generate uuid"));
			return NULL;
		}
	} else
		strcpy(i->id, id);

		/* set values */
#define IMAGES_COLUMN_DATE(member, number, title)\
   	kdata2_set_number_for_uuid(p, IMAGES_TABLENAME,\
			 	title, member, i->id);\
	i->member = member;
#define IMAGES_COLUMN_TEXT(member, number, title)\
   	if (member){\
		kdata2_set_text_for_uuid(p, IMAGES_TABLENAME,\
			 	title, member, i->id);\
		size_t len = strlen(member);\
		i->member = (char *)MALLOC(len + 1,\
				if (p->on_error)\
					p->on_error(p->on_error_data,\
				STR_ERR(\
					"can't allocate size: %ld"\
					, len + 1)), return NULL);\
		strncpy(i->member, member, len);\
		i->len_##member = len;\
	}
#define IMAGES_COLUMN_DATA(member, number, title, type)\
	if (member){\
		if (!prozubi_image_set_image_from_mem(p, i, \
					data, len_data)){\
			kdata2_set_data_for_uuid(p, IMAGES_TABLENAME, \
				title, i->data, i->len_data, i->id);\
		}\
	}

	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA
	
	return i;
}

static struct image_t *
prozubi_image_from_sql(
		prozubi_t *p,
		sqlite3_stmt *stmt)
{
	/* allocate image_t */	
	struct image_t * image = 
		_prozubi_image_new(p);
	if (!image)
		return NULL;
	
	/* iterate columns */
	int i;
	for (i = 0; i < IMAGES_COLS_NUM; ++i) {
		/* handle values */
		switch (i) {

#define IMAGES_COLUMN_DATE(member, number, title) \
			case number:\
			{\
				image->member = 0;\
				int col_type = sqlite3_column_type(stmt, i);\
				if (col_type == SQLITE_INTEGER) {\
					image->member = sqlite3_column_int64(stmt, i);\
				} else if (col_type == SQLITE_FLOAT) {\
					image->member = sqlite3_column_double(stmt, i)\
				 	+ NSTimeIntervalSince1970;\
				}\
				break;\
			}; 
			
#define IMAGES_COLUMN_TEXT(member, number, title) \
			case number:\
			{\
				size_t len = sqlite3_column_bytes(stmt, i);\
				const unsigned char *value =\
			 	sqlite3_column_text(stmt, i);\
				if (value){\
					char *str = (char *)MALLOC(len + 1,\
						if (p->on_error)\
							p->on_error(p->on_error_data,\
						STR_ERR(\
							"can't allocate string with len: %ld",\
						 	len+1)), break);\
					strncpy(str, (const char *)value, len);\
					str[len] = 0;\
					image->member = str;\
					image->len_##member = len;\
				} else {\
					image->member = NULL;\
				}\
				break;\
			}; 

#define IMAGES_COLUMN_DATA(member, number, title, type) \
			case number:\
			{\
				size_t len = sqlite3_column_bytes(stmt, i);\
				const void *value = sqlite3_column_blob(stmt, i);\
				if (\
					IMAGES_DATA_TYPE_##type == IMAGES_DATA_TYPE_void)\
				{\
					void *data = MALLOC(len,\
						if (p->on_error)\
							p->on_error(p->on_error_data,\
						STR_ERR(\
							"can't allocate data with len: %ld",\
						 	len)), break);\
					memcpy(data, value, len);\
					image->member = data;\
					image->len_##member = len;\
				}\
				break;\
			};

		IMAGES_COLUMNS

#undef IMAGES_COLUMN_DATE			
#undef IMAGES_COLUMN_TEXT			
#undef IMAGES_COLUMN_DATA			

			default:
				break;					
		}
	}		

	/* handle image id */
	const unsigned char *value = 
		sqlite3_column_text(stmt, i);				
	strncpy(image->id, (const char *)value, 
			sizeof(image->id) - 1);
	image->id[sizeof(image->id) - 1] = 0;		

	return image;
}

/* callback all images with case id; set caseid to NULL 
 * to get all images in database */
static void 
prozubi_image_foreach(
		prozubi_t  *p,
		const char *caseid,
		const char *predicate,
		void       *user_data,
		int        (*callback)(
			void *user_data, struct image_t *i))
{
	/* check kdata */
	if (!p){
		return;
	}
	if (!p->db){
		if (p->on_error)
			p->on_error(p->on_error_data,		
		STR_ERR("%s", "kdata->db is NULL"));
		return;
	}

	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define IMAGES_COLUMN_DATE(member, number, title      )\
 	strcat(SQL, title); strcat(SQL, ", "); 
#define IMAGES_COLUMN_TEXT(member, number, title      )\
 	strcat(SQL, title); strcat(SQL, ", "); 
#define IMAGES_COLUMN_DATA(member, number, title, type)\
 	strcat(SQL, title); strcat(SQL, ", "); 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT			
#undef IMAGES_COLUMN_DATA			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, IMAGES_TABLENAME);
	strcat(SQL, " ");
	if (caseid)
		strcat(SQL, STR(" WHERE ZCASEID = '%s' ", caseid));
	if (predicate)
		strcat(SQL, predicate);

	/* start SQLite request */
	int res;
	sqlite3_stmt *stmt;
	
	res = sqlite3_prepare_v2(p->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		if (p->on_error)
			p->on_error(p->on_error_data,		
		STR_ERR("sqlite3_prepare_v2: %s: %s", 
			SQL, sqlite3_errmsg(p->db)));	
		return;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
		struct image_t *image =
			prozubi_image_from_sql(p, stmt);
		if (!image)
			continue;
		
		/* callback */
		if (callback)
			if (callback(user_data, image))
				break;		
	}	

	sqlite3_finalize(stmt);
}

static void
prozubi_image_free(struct image_t *i){
	if (i){

#define IMAGES_COLUMN_DATE(member, number, title) 
#define IMAGES_COLUMN_TEXT(member, number, title)\
	 	if(i->member) free(i->member); 
#define IMAGES_COLUMN_DATA(member, number, title, type)\
	 	if(i->member) free(i->member); 
		IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE			
#undef IMAGES_COLUMN_TEXT			
#undef IMAGES_COLUMN_DATA			
		
		free(i);
		i = NULL;
	}
}

#define IMAGES_COLUMN_DATE(member, number, title)\
static int prozubi_image_set_##number(\
		kdata2_t *p, struct image_t *c, time_t t){\
	if (!kdata2_set_number_for_uuid(p, IMAGES_TABLENAME,\
			 	title, t, c->id))\
		return -1;\
	c->member = t;\
	return 0;\
}
#define IMAGES_COLUMN_DATA(member, number, title, type)\
static int prozubi_image_set_##number(\
		kdata2_t *p, struct image_t *c,\
	   	void *data, size_t len)\
{\
	if (IMAGES_DATA_TYPE_##type == IMAGES_DATA_TYPE_void){\
		if (!kdata2_set_data_for_uuid(p, IMAGES_TABLENAME,\
				 	title, data, len, c->id))\
			return -1;\
		if(c->member)\
			free(c->member);\
		c->member = MALLOC(len,\
			if (p->on_error)\
				p->on_error(p->on_error_data,\
				STR_ERR(\
					"can't allocate size: %ld"\
					, len)), return -1);\
		memcpy(c->member, data, len);\
		c->len_##member = len;\
	}\
	return 0;\
}
#define IMAGES_COLUMN_TEXT(member, number, title)\
static int prozubi_image_set_##number(\
		kdata2_t *p, struct image_t *c, const char *text){\
	if (!kdata2_set_text_for_uuid(p, IMAGES_TABLENAME,\
			 	title, text, c->id))\
		return -1;\
	if(c->member)\
		free(c->member);\
	size_t len = strlen(text);\
   	c->member = (char *)MALLOC(len + 1,\
			if (p->on_error)\
				p->on_error(p->on_error_data,\
			STR_ERR(\
				"can't allocate size: %ld"\
				, len + 1)), return -1);\
	strncpy(c->member, text, len);\
	c->len_##member = len;\
	return 0;\
}
		IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT			
#undef IMAGES_COLUMN_DATA			

static int prozubi_image_set_text(
		IMAGES key, 
		kdata2_t *p, 
		struct image_t *c, 
		const char *text)
{
	switch (key) {
#define IMAGES_COLUMN_DATE(member, number, title)\
	 	case number: break;	
#define IMAGES_COLUMN_DATA(member, number, title, type)\
	 	case number: break;	
#define IMAGES_COLUMN_TEXT(member, number, title)\
	 	case number:\
		return prozubi_image_set_##number(p, c, text);\
		
		IMAGES_COLUMNS

#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT			
#undef IMAGES_COLUMN_DATA	
		
		default:
			break;
	}
	return -1;
}

static int prozubi_image_set_date(
		IMAGES key, kdata2_t *p, struct image_t *c, time_t t)
{
	switch (key) {
#define IMAGES_COLUMN_TEXT(member, number, title)\
	 	case number: break;	
#define IMAGES_COLUMN_DATA(member, number, title, type)\
	 	case number: break;	
#define IMAGES_COLUMN_DATE(member, number, title)\
	 	case number:\
		return prozubi_image_set_##number(p, c, t);\
		
		IMAGES_COLUMNS

#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT			
#undef IMAGES_COLUMN_DATA	
		
		default:
			break;
	}
	return -1;
}

static int prozubi_image_set_data(
		IMAGES key, 
		kdata2_t *p, 
		struct image_t *c, 
		void *data, 
		size_t len)
{
	switch (key) {
#define IMAGES_COLUMN_TEXT(member, number, title)\
	 	case number: break;	
#define IMAGES_COLUMN_DATE(member, number, title)\
	 	case number: break;	
#define IMAGES_COLUMN_DATA(member, number, title, type)\
	 	case number:\
		return prozubi_image_set_image_from_mem(\
				p, c, data, len);\
		
		IMAGES_COLUMNS

#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT			
#undef IMAGES_COLUMN_DATA	
		
		default:
			break;
	}
	return -1;
}

static int prozubi_image_remove(
		kdata2_t *p, struct image_t *c)
{
	return kdata2_remove_for_uuid(p, IMAGES_TABLENAME, c->id);
}

/* convert image to RTF string */
static char * prozubi_image_to_rtf(
		prozubi_t *p, struct image_t *image)
{
	struct str s;
	if (str_init(&s, image->len_data * 2 + BUFSIZ )){
		if (p->on_error)
			p->on_error(p->on_error_data,
					STR("can't allocate memory"));
		return NULL;
	}

	// try to load image
	int x, y, c;
	if (!stbi_info_from_memory(image->data, 
				image->len_data, &x, &y, &c))
	{
		if (p->on_error)
			p->on_error(p->on_error_data,
					STR("can't load image: %s", image->id));
		return NULL;
	}

	// append image header to rtf
	str_append(&s, 
			"{\\pict\\picw0\\pich0\\picwgoal10254"
			"\\pichgoal6000\\jpegblip\n");
	
	// append image data to rtf
	int i;
	for (i = 0; i < image->len_data; i++) {
		char bit = ((char *)image->data)[i];
		for (i = 0; i < 2; ++i)	
			str_appendf(&s, "%02x", bit);
	}
	
	// append image close to rtf
	str_append(&s, "}\n");
	
	return s.str;
}

#endif /* ifndef IMAGES_H */
