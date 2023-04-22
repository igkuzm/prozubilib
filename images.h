/**
 * File              : images.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 21.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef IMAGES_H
#define IMAGES_H

#include "prozubilib_conf.h"

#include <string.h>
#include <stdlib.h>

#define IMAGES_TABLENAME "ZIMAGES"

/*
 * IMAGES_COLUMN_DATE(struct member, enum number, SQLite column title)
 * IMAGES_COLUMN_TEXT(struct member, enum number, SQLite column title, size)
 * IMAGES_COLUMN_DATA(struct member, enum number, SQLite column title, type)
 */
#define IMAGES_COLUMNS \
	IMAGES_COLUMN_DATE(date,   IMAGEDATE,   "ZLASTMODIFIED"   )\
	IMAGES_COLUMN_TEXT(title,  IMAGETITLE,  "ZTITLE",      128)\
	IMAGES_COLUMN_TEXT(caseid, IMAGECASEID, "ZCASEID",      37)\
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

#define IMAGES_COLUMN_DATE(member, number, title) time_t member; 
#define IMAGES_COLUMN_TEXT(member, number, title, size) char member[size]; 
#define IMAGES_COLUMN_DATA(member, number, title, type) type * member; size_t len_##member; 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA
};


enum images_column_number {
#define IMAGES_COLUMN_DATE(member, number, title) number, 
#define IMAGES_COLUMN_TEXT(member, number, title, size) number, 
#define IMAGES_COLUMN_DATA(member, number, title, type) number, 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA

	IMAGES_COLS_NUM,
};

static void	
prozubi_images_table_init(struct kdata2_table **images){
	kdata2_table_init(images, IMAGES_TABLENAME,

#define IMAGES_COLUMN_DATE(member, number, title) KDATA2_TYPE_NUMBER, title, 
#define IMAGES_COLUMN_TEXT(member, number, title, size) KDATA2_TYPE_TEXT, title, 
#define IMAGES_COLUMN_DATA(member, number, title, type) KDATA2_TYPE_DATA, title, 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA
			
			NULL); 
} 

/* allocate and init new case */
static struct image_t *
prozubi_image_new(
		kdata2_t *kdata,
#define IMAGES_COLUMN_DATE(member, number, title) 
#define IMAGES_COLUMN_TEXT(member, number, title, size) const char * member, 
#define IMAGES_COLUMN_DATA(member, number, title, type) type * member, size_t len_##member, 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA		
		const char *id
		)
{
	/* allocate case_t */
	struct image_t *i = malloc(sizeof(struct image_t));
	if (!i){
		ERR("%s", "can't allocate struct image_t");
		return NULL;
	}

	if (!id){
		/* create new uuid */
		UUID4_STATE_T state; UUID4_T identifier;
		uuid4_seed(&state);
		uuid4_gen(&state, &identifier);
		if (!uuid4_to_s(identifier, i->id, 37)){
			ERR("%s", "can't generate uuid");
			return NULL;
		}
	} else
		strcpy(i->id, id);

	/* set values */
#define IMAGES_COLUMN_DATE(member, number, title)\
   	kdata2_set_number_for_uuid(kdata, IMAGES_TABLENAME, title, time(NULL), i->id); 
#define IMAGES_COLUMN_TEXT(member, number, title, size)\
   	if (member)\
		kdata2_set_text_for_uuid(kdata, IMAGES_TABLENAME, title, member, i->id);	
#define IMAGES_COLUMN_DATA(member, number, title, type)\
   	if (member)\
		kdata2_set_data_for_uuid(kdata, IMAGES_TABLENAME, title, (void *)member, len_##member, i->id);	
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT
#undef IMAGES_COLUMN_DATA
	
	return i;
}

/* callback all images with case id; set caseid to NULL to get all images in database */
static void 
prozubi_image_foreach(
		kdata2_t   *kdata,
		const char *caseid,
		void       *user_data,
		int        *(callback)(void *user_data, struct image_t *i)
		)
{
	/* check kdata */
	if (!kdata){
		ERR("%s", "kdata is NULL");
		return;
	}
	if (!kdata->db){
		ERR("%s", "kdata->db is NULL");
		return;
	}

	struct image_t image;
	
	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define IMAGES_COLUMN_DATE(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
#define IMAGES_COLUMN_TEXT(member, number, title, size) strcat(SQL, title); strcat(SQL, ", "); 
#define IMAGES_COLUMN_DATA(member, number, title, type) strcat(SQL, title); strcat(SQL, ", "); 
	IMAGES_COLUMNS
#undef IMAGES_COLUMN_DATE
#undef IMAGES_COLUMN_TEXT			
#undef IMAGES_COLUMN_DATA			
	
	strcat(SQL, "RECORDNAME FROM ");
	strcat(SQL, IMAGES_TABLENAME);
	if (caseid)
		strcat(SQL, STR(" WHERE ZCASEID = '%s'", caseid));

	/* start SQLite request */
	int res;
	sqlite3_stmt *stmt;
	
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db));	
		return;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
		/* iterate columns */
		int i;
		for (i = 0; i < IMAGES_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define IMAGES_COLUMN_DATE(member, number, title) \
				case number:\
				{\
					int col_type = sqlite3_column_type(stmt, i);\
					if (col_type == SQLITE_INTEGER) {\
						image.member = sqlite3_column_int64(stmt, i);\
					} else if (col_type == SQLITE_FLOAT) {\
						image.member = sqlite3_column_double(stmt, i) + NSTimeIntervalSince1970;\
					}\
					break;\
				}; 
				
#define IMAGES_COLUMN_TEXT(member, number, title, size) \
				case number:\
				{\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					strncpy(image.member, (const char *)value, size - 1);\
					image.member[size - 1] = 0;\
					break;\
				}; 

#define IMAGES_COLUMN_DATA(member, number, title, type) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const void *value = sqlite3_column_blob(stmt, i);\
					if (IMAGES_DATA_TYPE_##type == IMAGES_DATA_TYPE_void){\
						image.member = (void *)value;\
						image.len_##member = len;\
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
		const unsigned char *value = sqlite3_column_text(stmt, i);				
		strncpy(image.id, (const char *)value, sizeof(image.id) - 1);
		image.id[sizeof(image.id) - 1] = 0;		

		/* callback */
		if (callback)
			if (callback(user_data, &image))
				break;		
	}	

	sqlite3_finalize(stmt);
}

#endif /* ifndef IMAGES_H */
