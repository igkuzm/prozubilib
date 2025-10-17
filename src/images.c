/**
 * File              : images.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 24.08.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/images.h"
#include <tiffio.h>

 void	
prozubi_images_table_init(struct kdata2_table **images){
	kdata2_table_init(images, IMAGES_TABLENAME,

#define IMAGES_COLUMN_DATE(member, number, title      )\
	KDATA2_TYPE_NUMBER, title, 
#define IMAGES_COLUMN_TEXT(member, number, title      )\
	KDATA2_TYPE_TEXT, title, 
#define IMAGES_COLUMN_DATA(member, number, title, type)\
	KDATA2_TYPE_DATA, title, 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA
			
			NULL); 
} 

 void 
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
	memcpy(&(((char *)s->data)[s->len]),
			data, size);
	s->len += size;
}

 int
prozubi_image_set_image_raw(
		prozubi_t *p,
		struct image_t *i,
		unsigned char * raw_image, 
		int width, int height, int channels)
{
	int res;
	struct prozubi_image_jpg_write_s s;
	s.data = malloc(1);
	if (!s.data){
		perror("allocate");
		exit(EXIT_FAILURE);
	}
	s.len = 0;
	s.p = p;

	res = stbi_write_jpg_to_func(
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
				STR("set image data with %ld bytes", 
					i->len_data));

	if (!kdata2_set_data_for_uuid(
				p, IMAGES_TABLENAME, 
				"ZIMAGEDATA", i->data, 
				i->len_data, i->id))
		return -1;
	return 0;
}

 int
prozubi_image_set_image_from_mem(
		prozubi_t *p,
		struct image_t *i,
		void * data, int len)
{
	int w, h, c, ret;
	stbi_uc *image; 

	if (!p)
		return -1;
	
	if (!i)
		return -1;
	
	image = 
			stbi_load_from_memory((stbi_uc *)data, len,
					&w, &h, &c,
					0);
	if (!image){
		if (p->on_error)
			p->on_error(p->on_error_data,			
		STR_ERR("%s", "can't open image"));
		return -1;
	}
	
	ret = prozubi_image_set_image_raw(p, i, 
			image, w, h, c);

	stbi_image_free(image);
	return ret;
}

 int
prozubi_image_set_image_from_file(
		prozubi_t *p,
		struct image_t *i,
		const char *filename)
{
	// try to load file
	int w=0, h=0, c, ret;
	stbi_uc *image = 
		stbi_load(filename, &w, &h, &c,
				0);

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
								tif, w, h, (unsigned int *)image, 0))
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

	ret = prozubi_image_set_image_raw(
			p, i, image, w, h, 
			c);
	stbi_image_free(image);
	return ret;
}

static struct image_t *_prozubi_image_new(
		prozubi_t *p)
{
	/* allocate image_t */
	struct image_t *i = NEW(struct image_t);
	if (i == NULL){
		if (p->on_error)
				p->on_error(p->on_error_data,			
			STR_ERR("%s", 
				"can't allocate struct image_t")); 
		return NULL;
	} 

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

#define IMAGES_COLUMN_DATE(member, number, title)\
 int prozubi_image_set_##number(\
		kdata2_t *p, struct image_t *c, time_t t){\
	if (!kdata2_set_number_for_uuid(p, IMAGES_TABLENAME,\
			 	title, t, c->id))\
		return -1;\
	c->member = t;\
	return 0;\
}
#define IMAGES_COLUMN_DATA(member, number, title, type)\
 int prozubi_image_set_##number(\
		kdata2_t *p, struct image_t *c,\
	   	void *data, size_t len)\
{\
	if (IMAGES_DATA_TYPE_##type == IMAGES_DATA_TYPE_void){\
		if (!kdata2_set_data_for_uuid(p, IMAGES_TABLENAME,\
				 	title, data, len, c->id))\
			return -1;\
		if(c->member)\
			free(c->member);\
		c->member = MALLOC(len); \
		if (c->member == NULL){ \
			if (p->on_error)\
				p->on_error(p->on_error_data,\
				STR_ERR(\
					"can't allocate size: %ld"\
					, len)); \
			return -1;\
		} \
		memcpy(c->member, data, len);\
		c->len_##member = len;\
	}\
	return 0;\
}
#define IMAGES_COLUMN_TEXT(member, number, title)\
 int prozubi_image_set_##number(\
		kdata2_t *p, struct image_t *c, const char *text){\
	size_t len; \
	if (!kdata2_set_text_for_uuid(p, IMAGES_TABLENAME,\
			 	title, text, c->id)) \
		return -1; \
	if(c->member) \
		free(c->member); \
	len = strlen(text); \
  c->member = (char *)MALLOC(len + 1); \
	if (c->member == NULL){ \
		if (p->on_error)\
				p->on_error(p->on_error_data,\
			STR_ERR(\
				"can't allocate size: %ld"\
				, len + 1)); \
		return -1; \
	} \
	strncpy(c->member, text, len);\
	c->len_##member = len;\
	return 0;\
}
		IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT			
#undef IMAGES_COLUMN_DATA			

/* allocate and init new image */
 struct image_t *
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
	struct image_t *i;

	if (!p)
		return NULL;

	/* allocate image_t */
	i = _prozubi_image_new(p);
	if (!i)
		return NULL;

	if (!id){
		/* create new uuid */
		uuid4_init();
		uuid4_generate(i->id);
	} else
		strcpy(i->id, id);

		/* set values */
#define IMAGES_COLUMN_DATE(member, number, title)\
	prozubi_image_set_##number(p, i, member);

#define IMAGES_COLUMN_TEXT(member, number, title)\
	prozubi_image_set_##number(p, i, member);

#define IMAGES_COLUMN_DATA(member, number, title, type)\
	prozubi_image_set_##number(p, i, member, len_##member);

	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA
	
	return i;
}

 struct image_t *
prozubi_image_from_sql(
		prozubi_t *p,
		sqlite3_stmt *stmt)
{
	int i;
	const unsigned char *value;
	/* allocate image_t */	
	struct image_t * image = 
		_prozubi_image_new(p);
	if (!image)
		return NULL;
	
	/* iterate columns */
	for (i = 0; i < IMAGES_COLS_NUM; ++i) {
		/* handle values */
		switch (i) {

#define IMAGES_COLUMN_DATE(member, number, title) \
			case number:\
			{\
				int col_type = sqlite3_column_type(stmt, i);\
				image->member = 0;\
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
					char *str = (char *)MALLOC(len + 1);\
					if (str == NULL){ \
					if (p->on_error)\
							p->on_error(p->on_error_data,\
						STR_ERR(\
							"can't allocate string with len: %ld",\
						 	len+1)); break;\
					}	\
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
					void *data = MALLOC(len);\
					if (data == NULL){ \
						if (p->on_error)\
							p->on_error(p->on_error_data,\
						STR_ERR(\
							"can't allocate data with len: %ld",\
						 	len)); break;\
					} \
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
	value = 
		sqlite3_column_text(stmt, i);				
	strncpy(image->id, (const char *)value, 
			sizeof(image->id) - 1);
	image->id[sizeof(image->id) - 1] = 0;		

	return image;
}

/* callback all images with case id; set caseid to NULL 
 * to get all images in database */
 void 
prozubi_image_foreach(
		prozubi_t  *p,
		const char *caseid,
		const char *predicate,
		void       *user_data,
		int        (*callback)(
			void *user_data, struct image_t *i))
{
	int res;
	sqlite3_stmt *stmt;
	char SQL[2*BUFSIZ] = "SELECT ";

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

 void
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

 int prozubi_image_set_text(
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

 int prozubi_image_set_date(
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

 int prozubi_image_set_data(
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

 int prozubi_image_remove(
		kdata2_t *p, struct image_t *c)
{
	return kdata2_remove_for_uuid(p,
			IMAGES_TABLENAME, c->id);
}

static unsigned char * _prozubi_image_bin_to_strhex(
		const unsigned char *bin,
		unsigned int binsz,
		unsigned char **result)
{
	unsigned char hex_str[] = "0123456789abcdef";
	unsigned int  i;

	if (!binsz)
		return NULL;
	
	if (!(*result = 
				(unsigned char *)malloc(binsz * 2 + 1)))
		return NULL;

	(*result)[binsz * 2] = 0;

	for (i = 0; i < binsz; ++i)
	{
		(*result)[i * 2 + 0] = hex_str[(bin[i] >> 4) & 0x0F];
		(*result)[i * 2 + 1] = hex_str[(bin[i]     ) & 0x0F];
	}

	return (*result);
}

/* convert image to RTF string */
 size_t prozubi_image_to_rtf(
		prozubi_t *p, struct image_t *image, char **rtf)
{
	int x, y, c;
	unsigned char *str;
	struct str s;
	if (str_init(&s))
	{
		if (p->on_error)
			p->on_error(p->on_error_data,
					STR("can't allocate memory"));
		return 0;
	}

	// try to load image
	if (!stbi_info_from_memory(
				(stbi_uc*)image->data, 
				image->len_data, &x, &y, &c))
	{
		if (p->on_error)
			p->on_error(p->on_error_data,
					STR("can't load image: %s", image->id));
		return 0;
	}

	// append image header to rtf
	str_appendf(&s, 
			"{\\pict\\picw0\\pich0\\picwgoal10254"
			"\\pichgoal6000\\jpegblip\n");
	
	// append image data to rtf
	_prozubi_image_bin_to_strhex(
		(unsigned char*)(image->data),
		image->len_data, &str);
	str_append(
			&s, (char*)str, image->len_data*2);
	free(str);
	
	// append image close to rtf
	str_appendf(&s, "}\n");

	if (rtf)
		*rtf = s.str;
	else
		free(s.str);
	
	return s.len;
}

