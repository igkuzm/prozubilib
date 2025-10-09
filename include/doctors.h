/**
 * File              : doctors.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 24.08.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef DOCTORS_H
#define DOCTORS_H

#include "prozubilib_conf.h"

#define DOCTORS_TABLENAME "ZDOCTORS"

/*
 * DOCTORS_COLUMN_TEXT(struct member, enum number, SQLite column title)
 * DOCTORS_COLUMN_DATA(struct member, enum number, SQLite column title, type)
 */
#define DOCTORS_COLUMNS \
	DOCTORS_COLUMN_TEXT(fio,           DOCTORFIO,           "ZFIO"           )\
	DOCTORS_COLUMN_TEXT(eventcalendar, DOCTOREVENTCALENDAR, "ZEVENTCALENDAR" )\
	DOCTORS_COLUMN_TEXT(email,         DOCTOREMAIL,         "ZICLOUDID"      )\
	DOCTORS_COLUMN_TEXT(tel,           DOCTORTEL,           "ZTEL"           )\
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

#define DOCTORS_COLUMN_TEXT(member, number, title      ) char * member; size_t len_##member; 
#define DOCTORS_COLUMN_DATA(member, number, title, type) type * member; size_t len_##member; 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA
};


BEGIN_ENUM(DOCTORS) 
#define DOCTORS_COLUMN_TEXT(member, number, title      ) DECL_ENUM_ELEMENT(number), 
#define DOCTORS_COLUMN_DATA(member, number, title, type) DECL_ENUM_ELEMENT(number), 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA

	DOCTORS_COLS_NUM,
END_ENUM(DOCTORS)

BEGIN_ENUM_STRING(DOCTORS) 
#define DOCTORS_COLUMN_TEXT(member, number, title      ) DECL_ENUM_STRING_ELEMENT(number), 
#define DOCTORS_COLUMN_DATA(member, number, title, type) DECL_ENUM_STRING_ELEMENT(number), 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA
END_ENUM_STRING(DOCTORS)	

extern void	
prozubi_doctors_table_init(struct kdata2_table **doctors); 

/* allocate and init new case */
extern struct doctor_t *
prozubi_doctor_new(
		kdata2_t *kdata,
#define DOCTORS_COLUMN_TEXT(member, number, title      ) const char * member, 
#define DOCTORS_COLUMN_DATA(member, number, title, type) void * member, size_t len_##member, 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA		
		const char *id
		);

/* callback all images with case id; set caseid to NULL to get all images in database */
extern void 
prozubi_doctor_foreach(
		kdata2_t   *kdata,
		const char *predicate,
		void       *user_data,
		int        (*callback)(void *user_data, struct doctor_t *d)
		);

extern void
prozubi_doctor_free(struct doctor_t *d);

static int prozubi_doctor_remove(
		kdata2_t *p, struct doctor_t *c
		);

#endif /* ifndef DOCTORS_H */
