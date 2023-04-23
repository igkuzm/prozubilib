/**
 * File              : cases.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 23.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef CASES_H
#define CASES_H

#include "prozubilib_conf.h"

#include <string.h>
#include <stdlib.h>

#define CASES_TABLENAME "ZCASES"

struct case_id {
	uuid4_str id;            /* uuid of the case */
	time_t    date;          /* date and time of visit */
	char      name[128];     /* name of case */
	char      patientid[37]; /* patient uuid recordname */
};

/*
 * CASES_COLUMN_DATE(struct member, enum number, SQLite column title)
 * CASES_COLUMN_TEXT(struct member, enum number, SQLite column title)
 * CASES_COLUMN_DATA(struct member, enum number, SQLite column title, type)
 */
#define CASES_COLUMNS \
	CASES_COLUMN_DATE(date, CASEDATE, "ZDATE")\
	CASES_COLUMN_TEXT(name, CASENAME, "ZNAME")\
	CASES_COLUMN_TEXT(patientid, CASEPATIENTID, "ZPATIENTID")\
	CASES_COLUMN_DATE(dateofnext, CASEDATEOFNEXT, "ZDATEOFNEXT")\
	CASES_COLUMN_TEXT(z11, Z11, "Z11")\
	CASES_COLUMN_TEXT(z12, Z12, "Z12")\
	CASES_COLUMN_TEXT(z13, Z13, "Z13")\
	CASES_COLUMN_TEXT(z14, Z14, "Z14")\
	CASES_COLUMN_TEXT(z15, Z15, "Z15")\
	CASES_COLUMN_TEXT(z16, Z16, "Z16")\
	CASES_COLUMN_TEXT(z17, Z17, "Z17")\
	CASES_COLUMN_TEXT(z18, Z18, "Z18")\
	CASES_COLUMN_TEXT(z21, Z21, "Z21")\
	CASES_COLUMN_TEXT(z22, Z22, "Z22")\
	CASES_COLUMN_TEXT(z23, Z23, "Z23")\
	CASES_COLUMN_TEXT(z24, Z24, "Z24")\
	CASES_COLUMN_TEXT(z25, Z25, "Z25")\
	CASES_COLUMN_TEXT(z26, Z26, "Z26")\
	CASES_COLUMN_TEXT(z27, Z27, "Z27")\
	CASES_COLUMN_TEXT(z28, Z28, "Z28")\
	CASES_COLUMN_TEXT(z31, Z31, "Z31")\
	CASES_COLUMN_TEXT(z32, Z32, "Z32")\
	CASES_COLUMN_TEXT(z33, Z33, "Z33")\
	CASES_COLUMN_TEXT(z34, Z34, "Z34")\
	CASES_COLUMN_TEXT(z35, Z35, "Z35")\
	CASES_COLUMN_TEXT(z36, Z36, "Z36")\
	CASES_COLUMN_TEXT(z37, Z37, "Z37")\
	CASES_COLUMN_TEXT(z38, Z38, "Z38")\
	CASES_COLUMN_TEXT(z41, Z41, "Z41")\
	CASES_COLUMN_TEXT(z42, Z42, "Z42")\
	CASES_COLUMN_TEXT(z43, Z43, "Z43")\
	CASES_COLUMN_TEXT(z44, Z44, "Z44")\
	CASES_COLUMN_TEXT(z45, Z45, "Z45")\
	CASES_COLUMN_TEXT(z46, Z46, "Z46")\
	CASES_COLUMN_TEXT(z47, Z47, "Z47")\
	CASES_COLUMN_TEXT(z48, Z48, "Z48")\
	CASES_COLUMN_TEXT(alleganamnez, CASEALLERGANAMNEZ, "ZALLERGANAMNEZ")\
	CASES_COLUMN_TEXT(anamnezvitae, CASEANAMNEZVITAE, "ZANAMNEZVITAE")\
	CASES_COLUMN_TEXT(anamnezmorbi, CASEANAMNEZMORBI, "ZANAMNEZMORBI")\
	CASES_COLUMN_TEXT(diagnozis, CASEDIAGNOZIS, "ZDIAGNOZIS")\
	CASES_COLUMN_TEXT(instrumentandrentgen, CASEINSTRUMENTANDRENTGEN, "ZINSTRUMENTANDRENTGEN")\
	CASES_COLUMN_TEXT(lecheniye, CASELECHENIYE, "ZLECHENIYE")\
	CASES_COLUMN_TEXT(mestno, CASEMESTNO, "ZMESTNO")\
	CASES_COLUMN_TEXT(polozheniye, CASEPOLOZHENIYE, "ZPOLOZHENIYE")\
	CASES_COLUMN_TEXT(recomendacii, CASERECOMENDACII, "ZRECOMENDACII")\
	CASES_COLUMN_TEXT(sostoyanievnutrennihorganov, CASESOSTORONIVNUTRENNIHORGANOV, "ZSOSTORONIVNUTRENNIHORGANOV")\
	CASES_COLUMN_TEXT(sostoyanie, CASESOSTOYANIYE, "ZSOSTOYANIYE")\
	CASES_COLUMN_TEXT(zhalobi, CASEZHALOBI, "ZZHALOBI")\
	CASES_COLUMN_DATA(bill, CASEBILL, "ZBILL", cJSON)\
	CASES_COLUMN_DATA(planlecheniya, CASEPLANLECHENIYA, "ZPLANLECHENIYADATA", cJSON)

#define CASES_DATA_TYPES\
	CASES_DATA_TYPE(cJSON)

enum cases_data_types {
#define CASES_DATA_TYPE(type) CASES_DATA_TYPE_##type, 
	CASES_DATA_TYPES
#undef CASES_DATA_TYPE

	CASES_DATA_TYPES_NUM,
};

struct case_t {
	uuid4_str id;         /* uuid of the case */

#define CASES_COLUMN_DATE(member, number, title) time_t member; 
#define CASES_COLUMN_TEXT(member, number, title) char * member; 
#define CASES_COLUMN_DATA(member, number, title, type) type * member; size_t len_##member; 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA
};


enum cases_column_number {
#define CASES_COLUMN_DATE(member, number, title) number, 
#define CASES_COLUMN_TEXT(member, number, title) number, 
#define CASES_COLUMN_DATA(member, number, title, type) number, 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA

	CASES_COLS_NUM,
};

static void	
prozubi_cases_table_init(struct kdata2_table **cases){
	kdata2_table_init(cases, CASES_TABLENAME,

#define CASES_COLUMN_DATE(member, number, title) KDATA2_TYPE_NUMBER, title, 
#define CASES_COLUMN_TEXT(member, number, title) KDATA2_TYPE_TEXT, title, 
#define CASES_COLUMN_DATA(member, number, title, type) KDATA2_TYPE_DATA, title, 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA
			
			NULL); 
} 

/* do callback for each case for patientid in database; set patientid to NULL to get all */
static void 
prozubi_cases_foreach(
		kdata2_t   *kdata,
		const char *patientid,
		void       *user_data,
		int        (*callback)(void *user_data, struct case_id *c)
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

	char SQL[BUFSIZ] = "SELECT ZRECORDNAME, ZDATE, ZNAME, ZPATIENTID FROM ";
		strcat(SQL, CASES_TABLENAME);
	/* check patient id */
	if (patientid)
		strcat(SQL, STR(" WHERE ZPATIENTID = '%s'", patientid));

	/* case id */
	struct case_id c;

	/* start SQLite request */
	int res;
	sqlite3_stmt *stmt;
	
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db));	
		return;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
		/* ZRECORDNAME */
		strncpy(c.id, (const char *)sqlite3_column_text(stmt, 0), sizeof(c.id) - 1); 
		c.id[sizeof(c.id) - 1] = 0;

		/* ZDATE */
		int col_type = sqlite3_column_type(stmt, 1);
		if (col_type == SQLITE_INTEGER) {
			c.date = sqlite3_column_int64(stmt, 1);
		} else if (col_type == SQLITE_FLOAT) {
			//NSDate stores timestamp as double from 2001-01-01 00:00:00 (+978307200)
			c.date = sqlite3_column_double(stmt, 1) + NSTimeIntervalSince1970; 
		}

		/* ZNAME */
		strncpy(c.name, (const char *)sqlite3_column_text(stmt, 2), 
				sizeof(c.name) - 1); 
		c.name[sizeof(c.name) - 1] = 0;
		
		/* ZPATIENTID */
		strncpy(c.patientid, (const char *)sqlite3_column_text(stmt, 0), 
					sizeof(c.patientid) - 1); 
		c.patientid[sizeof(c.patientid) - 1] = 0;		

		/* callback */
		if (callback)
			if (callback(user_data, &c))
				break;
	}	
	
	sqlite3_finalize(stmt);
}

/* allocate and init new case */
static struct case_t *
prozubi_case_new(const char *id){
	/* allocate case_t */
	struct case_t *c = malloc(sizeof(struct case_t));
	if (!c){
		ERR("%s", "can't allocate struct case_t");
		return NULL;
	}

	if (!id){
		/* create new uuid */
		UUID4_STATE_T state; UUID4_T identifier;
		uuid4_seed(&state);
		uuid4_gen(&state, &identifier);
		if (!uuid4_to_s(identifier, c->id, 37)){
			ERR("%s", "can't generate uuid");
			return NULL;
		}
	} else
		strcpy(c->id, id);

	/* init values to NULL */
#define CASES_COLUMN_DATE(member, number, title) c->member = time(NULL); 
#define CASES_COLUMN_TEXT(member, number, title) c->member = NULL; 
#define CASES_COLUMN_DATA(member, number, title, type) c->member = NULL; 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA
	
	return c;
}

/* get case with id */
static struct case_t * 
prozubi_case_get(
		kdata2_t   *kdata,
		const char *id)
{
	/* check kdata */
	if (!kdata){
		ERR("%s", "kdata is NULL");
		return NULL;
	}
	if (!kdata->db){
		ERR("%s", "kdata->db is NULL");
		return NULL;
	}

	/* check id */
	if (!id){
		ERR("%s", "id is NULL");
		return NULL;
	}
	
	/* allocate and init case_t */
	struct case_t *c = prozubi_case_new(id);
	if (!c)
		return NULL;
	
	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define CASES_COLUMN_DATE(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
#define CASES_COLUMN_TEXT(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
#define CASES_COLUMN_DATA(member, number, title, type) strcat(SQL, title); strcat(SQL, ", "); 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, CASES_TABLENAME);
	strcat(SQL, STR(" WHERE ZRECORDNAME = '%s'", id));

	/* start SQLite request */
	int res;
	sqlite3_stmt *stmt;
	
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db));	
		free(c);
		return NULL;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
		/* iterate columns */
		int i;
		for (i = 0; i < CASES_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define CASES_COLUMN_DATE(member, number, title) \
				case number:\
				{\
					int col_type = sqlite3_column_type(stmt, i);\
					if (col_type == SQLITE_INTEGER) {\
						c->member = sqlite3_column_int64(stmt, i);\
					} else if (col_type == SQLITE_FLOAT) {\
						c->member = sqlite3_column_double(stmt, i) + NSTimeIntervalSince1970;\
					}\
					break;\
				}; 
				
#define CASES_COLUMN_TEXT(member, number, title) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					if (value){\
						char *str = malloc(len);\
						if (!str){\
							ERR("can't allocate string with len: %ld", len);\
							break;\
						}\
						strncpy(str, (const char *)value, len - 1);\
						str[len - 1] = 0;\
						c->member = str;\
					}\
					break;\
				}; 

#define CASES_COLUMN_DATA(member, number, title, type) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const void *value = sqlite3_column_blob(stmt, i);\
					if (CASES_DATA_TYPE_##type == CASES_DATA_TYPE_cJSON){\
						cJSON *json = cJSON_ParseWithLength(value, len);\
						c->member = json;\
						c->len_##member = len;\
					}\
					break;\
				};

			CASES_COLUMNS

#undef CASES_COLUMN_DATE			
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA			

				default:
					break;					
			}
		}		
	}	
	
	sqlite3_finalize(stmt);
	return c;
}

static void
prozubi_case_free(struct case_t *c){
	if (c){

#define CASES_COLUMN_DATE(member, number, title) 
#define CASES_COLUMN_TEXT(member, number, title) if(c->member) free(c->member); 
#define CASES_COLUMN_DATA(member, number, title, type) if(c->member) free(c->member); 
		CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA			
		
		free(c);
	}
}

#endif /* ifndef CASES_H */
