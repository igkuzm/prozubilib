/**
 * File              : doctors.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 09.10.2025
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/doctors.h"

void	
prozubi_doctors_table_init(struct kdata2_table **doctors)
{
	kdata2_table_init(doctors, DOCTORS_TABLENAME,

#define DOCTORS_COLUMN_TEXT(member, number, title) KDATA2_TYPE_TEXT, title, 
#define DOCTORS_COLUMN_DATA(member, number, title, type) KDATA2_TYPE_DATA, title, 
DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA			
			
			NULL); 
} 

/* allocate and init new case */
 struct doctor_t *
prozubi_doctor_new(
		kdata2_t *kdata,
#define DOCTORS_COLUMN_TEXT(member, number, title      ) const char * member, 
#define DOCTORS_COLUMN_DATA(member, number, title, type) void * member, size_t len_##member, 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT
#undef DOCTORS_COLUMN_DATA		
		const char *id
		)
{
	/* allocate case_t */
	struct doctor_t *d = NEW(struct doctor_t);
	if (d == NULL){	
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,			
				STR_ERR("%s", "can't allocate struct doctor_t")); 
		return NULL;
	}
	if (!id){
		/* create new uuid */
		uuid4_init();
		uuid4_generate(d->id);	
	} else
		strcpy(d->id, id);

	/* set values */
#define DOCTORS_COLUMN_TEXT(member, number, title)\
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
 void 
prozubi_doctor_foreach(
		kdata2_t   *kdata,
		const char *predicate,
		void       *user_data,
		int        (*callback)(void *user_data, struct doctor_t *d)
		)
{
	const unsigned char *value;
	int res, i;
	sqlite3_stmt *stmt;
	char SQL[2*BUFSIZ] = "SELECT ";
	
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
#define DOCTORS_COLUMN_TEXT(member, number, title      ) strcat(SQL, title); strcat(SQL, ", "); 
#define DOCTORS_COLUMN_DATA(member, number, title, type) strcat(SQL, title); strcat(SQL, ", "); 
	DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT			
#undef DOCTORS_COLUMN_DATA			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, DOCTORS_TABLENAME);
	strcat(SQL, " ");
	if (predicate)
		strcat(SQL, predicate);

	/* start SQLite request */
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,
		STR_ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db)));	
		return;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
	
		struct doctor_t *d = NEW(struct doctor_t);
		if (d == NULL){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,				
				STR_ERR("%s", "can't allocate struct doctor_t")); 
			return;
		} 
	
		/* iterate columns */
		for (i = 0; i < DOCTORS_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define DOCTORS_COLUMN_TEXT(member, number, title) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					if (value){\
						d->member = strdup((char*)value);\
						d->len_##member = len;\
					} else {\
						d->member = NULL;\
					}\
					break;\
				}; 

#define DOCTORS_COLUMN_DATA(member, number, title, type) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const void *value = sqlite3_column_blob(stmt, i);\
					if (DOCTORS_DATA_TYPE_##type == DOCTORS_DATA_TYPE_cJSON){\
						d->member = cJSON_Parse((char*)value);\
						d->len_##member = 0;\
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
		value = sqlite3_column_text(stmt, i);				
		strncpy(d->id, (const char *)value, sizeof(d->id) - 1);
		d->id[sizeof(d->id) - 1] = 0;		

		/* callback */
		if (callback)
			if (callback(user_data, d))
				break;		
	}	

	sqlite3_finalize(stmt);
}

 void
prozubi_doctor_free(struct doctor_t *d){
	if (d){

#define DOCTORS_COLUMN_TEXT(member, number, title) if(d->member) free(d->member); 
#define DOCTORS_COLUMN_DATA(member, number, title, type) if(d->member) free(d->member); 
		DOCTORS_COLUMNS
#undef DOCTORS_COLUMN_TEXT			
#undef DOCTORS_COLUMN_DATA			
		
		free(d);
		d = NULL;
	}
}

int prozubi_doctor_remove(
		kdata2_t *p, struct doctor_t *c
		)
{
	return kdata2_remove_for_uuid(p, DOCTORS_TABLENAME, c->id);
}

