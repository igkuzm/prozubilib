/**
 * File              : passport.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 27.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PASSPORT_H
#define PASSPORT_H

#include "prozubilib_conf.h"

#include "enum.h"

#include <string.h>
#include <stdlib.h>

#define PASSPORT_TABLENAME "ZPASSPORT"

/*
 * PASSPORT_COLUMN_DATE(struct member, enum number, SQLite column title)
 * PASSPORT_COLUMN_TEXT(struct member, enum number, SQLite column title, size)
 */
#define PASSPORT_COLUMNS \
	PASSPORT_COLUMN_DATE(dateofbirth, PASSPORTDATEOFBIRTH, "ZDATEOFBIRTH")\
	PASSPORT_COLUMN_TEXT(familiya,    PASSPORTFAMILIYA,    "ZFAMILIYA",  128)\
	PASSPORT_COLUMN_TEXT(imia,        PASSPOTIMIA,         "ZIMIA",      128)\
	PASSPORT_COLUMN_TEXT(otchestvo,   PASSPORTOTCHESTVO,   "ZOTCHESTVO", 128)\
	PASSPORT_COLUMN_TEXT(address,     PASSPORTADDRESS,     "ZADDRESS",   256)\
	PASSPORT_COLUMN_TEXT(document,    PASSPORTDOCUMENT,    "ZDOCUMENT",  256)\
	PASSPORT_COLUMN_TEXT(comment,     PASSPORTCOMMENT,     "ZCOMMENT",   256)\
	PASSPORT_COLUMN_TEXT(tel,         PASSPORTTEL,         "ZTEL",       32)\
	PASSPORT_COLUMN_TEXT(email,       PASSPORTEMAIL,       "ZEMAIL",     32)

struct passport_t {
	uuid4_str id;              /* uuid of the passport (patientid) */

#define PASSPORT_COLUMN_DATE(member, number, title) time_t member; 
#define PASSPORT_COLUMN_TEXT(member, number, title, size) char member[size]; 
	PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT
	
};


BEGIN_ENUM(PASSPORT) 
#define PASSPORT_COLUMN_DATE(member, number, title) DECL_ENUM_ELEMENT(number), 
#define PASSPORT_COLUMN_TEXT(member, number, title, size) DECL_ENUM_ELEMENT(number), 
	PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT	

	PASSPORT_COLS_NUM,
END_ENUM(PASSPORT)

BEGIN_ENUM_STRING(PASSPORT) 
#define PASSPORT_COLUMN_DATE(member, number, title) DECL_ENUM_STRING_ELEMENT(number), 
#define PASSPORT_COLUMN_TEXT(member, number, title, size) DECL_ENUM_STRING_ELEMENT(number), 
	PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT	
END_ENUM_STRING(PASSPORT)	

static void	
prozubi_passport_table_init(struct kdata2_table **passport){
	kdata2_table_init(passport, PASSPORT_TABLENAME,

#define PASSPORT_COLUMN_DATE(member, number, title) KDATA2_TYPE_NUMBER, title, 
#define PASSPORT_COLUMN_TEXT(member, number, title, size) KDATA2_TYPE_TEXT, title, 
PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT			
			
			NULL); 
} 

/* do callback for each patient in database */
static void 
prozubi_passport_foreach(
		kdata2_t *kdata,
		void     *user_data,
		int      (*callback)(void *user_data, struct passport_t *p)
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

	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define PASSPORT_COLUMN_DATE(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
#define PASSPORT_COLUMN_TEXT(member, number, title, size) strcat(SQL, title); strcat(SQL, ", "); 
PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, PASSPORT_TABLENAME);

	/* passort struct */
	struct passport_t p;

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
		for (i = 0; i < PASSPORT_COLS_NUM; ++i) {
			/* cast string */
			switch (i) {

#define PASSPORT_COLUMN_DATE(member, number, title) \
				case number:\
				{\
					int col_type = sqlite3_column_type(stmt, i);\
					if (col_type == SQLITE_INTEGER) {\
						p.member = sqlite3_column_int64(stmt, i);\
					} else if (col_type == SQLITE_FLOAT) {\
						p.member = sqlite3_column_double(stmt, i) + NSTimeIntervalSince1970;\
					}\
					break;\
				}; 
				
#define PASSPORT_COLUMN_TEXT(member, number, title, size) \
				case number:\
				{\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					if (value){\
						strncpy(p.member, (const char *)value, sizeof(p.member) - 1);\
						p.member[sizeof(p.member) - 1] = 0;\
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
		const unsigned char *value = sqlite3_column_text(stmt, i);				
		strncpy(p.id, (const char *)value, sizeof(p.id) - 1);
		p.id[sizeof(p.id) - 1] = 0;		

		/* callback */
		if (callback)
			if (callback(user_data, &p))
				break;
	}	
	
	sqlite3_finalize(stmt);
}

/* allocate, init and add new passport */
static struct passport_t *
prozubi_passport_new(
		kdata2_t *kdata,

#define PASSPORT_COLUMN_DATE(member, number, title)
#define PASSPORT_COLUMN_TEXT(member, number, title, size) const char * member,
			
		PASSPORT_COLUMNS

#undef PASSPORT_COLUMN_DATE			
#undef PASSPORT_COLUMN_TEXT
		const char *id
		)
{
	/* allocate case_t */
	struct passport_t *p = malloc(sizeof(struct passport_t));
	if (!p){
		ERR("%s", "can't allocate struct passport_t");
		return NULL;
	}

	if (!id){
		/* create new uuid */
		UUID4_STATE_T state; UUID4_T identifier;
		uuid4_seed(&state);
		uuid4_gen(&state, &identifier);
		if (!uuid4_to_s(identifier, p->id, 37)){
			ERR("%s", "can't generate uuid");
			return NULL;
		}
	} else
		strcpy(p->id, id);

	/* set values */

#define PASSPORT_COLUMN_DATE(member, number, title)\
   	kdata2_set_number_for_uuid(kdata, PASSPORT_TABLENAME, title, time(NULL), p->id); 
#define PASSPORT_COLUMN_TEXT(member, number, title, size)\
   	if (member) kdata2_set_text_for_uuid(kdata, PASSPORT_TABLENAME, title, member, p->id);
			
	PASSPORT_COLUMNS

#undef PASSPORT_COLUMN_DATE			
#undef PASSPORT_COLUMN_TEXT	

	return p;
}

#endif /* ifndef PASSPORT_H */
