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

extern void	
prozubi_passport_table_init(struct kdata2_table **passport); 

/* do callback for each patient in database */
extern void 
prozubi_passport_foreach(
		kdata2_t *kdata,
		const char *predicate,
		void     *user_data,
		int      (*callback)(void *user_data, struct passport_t *p)
		);

/* allocate, init and add new passport */
extern struct passport_t *
prozubi_passport_new(
		kdata2_t *kdata,

#define PASSPORT_COLUMN_DATE(member, number, title) time_t member,
#define PASSPORT_COLUMN_TEXT(member, number, title) const char * member,
			
		PASSPORT_COLUMNS

#undef PASSPORT_COLUMN_DATE			
#undef PASSPORT_COLUMN_TEXT
		const char *id
		);

extern void
prozubi_passport_free(struct passport_t *d);

#define PASSPORT_COLUMN_DATE(member, number, title)\
	extern time_t prozubi_passport_get_##number(struct passport_t *c);

#define PASSPORT_COLUMN_TEXT(member, number, title)\
	extern char * prozubi_passport_get_##number(struct passport_t *c);\
	extern size_t prozubi_passport_get_len_##number(struct passport_t *c);	
   	
	PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT


extern void * 
prozubi_passport_get(struct passport_t *c, PASSPORT key);

extern size_t 
prozubi_passport_get_len(struct passport_t *c, const char *name);

#define PASSPORT_COLUMN_DATE(member, number, title)\
extern int prozubi_passport_set_##number (kdata2_t *p, struct passport_t *c,\
		time_t t, bool update);

#define PASSPORT_COLUMN_TEXT(member, number, title)\
extern int prozubi_passport_set_##number (kdata2_t *p, struct passport_t *c,\
		const char *text, bool update);

		PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT			

extern int prozubi_passport_set_text(
		PASSPORT key, kdata2_t *p, struct passport_t *c, const char *text, bool update);

extern int prozubi_passport_set_date(
		PASSPORT key, kdata2_t *p, struct passport_t *c, time_t t, bool update);

extern int prozubi_passport_update(
		kdata2_t *p, struct passport_t *c
		);

extern int prozubi_passport_remove(
		kdata2_t *p, struct passport_t *c
		);

#endif /* ifndef PASSPORT_H */
