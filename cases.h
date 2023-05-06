/**
 * File              : cases.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 06.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef CASES_H
#define CASES_H

#ifndef UUIDCOLUMN
#define UUIDCOLUMN "ZRECORDNAME"
#endif /* ifndef UUIDCOLUMN */

#include "prozubilib_conf.h"
#include "kdata2/cYandexDisk/cJSON.h"

#include "enum.h"
#include "alloc.h"
#include "date-time/time_local.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define CASES_TABLENAME "ZCASES"

#define TO_STR_(X) #X
#define TO_STR(X) TO_STR_(X)

#define CASES_LIST_TYPES \
	CASES_LIST_TYPE_D(CASES_LIST_TYPE_COMBOBOX)\
	CASES_LIST_TYPE_D(CASES_LIST_TYPE_DATE)\
	CASES_LIST_TYPE_D(CASES_LIST_TYPE_TEXT)\
	CASES_LIST_TYPE_D(CASES_LIST_TYPE_ZFORMULA)\
	CASES_LIST_TYPE_D(CASES_LIST_TYPE_XRAY)\
	CASES_LIST_TYPE_D(CASES_LIST_TYPE_PLANLECHENIYA)\
	CASES_LIST_TYPE_D(CASES_LIST_TYPE_BILL)

BEGIN_ENUM(CASES_LIST_TYPE) 
#define CASES_LIST_TYPE_D(element) element, 
	CASES_LIST_TYPES
#undef CASES_LIST_TYPE_D 
END_ENUM(CASES_LIST_TYPE)

BEGIN_ENUM_STRING(CASES_LIST_TYPE) 
#define CASES_LIST_TYPE_D(element) #element, 
	CASES_LIST_TYPES
#undef CASES_LIST_TYPE_D 
END_ENUM_STRING(CASES_LIST_TYPE)	

/* case_list */
static const char *
_prozubi_cases_list_string =
"["
	"["
		"\"Вид осмотра\", " TO_STR("CASENAME")", "TO_STR("CASES_LIST_TYPE_COMBOBOX")", "
			"[\"Первичный приём\", \"Текущий приём\", \"Этапный осмотр\"]"
	"],"
	"["
		"\"дата\", " TO_STR("CASEDATE")", "TO_STR("CASES_LIST_TYPE_DATE")
	"],"
	"["
		"\"жалобы\", " TO_STR("CASEZHALOBI")", "TO_STR("CASES_LIST_TYPE_TEXT")
	"],"	
	"["
		"\"анамнез жизни\", " TO_STR("CASEANAMNEZVITAE")", "TO_STR("CASES_LIST_TYPE_TEXT")
	"],"		
	"["
		"\"анамнез заболевания\", " TO_STR("CASEANAMNEZMORBI")", "TO_STR("CASES_LIST_TYPE_TEXT")
	"],"	
	"["
		"\"аллергический анамнез\", " TO_STR("CASEALLERGANAMNEZ")", "TO_STR("CASES_LIST_TYPE_TEXT")
	"],"	
	"{"
		"\"parent\": \"Общий осмотр\","
		"\"children\": "
			"["
				"["
					"\"состояние\", " TO_STR("CASESOSTOYANIYE")", "TO_STR("CASES_LIST_TYPE_COMBOBOX")", " 
						"[\"удовлетворительное\", \"средней степени тяжести\", \"тяжёлое\"]"
					
				"],"
				"["
					"\"сознание\", " TO_STR("CASESOSOZNANIYE")", "TO_STR("CASES_LIST_TYPE_COMBOBOX")", " 
						"[\"ясное\", \"спутанное\", \"изменённое\", \"ступор\", \"сопор\"]"
					
				"],"			
				"["
					"\"положение\", " TO_STR("CASEPOLOZHENIYE")", "TO_STR("CASES_LIST_TYPE_COMBOBOX")", " 
						"[\"активное\", \"пассивное\", \"вынужденное\"]"
					
				"],"				
				"["
					"\"со стороны внутренних органов\",  " TO_STR("CASESOSTORONIVNUTRENNIHORGANOV")", "TO_STR("CASES_LIST_TYPE_TEXT")
				"]"	
			"]"
	"},"
	"["
		"\"местно\", " TO_STR("CASEMESTNO")", "TO_STR("CASES_LIST_TYPE_TEXT")
	"],"	
	"["
		"\"зубная формула\", \"\", "TO_STR("CASES_LIST_TYPE_ZFORMULA")
	"],"
	"["
		"\"инструментальные и рентгенологические методы исследования\",  " TO_STR("CASEINSTRUMENTANDRENTGEN")", "TO_STR("CASES_LIST_TYPE_TEXT")
	"],"		
	"["
		"\"прикрепить фото/рентген\", \"\", "TO_STR("CASES_LIST_TYPE_XRAY")
	"],"		
	"["
		"\"диагноз\",  " TO_STR("CASEDIAGNOZIS")", "TO_STR("CASES_LIST_TYPE_TEXT")
	"],"		
	"["
		"\"план лечения\",  " TO_STR("CASEPLANLECHENIYA")", "TO_STR("CASES_LIST_TYPE_PLANLECHENIYA")
	"],"		
	"["
		"\"проведено лечение\", " TO_STR("CASELECHENIYE")", "TO_STR("CASES_LIST_TYPE_TEXT")
	"],"		
	"["
		"\"рекомендации\",  " TO_STR("CASERECOMENDACII")", "TO_STR("CASES_LIST_TYPE_TEXT")
	"],"		
	"["
		"\"Следующий визит\", " TO_STR("CASEDATEOFNEXT")", "TO_STR("CASES_LIST_TYPE_DATE")
	"],"		
	"["
		"\"Выставить счёт\",  " TO_STR("CASEBILL")", "TO_STR("CASES_LIST_TYPE_BILL")
	"]"		
"]";

#define ZUBFORMULA_TYPES \
	ZUBFORMULA_TYPE_DEF(ZUBFORMULA_TYPE_N,  "норма",        "")\
	ZUBFORMULA_TYPE_DEF(ZUBFORMULA_TYPE_C,  "кариес",      "C")\
	ZUBFORMULA_TYPE_DEF(ZUBFORMULA_TYPE_P,  "пульпит",     "P")\
	ZUBFORMULA_TYPE_DEF(ZUBFORMULA_TYPE_Pt, "периодонтит", "Pt")\
	ZUBFORMULA_TYPE_DEF(ZUBFORMULA_TYPE_0,  "отсутствует", "0")\
	ZUBFORMULA_TYPE_DEF(ZUBFORMULA_TYPE_Pl, "пломба",      "П")\
	ZUBFORMULA_TYPE_DEF(ZUBFORMULA_TYPE_K,  "коронка",     "К")\
	ZUBFORMULA_TYPE_DEF(ZUBFORMULA_TYPE_I,  "искусств",    "И")\
	ZUBFORMULA_TYPE_DEF(ZUBFORMULA_TYPE_R,  "корень",      "R")

BEGIN_ENUM(ZUBFORMULA_TYPE) 
#define ZUBFORMULA_TYPE_DEF(name, title, abbr) DECL_ENUM_ELEMENT(name), 
	ZUBFORMULA_TYPES
#undef ZUBFORMULA_TYPE_DEF
	DECL_ENUM_ELEMENT(ZUBFORMULA_TYPE_COLS_NUM),
END_ENUM(ZUBFORMULA_TYPE)

BEGIN_ENUM_STRING(ZUBFORMULA_TYPE)
#define ZUBFORMULA_TYPE_DEF(name, title, abbr) DECL_ENUM_STRING_ELEMENT(name), 
	ZUBFORMULA_TYPES
#undef ZUBFORMULA_TYPE_DEF
END_ENUM_STRING(ZUBFORMULA_TYPE)	

struct ZUBFORMULA_TYPE_t {
	ZUBFORMULA_TYPE type;
	const char *title;
	const char *attr;
};

static struct ZUBFORMULA_TYPE_t ZUBFORMULA_TYPE_ARRAY[] =
{
#define ZUBFORMULA_TYPE_DEF(name, title, abbr) {name, title, abbr}, 
	ZUBFORMULA_TYPES
#undef ZUBFORMULA_TYPES	
	-1
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
	CASES_COLUMN_TEXT(soznaniye, CASESOSOZNANIYE, "ZSOZNANIYE")\
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
	cJSON     *case_list; /* case structure */ 

#define CASES_COLUMN_DATE(member, number, title) time_t member; 
#define CASES_COLUMN_TEXT(member, number, title) char * member; size_t len_##member; 
#define CASES_COLUMN_DATA(member, number, title, type) type * member; size_t len_##member; 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA
};


BEGIN_ENUM(CASES) 
#define CASES_COLUMN_DATE(member, number, title) DECL_ENUM_ELEMENT(number), 
#define CASES_COLUMN_TEXT(member, number, title) DECL_ENUM_ELEMENT(number), 
#define CASES_COLUMN_DATA(member, number, title, type) DECL_ENUM_ELEMENT(number), 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA

	DECL_ENUM_ELEMENT(CASES_COLS_NUM),
END_ENUM(CASES)

BEGIN_ENUM_STRING(CASES)
#define CASES_COLUMN_DATE(member, number, title      ) DECL_ENUM_STRING_ELEMENT(number), 
#define CASES_COLUMN_TEXT(member, number, title      ) DECL_ENUM_STRING_ELEMENT(number), 
#define CASES_COLUMN_DATA(member, number, title, type) DECL_ENUM_STRING_ELEMENT(number), 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA
END_ENUM_STRING(CASES)	

static void	
prozubi_cases_table_init(struct kdata2_table **cases){
	kdata2_table_init(cases, CASES_TABLENAME,

#define CASES_COLUMN_DATE(member, number, title      ) KDATA2_TYPE_NUMBER, title, 
#define CASES_COLUMN_TEXT(member, number, title      ) KDATA2_TYPE_TEXT, title, 
#define CASES_COLUMN_DATA(member, number, title, type) KDATA2_TYPE_DATA, title, 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA
			
			NULL); 
} 

static cJSON *
_prozubi_cases_list_new(
		struct case_t *c,
		cJSON *cases_list_children
		)
{
	
	if (!c){
		ERR("%s", "case_t is NULL");
		return NULL;
	}	

	cJSON *json = cJSON_CreateObject();
	if (!json){
		ERR("%s", "can't create cJSON");
		return NULL;
	}
	
	/* add case id */
	cJSON_AddItemToObject(json, "id", cJSON_CreateString(c->id));

	/* get date */
	struct tm tm;
	sec_to_tm(c->date, &tm);
	char date[11];
	strftime(date, 11, "%d.%m.%Y", &tm);

	/* set title */
	char title[BUFSIZ] = "";
	strcat(title, date);
	if (c->name){
		strcat(title, " - ");
		strncat(title, c->name, 128);
	}
	if (c->diagnozis){
		strcat(title, " - ");
		strncat(title, c->diagnozis, 512);
	}	

	/* add title to json */
	cJSON_AddItemToObject(json, "parent", cJSON_CreateString(title));

	/* add cases_list_string */
	cJSON_AddItemToObject(json, "children", cases_list_children);

	return json;
}

/* allocate and init new case */
static struct case_t *
_prozubi_case_new(){
	/* allocate case_t */
	struct case_t *c = NEW(struct case_t, 
			ERR("%s", "can't allocate struct case_t"), return NULL);
	
	/* init values to NULL */
#define CASES_COLUMN_DATE(member, number, title      ) c->member = time(NULL); 
#define CASES_COLUMN_TEXT(member, number, title      ) c->member = NULL; 
#define CASES_COLUMN_DATA(member, number, title, type) c->member = NULL; 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA
	
	return c;
}

static struct case_t *
prozubi_case_new_for_patient(kdata2_t *kdata, char patientid[37]){
	/* allocate case_t */
	struct case_t *c = _prozubi_case_new();

	/* get last case pro patient - to copy data from it */
	char SQL[BUFSIZ];
	strcat(SQL, "SELECT ");
#define CASES_COLUMN_DATE(member, number, title      ) strcat(SQL, title); strcat(SQL, ", "); 
#define CASES_COLUMN_TEXT(member, number, title      ) strcat(SQL, title); strcat(SQL, ", "); 
#define CASES_COLUMN_DATA(member, number, title, type) strcat(SQL, title); strcat(SQL, ", "); 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA			
	
	strcat(SQL, "ZRECORDNAME, MAX(ZDATE) FROM ");
	strcat(SQL, CASES_TABLENAME);
	strcat(SQL, STR(" WHERE ZPATIENTID = '%s'", patientid));

	/* start SQLite request */
	int res;
	sqlite3_stmt *stmt;
	
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db));	
		return NULL;
	}	

	bool no_rows = true;
	while (sqlite3_step(stmt) != SQLITE_DONE) {
		no_rows = false;
	
		/* iterate columns */
		int i;
		for (i = 0; i < CASES_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define CASES_COLUMN_DATE(member, number, title) \
				case number:\
				{\
					c->member = time(NULL);\
					break;\
				}; 
				
#define CASES_COLUMN_TEXT(member, number, title) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					if (value){\
						char *str = MALLOC(len + 1,\
							ERR("can't allocate string with len: %ld", len+1), break);\
						strncpy(str, (const char *)value, len);\
						str[len] = 0;\
						c->member = str;\
						c->len_##member = len;\
					} else {\
						c->member = NULL;\
						c->len_##member = -1;\
					}\
					break;\
				}; 

#define CASES_COLUMN_DATA(member, number, title, type) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const void *value = sqlite3_column_blob(stmt, i);\
					if (value){\
						if (CASES_DATA_TYPE_##type == CASES_DATA_TYPE_cJSON){\
							cJSON *json = cJSON_ParseWithLength(value, len);\
							c->member = json;\
							c->len_##member = -1;\
						}\
					} else {\
						c->member = NULL;\
						c->len_##member = -1;\
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

	if (no_rows){
		/* add default values if now rows */
		c->date = time(NULL);
		c->dateofnext = time(NULL);
		c->name = strdup("Первичный приём");
		c->zhalobi = strdup("активно не предъявляет");
		c->sostoyanie = strdup("удовлетворительное");
		c->soznaniye = strdup("ясное");
		c->polozheniye = strdup("активное");
		c->patientid = strdup(patientid);
	}

	/*write data to SQL */
	int i;
	for (i = 0; i < CASES_COLS_NUM; ++i) {
		switch (i) {
				
#define CASES_COLUMN_DATE(member, number, title) \
				case number:\
				{\
					kdata2_set_number_for_uuid(kdata, CASES_TABLENAME, title,\
						   	c->member, c->id);\
					break;\
				}; 
				
#define CASES_COLUMN_TEXT(member, number, title) \
				case number:\
				{\
					char *str = c->member;\
					if (!str) str = "";\
					kdata2_set_text_for_uuid(kdata, CASES_TABLENAME, title,\
						   	str, c->id);\
					break;\
				}; 

#define CASES_COLUMN_DATA(member, number, title, type) \
				case number:\
				{\
					if (c->member){\
						kdata2_set_data_for_uuid(kdata, CASES_TABLENAME, title,\
						   	c->member, c->len_##member, c->id);\
					}\
					break;\
				}; 

			CASES_COLUMNS

#undef CASES_COLUMN_DATE			
#undef CASES_COLUMN_TEXT			

			default:
				break;
		}
	}


	return c;
}


/* callback all cases with patientid (set patient id to NULL to get all cases from database) */
static void 
prozubi_cases_foreach(
		kdata2_t   *kdata,
		const char *patient_id,
		void       *user_data,
		int        (*callback)(void *user_data, struct case_t *c)
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

	/* get cases list children */
	cJSON *cases_list_children = cJSON_Parse(_prozubi_cases_list_string);
	if (!cases_list_children) {
		ERR("%s", "can't get cJSON from _prozubi_cases_list_string");
		return;
	}
	
	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define CASES_COLUMN_DATE(member, number, title      ) strcat(SQL, title); strcat(SQL, ", "); 
#define CASES_COLUMN_TEXT(member, number, title      ) strcat(SQL, title); strcat(SQL, ", "); 
#define CASES_COLUMN_DATA(member, number, title, type) strcat(SQL, title); strcat(SQL, ", "); 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, CASES_TABLENAME);
	if (patient_id)
		strcat(SQL, STR(" WHERE ZPATIENTID = '%s'", patient_id));
	strcat(SQL, " ORDER BY ZDATE ASC");

	/* start SQLite request */
	int res;
	sqlite3_stmt *stmt;
	
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db));	
		return;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
	
		/* allocate and init case_t */
		struct case_t *c = _prozubi_case_new();

		/* iterate columns */
		int i;
		for (i = 0; i < CASES_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define CASES_COLUMN_DATE(member, number, title) \
				case number:\
				{\
					int col_type = sqlite3_column_type(stmt, i);\
					fprintf(stderr, "COLL_TYPE: %d\n", col_type);\
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
						char *str = MALLOC(len + 1,\
							ERR("can't allocate string with len: %ld", len+1), break);\
						strncpy(str, (const char *)value, len);\
						str[len] = 0;\
						c->member = str;\
						c->len_##member = len;\
					} else {\
						c->member = NULL;\
						c->len_##member = -1;\
					}\
					break;\
				}; 

#define CASES_COLUMN_DATA(member, number, title, type) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const void *value = sqlite3_column_blob(stmt, i);\
					if (value){\
						if (CASES_DATA_TYPE_##type == CASES_DATA_TYPE_cJSON){\
							cJSON *json = cJSON_ParseWithLength(value, len);\
							c->member = json;\
							c->len_##member = -1;\
						}\
					} else {\
						c->member = NULL;\
						c->len_##member = -1;\
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
		
		/* handle case id */
		const unsigned char *value = sqlite3_column_text(stmt, i);				
		if (value){
			strncpy(c->id, (const char *)value, sizeof(c->id) - 1);
			c->id[sizeof(c->id) - 1] = 0;		
		}

		/* handle cases list */
		c->case_list = _prozubi_cases_list_new(c, cases_list_children);
		if (!c->case_list){
			ERR("%s", "can't get _prozubi_cases_list_new");
			break;
		}

		/* callback */
		if (callback)
			if (callback(user_data, c))
				break;		
	}	
	
	sqlite3_finalize(stmt);
}

static void
prozubi_case_free(struct case_t *c){
	if (c){

#define CASES_COLUMN_DATE(member, number, title) 
#define CASES_COLUMN_TEXT(member, number, title) if(c->member) free(c->member); 
#define CASES_COLUMN_DATA(member, number, title, type)\
		if (CASES_DATA_TYPE_##type == CASES_DATA_TYPE_cJSON){\
			if(c->member) cJSON_free(c->member);\
		}
		CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA			
		
		free(c);
		c = NULL;
	}
}


#define CASES_COLUMN_DATE(member, number, title)\
	static time_t prozubi_case_get_##number(struct case_t *c){\
		return c->member;\
	}
#define CASES_COLUMN_TEXT(member, number, title)\
	static char * prozubi_case_get_##number(struct case_t *c){\
		return c->member;\
	}\
	static size_t prozubi_case_get_len_##number(struct case_t *c){\
		return c->len_##member;\
	}	
   	
#define CASES_COLUMN_DATA(member, number, title, type)\
	static void * prozubi_case_get_##number(struct case_t *c){\
		return c->member;\
	}\
	static size_t prozubi_case_get_len_##number(struct case_t *c){\
		return c->len_##member;\
	}	
	
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA


static void * 
prozubi_case_get(struct case_t *c, CASES key){
	switch (key) {
#define CASES_COLUMN_DATA(member, number, title, type)\
		case number:\
			{\
				return c->member;\
			}		
#define CASES_COLUMN_DATE(member, number, title)\
		case number:\
			{\
				return &(c->member);\
			}		
#define CASES_COLUMN_TEXT(member, number, title)\
		case number:\
			{\
				return c->member;\
			}
		CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA			
		default:
			break;
	}
	
	return NULL;
}

static size_t 
prozubi_case_get_len(struct case_t *c, const char *name){
	int index = getIndexCASES(name);	
	if (index == -1)
		return 0;

	switch (index) {
#define CASES_COLUMN_DATA(member, number, title, type)\
		case number:\
			{\
				return c->len_##member;\
			}		
#define CASES_COLUMN_DATE(member, number, title)\
		case number:\
			{\
				return 0;\
			}		
#define CASES_COLUMN_TEXT(member, number, title)\
		case number:\
			{\
				return c->len_##member;\
			}
		CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA			
	}
	
	return 0;
}

static void
prozubi_cases_list_foreach(
		struct case_t *c,
		void * user_data,
		void * (*item_callback)(
			void *user_data,
			struct case_t *c,
			void * parent,
			bool has_children,
			char * title,
			enum tagCASES key,
			enum tagCASES_LIST_TYPE type,
			char ** array
			)	
		)
{
	cJSON *json = c->case_list;
	if (!cJSON_IsObject(json)){
		ERR("%s", "error to read json");
		return;
	}	
	cJSON *jparent = cJSON_GetObjectItem(json, "parent");
	char *title = cJSON_GetStringValue(jparent); 
	void *parent = item_callback(user_data, c, NULL, true, title, -1, -1, NULL);
	
	cJSON *root = cJSON_GetObjectItem(json, "children");
	if (!cJSON_IsArray(root)){
		ERR("%s", "error to read json");
		return;
	}

	cJSON *element;
	cJSON_ArrayForEach(element, root){
		if (cJSON_IsArray(element)){
			/* handle array */
			cJSON *jtitle = cJSON_GetArrayItem(element, 0);
			char *el_title = cJSON_GetStringValue(jtitle);

			cJSON *jkey = cJSON_GetArrayItem(element, 1); 
			char  *skey = cJSON_GetStringValue(jkey); 
			int key = getIndexCASES(skey);	

			cJSON *jtype = cJSON_GetArrayItem(element, 2); 
			char  *stype = cJSON_GetStringValue(jtype); 
			int type = getIndexCASES_LIST_TYPE(stype);	
			
			char ** array = NULL;
			if (type == CASES_LIST_TYPE_COMBOBOX){
				cJSON *jarray = cJSON_GetArrayItem(element, 3); 
				array = MALLOC(8*10, ERR("%s", "can't allocate memory"), break);
				cJSON *item; int i = 0;
				cJSON_ArrayForEach(item, jarray){
					array[i++] = cJSON_GetStringValue(item); 
				}
				array[i] = NULL; // NULL-terminate array
			}
			item_callback(user_data, c, parent, false, el_title, key, type, array);

		} else if (cJSON_IsObject(element)){
			/* handle object */
			cJSON *jparent = cJSON_GetObjectItem(element, "parent");
			char *el_title = cJSON_GetStringValue(jparent); 
			void *new_parent = item_callback(user_data, c, parent, true, el_title, -1, -1, NULL);
			
			cJSON *child = cJSON_GetObjectItem(element, "children");
			cJSON *child_element;
			cJSON_ArrayForEach(child_element, child){
				if (cJSON_IsArray(child_element)){
					cJSON *jtitle = cJSON_GetArrayItem(child_element, 0);
					char *ctitle = cJSON_GetStringValue(jtitle);

					cJSON *jkey = cJSON_GetArrayItem(child_element, 1); 
					char  *skey = cJSON_GetStringValue(jkey); 
					int key = getIndexCASES(skey);	

					cJSON *jtype = cJSON_GetArrayItem(child_element, 2); 
					char  *stype = cJSON_GetStringValue(jtype); 
					int type = getIndexCASES_LIST_TYPE(stype);	
					
					char ** array = NULL;
					if (type == CASES_LIST_TYPE_COMBOBOX){
						cJSON *jarray = cJSON_GetArrayItem(child_element, 3); 
						array = MALLOC(8*10, ERR("%s", "can't allocate memory"), break);
						cJSON *item; int i = 0;
						cJSON_ArrayForEach(item, jarray){
							array[i++] = cJSON_GetStringValue(item); 
						}
						array[i] = NULL; // NULL-terminate array
					}
					item_callback(user_data, c, new_parent, false, ctitle, key, type, array);
				}
			}
		}
	}
}

#define CASES_COLUMN_DATE(member, number, title)\
static int prozubi_case_set_##number (kdata2_t *p, struct case_t *c, time_t t){\
	if (kdata2_set_number_for_uuid(p, CASES_TABLENAME, title, t, c->id))\
		return -1;\
	c->member = t;\
	return 0;\
}
#define CASES_COLUMN_DATA(member, number, title, type)\
static int prozubi_case_set_##number (kdata2_t *p, struct case_t *c,\
	   	type *data, size_t len)\
{\
	if (CASES_DATA_TYPE_##type == CASES_DATA_TYPE_cJSON){\
		char *str = cJSON_Print(data);\
		if (str){\
			if (kdata2_set_data_for_uuid(p, CASES_TABLENAME, title,\
					(void *)str, strlen(str), c->id))\
			{\
				free(str);\
				return -1;\
			}\
			if(c->member)\
				cJSON_free(c->member);\
			c->member = data;\
			c->len_##member = -1;\
			free(str);\
		}\
	}\
	return 0;\
}
#define CASES_COLUMN_TEXT(member, number, title)\
static int prozubi_case_set_##number (kdata2_t *p, struct case_t *c, const char *text){\
	if (kdata2_set_text_for_uuid(p, CASES_TABLENAME, title, text, c->id))\
		return -1;\
	if(c->member)\
		free(c->member);\
	size_t len = strlen(text);\
   	c->member = MALLOC(len + 1, ERR("can't allocate size: %ld", len + 1), return -1);\
	strncpy(c->member, text, len);\
	c->len_##member = len;\
	return 0;\
}
		CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA			

static int prozubi_case_set_text(
		CASES key, kdata2_t *p, struct case_t *c, const char *text)
{
	switch (key) {
#define CASES_COLUMN_DATE(member, number, title) case number: break;	
#define CASES_COLUMN_DATA(member, number, title, type) case number: break;	
#define CASES_COLUMN_TEXT(member, number, title) case number:\
		return prozubi_case_set_##number(p, c, text);\
		
		CASES_COLUMNS

#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA	
		
		default:
			break;
	}
	return -1;
}

static int prozubi_case_set_date(
		CASES key, kdata2_t *p, struct case_t *c, time_t t)
{
	switch (key) {
#define CASES_COLUMN_TEXT(member, number, title) case number: break;	
#define CASES_COLUMN_DATA(member, number, title, type) case number: break;	
#define CASES_COLUMN_DATE(member, number, title) case number:\
		return prozubi_case_set_##number(p, c, t);\
		
		CASES_COLUMNS

#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA	
		
		default:
			break;
	}
	return -1;
}

static int prozubi_case_set_data(
		CASES key, kdata2_t *p, struct case_t *c, void *data, size_t len)
{
	switch (key) {
#define CASES_COLUMN_TEXT(member, number, title) case number: break;	
#define CASES_COLUMN_DATE(member, number, title) case number: break;	
#define CASES_COLUMN_DATA(member, number, title, type) case number:\
		return prozubi_case_set_##number(p, c, data, len);\
		
		CASES_COLUMNS

#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA	
		
		default:
			break;
	}
	return -1;
}

static int prozubi_case_update(
		kdata2_t *p, struct case_t *c
		)
{
	int i;
	for (i = 0; i < CASES_COLS_NUM; ++i) {
		switch (i) {
				
#define CASES_COLUMN_DATE(member, number, title) \
				case number:\
				{\
					kdata2_set_number_for_uuid(p, CASES_TABLENAME, title,\
						   	c->member, c->id);\
					break;\
				}; 
				
#define CASES_COLUMN_TEXT(member, number, title) \
				case number:\
				{\
					char *str = c->member;\
					if (!str) str = "";\
					kdata2_set_text_for_uuid(p, CASES_TABLENAME, str,\
						   	c->member, c->id);\
					break;\
				}; 

#define CASES_COLUMN_DATA(member, number, title, type) \
				case number:\
				{\
					if (c->member){\
						prozubi_case_set_##number(p, c, c->member, c->len_##member);\
					}\
					break;\
				}; 

			CASES_COLUMNS

#undef CASES_COLUMN_DATE			
#undef CASES_COLUMN_TEXT			

			default:
				break;
		}
	}

	return 0;
}

static int prozubi_case_remove(
		kdata2_t *p, struct case_t *c
		)
{
	return kdata2_remove_for_uuid(p, CASES_TABLENAME, c->id);
}




#endif /* ifndef CASES_H */
