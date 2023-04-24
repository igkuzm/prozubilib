/**
 * File              : doctors.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 24.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef DOCTORS_H
#define DOCTORS_H

#include "prozubilib_conf.h"

#include "enum.h"

#include <string.h>
#include <stdlib.h>

#define DOCTORS_TABLENAME "ZDOCTORS"

/*
 * DOCTORS_COLUMN_TEXT(struct member, enum number, SQLite column title, size)
 * DOCTORS_COLUMN_DATA(struct member, enum number, SQLite column title, type)
 */
#define DOCTORS_COLUMNS \
	DOCTORS_COLUMN_TEXT(fio,           DOCTORFIO,           "ZFIO",           128)\
	DOCTORS_COLUMN_TEXT(eventcalendar, DOCTOREVENTCALENDAR, "ZEVENTCALENDAR", 128)\
	DOCTORS_COLUMN_TEXT(email,         DOCTOREMAIL,         "ZICLOUDID",      128)\
	DOCTORS_COLUMN_TEXT(tel,           DOCTORTEL,           "ZTEL",           32)\
	DOCTORS_COLUMN_DATA(zapis,         DOCTORZAPIS,         "ZZAPIS",         cJSON)

#define DOCTORS_DATA_TYPES\
	DOCTORS_DATA_TYPE(cJSON)

enum doctor_data_types {
#define DOCTORS_DATA_TYPE(type) DOCTORS_DATA_TYPE_##type, 
	DOCTORS_DATA_TYPES
#undef DOCTORS_DATA_TYPE

	DOCTORS_DATA_TYPES_NUM,
};

struct doctor_t {
	uuid4_str id;         /* uuid of the doctor */

#define DOCTORS_COLUMN_TEXT(member, number, title, size) char member[size]; 
#define DOCTORS_COLUMN_DATA(member, number, title, type) type * member; size_t len_##member; 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA
};


BEGIN_ENUM(DOCTORS) 
{
#define DOCTORS_COLUMN_TEXT(member, number, title, size) DECL_ENUM_ELEMENT(number), 
#define DOCTORS_COLUMN_DATA(member, number, title, type) DECL_ENUM_ELEMENT(number), 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA

	DOCTORS_COLS_NUM,
}
END_ENUM(DOCTORS)

BEGIN_ENUM_STRING(DOCTORS) 
{
#define DOCTORS_COLUMN_TEXT(member, number, title, size) DECL_ENUM_STRING_ELEMENT(number), 
#define DOCTORS_COLUMN_DATA(member, number, title, type) DECL_ENUM_STRING_ELEMENT(number), 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA
}
END_ENUM_STRING(DOCTORS)	

static void	
prozubi_doctors_table_init(struct kdata2_table **doctors){
	kdata2_table_init(doctors, DOCTORS_TABLENAME,

#define DOCTORS_COLUMN_TEXT(member, number, title, size) KDATA2_TYPE_TEXT, title, 
#define DOCTORS_COLUMN_DATA(member, number, title, type) KDATA2_TYPE_DATA, title, 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA
			
			NULL); 
} 

/* allocate and init new case */
static struct doctor_t *
prozubi_doctor_new(
		kdata2_t *kdata,
#define DOCTORS_COLUMN_TEXT(member, number, title, size) const char * member, 
#define DOCTORS_COLUMN_DATA(member, number, title, type) void * member, size_t len_##member, 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA		
		const char *id
		)
{
	/* allocate case_t */
	struct doctor_t *d = malloc(sizeof(struct doctor_t));
	if (!d){
		ERR("%s", "can't allocate struct doctor_t");
		return NULL;
	}

	if (!id){
		/* create new uuid */
		UUID4_STATE_T state; UUID4_T identifier;
		uuid4_seed(&state);
		uuid4_gen(&state, &identifier);
		if (!uuid4_to_s(identifier, d->id, 37)){
			ERR("%s", "can't generate uuid");
			return NULL;
		}
	} else
		strcpy(d->id, id);

	/* set values */
#define DOCTORS_COLUMN_TEXT(member, number, title, size)\
   	if (member)\
		kdata2_set_text_for_uuid(kdata, DOCTORS_TABLENAME, title, member, d->id);	
#define DOCTORS_COLUMN_DATA(member, number, title, type)\
   	if (member)\
		kdata2_set_data_for_uuid(kdata, DOCTORS_TABLENAME, title, member, len_##member, d->id);	
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA
	
	return d;
}

/* callback all images with case id; set caseid to NULL to get all images in database */
static void 
prozubi_doctor_foreach(
		kdata2_t   *kdata,
		void       *user_data,
		int        (*callback)(void *user_data, struct doctor_t *d)
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

	struct doctor_t d;
	
	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define DOCTORS_COLUMN_TEXT(member, number, title, size) strcat(SQL, title); strcat(SQL, ", "); 
#define DOCTORS_COLUMN_DATA(member, number, title, type) strcat(SQL, title); strcat(SQL, ", "); 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT			
#undef DOCTORS_COLUMN_DATA			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, DOCTORS_TABLENAME);

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
		for (i = 0; i < DOCTORS_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define DOCTORS_COLUMN_TEXT(member, number, title, size) \
				case number:\
				{\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					if (value){\
						strncpy(d.member, (const char *)value, size - 1);\
						d.member[size - 1] = 0;\
					}\
					break;\
				}; 

#define DOCTORS_COLUMN_DATA(member, number, title, type) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const void *value = sqlite3_column_blob(stmt, i);\
					if (DOCTORS_DATA_TYPE_##type == DOCTORS_DATA_TYPE_cJSON){\
						cJSON *json = cJSON_Parse(value);\
						d.member = json;\
						d.len_##member = 0;\
					}\
					break;\
				};

			DOCTORS_COLUMNS

#undef DOCTORS_COLUMN_TEXT			
#undef DOCTORS_COLUMN_DATA			

				default:
					break;					
			}
		}		

		/* handle doctor id */
		const unsigned char *value = sqlite3_column_text(stmt, i);				
		strncpy(d.id, (const char *)value, sizeof(d.id) - 1);
		d.id[sizeof(d.id) - 1] = 0;		

		/* callback */
		if (callback)
			if (callback(user_data, &d))
				break;		
	}	

	sqlite3_finalize(stmt);
}

#endif /* ifndef DOCTORS_H */
