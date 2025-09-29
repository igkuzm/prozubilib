/**
 * File              : passport.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 07.09.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PASSPORT_H
#define PASSPORT_H

#include "prozubilib_conf.h"

#include "enum.h"
#include "kdata2/cYandexDisk/alloc.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

#define PASSPORT_TABLENAME "ZPASSPORT"

/*
 * PASSPORT_COLUMN_DATE(struct member, enum number, SQLite column title)
 * PASSPORT_COLUMN_TEXT(struct member, enum number, SQLite column title, size)
 */
#define PASSPORT_COLUMNS \
	PASSPORT_COLUMN_TEXT(patientid,   PATIENTID,           "ZPATIENTID"  )\
	PASSPORT_COLUMN_TEXT(familiya,    PASSPORTFAMILIYA,    "ZFAMILIYA"   )\
	PASSPORT_COLUMN_TEXT(imia,        PASSPOTIMIA,         "ZIMIA"       )\
	PASSPORT_COLUMN_TEXT(otchestvo,   PASSPORTOTCHESTVO,   "ZOTCHESTVO"  )\
	PASSPORT_COLUMN_TEXT(address,     PASSPORTADDRESS,     "ZADDRESS"    )\
	PASSPORT_COLUMN_TEXT(document,    PASSPORTDOCUMENT,    "ZDOCUMENT"   )\
	PASSPORT_COLUMN_TEXT(comment,     PASSPORTCOMMENT,     "ZCOMMENT"    )\
	PASSPORT_COLUMN_TEXT(tel,         PASSPORTTEL,         "ZTEL"        )\
	PASSPORT_COLUMN_TEXT(email,       PASSPORTEMAIL,       "ZEMAIL"      )\
	PASSPORT_COLUMN_DATE(dateofbirth, PASSPORTDATEOFBIRTH, "ZDATEOFBIRTH")

struct passport_t {
	uuid4_str id;              /* uuid of the passport (patientid/recordname) */

#define PASSPORT_COLUMN_DATE(member, number, title) time_t member; 
#define PASSPORT_COLUMN_TEXT(member, number, title) char * member; size_t len_##member; 
	PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT
	
};


BEGIN_ENUM(PASSPORT) 
#define PASSPORT_COLUMN_DATE(member, number, title) DECL_ENUM_ELEMENT(number), 
#define PASSPORT_COLUMN_TEXT(member, number, title) DECL_ENUM_ELEMENT(number), 
	PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT	

	PASSPORT_COLS_NUM,
END_ENUM(PASSPORT)

BEGIN_ENUM_STRING(PASSPORT) 
#define PASSPORT_COLUMN_DATE(member, number, title) DECL_ENUM_STRING_ELEMENT(number), 
#define PASSPORT_COLUMN_TEXT(member, number, title) DECL_ENUM_STRING_ELEMENT(number), 
	PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT	
END_ENUM_STRING(PASSPORT)	

static void	
prozubi_passport_table_init(struct kdata2_table **passport){
	kdata2_table_init(passport, PASSPORT_TABLENAME,

#define PASSPORT_COLUMN_DATE(member, number, title) KDATA2_TYPE_NUMBER, title, 
#define PASSPORT_COLUMN_TEXT(member, number, title) KDATA2_TYPE_TEXT, title, 
PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT			
			
			NULL); 
} 

/* do callback for each patient in database */
static void 
prozubi_passport_foreach(
		kdata2_t *kdata,
		const char *predicate,
		void     *user_data,
		int      (*callback)(void *user_data, struct passport_t *p)
		)
{
	int res, i, col_type;
	size_t len;
	sqlite3_stmt *stmt;
	const unsigned char *value;
	char SQL[BUFSIZ] = "SELECT ";
	
	/* check kdata */
	if (!kdata){
		return;
	}
	if (!kdata->db){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,		
		STR_ERR("%s", "kdata->db is NULL"));
		return;
	}

	/* create SQL string */
#define PASSPORT_COLUMN_DATE(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
#define PASSPORT_COLUMN_TEXT(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, PASSPORT_TABLENAME);
	strcat(SQL, " ");
	if (predicate)
		strcat(SQL, predicate);
	strcat(SQL, " ORDER BY ZFAMILIYA ASC, ZIMIA ASC, ZOTCHESTVO ASC");

	/* start SQLite request */
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
		STR_ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db)));	
		return;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
		/* passort struct */
		struct passport_t *p = NEW(struct passport_t);
		if (p == NULL){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,				
					STR_ERR("%s", "can't allocate struct passport_t")); 
			return;
		}	

		/* iterate columns */
		for (i = 0; i < PASSPORT_COLS_NUM; ++i) {

			fprintf(stderr, "data: %s\n", sqlite3_column_text(stmt, i));

			/* cast string */
			switch (i) {
					
#define PASSPORT_COLUMN_DATE(member, number, title) \
				case number:\
				{\
					p->member = 0;\
					col_type = sqlite3_column_type(stmt, i);\
					if (col_type == SQLITE_INTEGER) {\
						p->member = sqlite3_column_int64(stmt, i);\
					} else if (col_type == SQLITE_FLOAT) {\
						p->member = sqlite3_column_double(stmt, i) + NSTimeIntervalSince1970;\
					}\
					break;\
				}; 
				
#define PASSPORT_COLUMN_TEXT(member, number, title) \
				case number:\
				{\
					len = sqlite3_column_bytes(stmt, i);\
					value = sqlite3_column_text(stmt, i);\
					if (value){\
						p->member = strdup((char*)value);\
						p->len_##member = len;\
					} else {\
						p->member = NULL;\
					}\
					break;\
				}; 

			PASSPORT_COLUMNS

#undef PASSPORT_COLUMN_DATE			
#undef PASSPORT_COLUMN_TEXT			

				default:
					break;					
			}
		}

		/* handle passport id */
		value = sqlite3_column_text(stmt, i);				
		strncpy(p->id, (const char *)value, sizeof(p->id) - 1);
		p->id[sizeof(p->id) - 1] = 0;		

		/* callback */
		if (callback)
			if (callback(user_data, p))
				break;
	}	
	
	sqlite3_finalize(stmt);
}

/* allocate, init and add new passport */
static struct passport_t *
prozubi_passport_new(
		kdata2_t *kdata,

#define PASSPORT_COLUMN_DATE(member, number, title) time_t member,
#define PASSPORT_COLUMN_TEXT(member, number, title) const char * member,
			
		PASSPORT_COLUMNS

#undef PASSPORT_COLUMN_DATE			
#undef PASSPORT_COLUMN_TEXT
		const char *id
		)
{
	/* allocate passport_t */
	struct passport_t *p = NEW(struct passport_t, 
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
			STR_ERR("%s", "can't allocate struct passport_t")); return NULL);

	if (!id){
		/* create new uuid */
		uuid4_init();
		uuid4_generate(p->id);
	} else
		strcpy(p->id, id);

	/* set values */

#define PASSPORT_COLUMN_DATE(member, number, title)\
	p->member = member;
#define PASSPORT_COLUMN_TEXT(member, number, title)\
	if (!member)\
		member = "";\
	p->member = strdup(member);
			
	PASSPORT_COLUMNS

#undef PASSPORT_COLUMN_DATE			
#undef PASSPORT_COLUMN_TEXT	

	return p;
}

static void
prozubi_passport_free(struct passport_t *d){
	if (d){

#define PASSPORT_COLUMN_DATE(member, number, title)
#define PASSPORT_COLUMN_TEXT(member, number, title) if(d->member) free(d->member); 
		PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_TEXT			
#undef PASSPORT_COLUMN_DATE			
		
		free(d);
		d = NULL;
	}
}

#define PASSPORT_COLUMN_DATE(member, number, title)\
	static time_t prozubi_passport_get_##number(struct passport_t *c){\
		return c->member;\
	}
#define PASSPORT_COLUMN_TEXT(member, number, title)\
	static char * prozubi_passport_get_##number(struct passport_t *c){\
		return c->member;\
	}\
	static size_t prozubi_passport_get_len_##number(struct passport_t *c){\
		return c->len_##member;\
	}	
   	
	PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT


static void * 
prozubi_passport_get(struct passport_t *c, PASSPORT key){
	switch (key) {
#define PASSPORT_COLUMN_DATE(member, number, title)\
		case number:\
			{\
				return &(c->member);\
			}		
#define PASSPORT_COLUMN_TEXT(member, number, title)\
		case number:\
			{\
				return c->member;\
			}
		PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT			
		default:
			break;
	}
	
	return NULL;
}

static size_t 
prozubi_passport_get_len(struct passport_t *c, const char *name){
	int index = getIndexPASSPORT(name);	
	if (index == -1)
		return 0;

	switch (index) {
#define PASSPORT_COLUMN_DATE(member, number, title)\
		case number:\
			{\
				return 0;\
			}		
#define PASSPORT_COLUMN_TEXT(member, number, title)\
		case number:\
			{\
				return c->len_##member;\
			}
		PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT			
	}
	
	return 0;
}

#define PASSPORT_COLUMN_DATE(member, number, title)\
static int prozubi_passport_set_##number (kdata2_t *p, struct passport_t *c,\
		time_t t, bool update)\
{\
	if (update)\
		if (!kdata2_set_number_for_uuid(p, PASSPORT_TABLENAME, title, t, c->id))\
			return -1;\
	c->member = t;\
	return 0;\
}
#define PASSPORT_COLUMN_TEXT(member, number, title)\
static int prozubi_passport_set_##number (kdata2_t *p, struct passport_t *c,\
		const char *text, bool update)\
{\
	if (update)\
		if (!kdata2_set_text_for_uuid(p, PASSPORT_TABLENAME, title, text, c->id))\
			return -1;\
	if(c->member)\
		free(c->member);\
	c->member = strdup(text);\
	c->len_##member = strlen(text);\
	return 0;\
}
		PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT			

static int prozubi_passport_set_text(
		PASSPORT key, kdata2_t *p, struct passport_t *c, const char *text, bool update)
{
	switch (key) {
#define PASSPORT_COLUMN_DATE(member, number, title) case number: break;	
#define PASSPORT_COLUMN_TEXT(member, number, title) case number:\
		return prozubi_passport_set_##number(p, c, text, update);\
		
		PASSPORT_COLUMNS

#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT			
		
		default:
			break;
	}
	return -1;
}

static int prozubi_passport_set_date(
		PASSPORT key, kdata2_t *p, struct passport_t *c, time_t t, bool update)
{
	switch (key) {
#define PASSPORT_COLUMN_TEXT(member, number, title) case number: break;	
#define PASSPORT_COLUMN_DATE(member, number, title) case number:\
		return prozubi_passport_set_##number(p, c, t, update);\
		
		PASSPORT_COLUMNS

#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT			
		
		default:
			break;
	}
	return -1;
}

static int prozubi_passport_update(
		kdata2_t *p, struct passport_t *c
		)
{
	int i;
	for (i = 0; i < PASSPORT_COLS_NUM; ++i) {
		switch (i) {
				
#define PASSPORT_COLUMN_DATE(member, number, title) \
				case number:\
				{\
					kdata2_set_number_for_uuid(p, PASSPORT_TABLENAME, title,\
						   	c->member, c->id);\
					break;\
				}; 
				
#define PASSPORT_COLUMN_TEXT(member, number, title) \
				case number:\
				{\
					kdata2_set_text_for_uuid(p, PASSPORT_TABLENAME, title,\
						   	c->member, c->id);\
					break;\
				}; 

			PASSPORT_COLUMNS

#undef PASSPORT_COLUMN_DATE			
#undef PASSPORT_COLUMN_TEXT			

			default:
				break;
		}
	}

	return 0;
}

static int prozubi_passport_remove(
		kdata2_t *p, struct passport_t *c
		)
{
	return kdata2_remove_for_uuid(p, PASSPORT_TABLENAME, c->id);
}

#endif /* ifndef PASSPORT_H */
