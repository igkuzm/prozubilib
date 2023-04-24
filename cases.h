/**
 * File              : cases.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 25.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef CASES_H
#define CASES_H

#include "prozubilib_conf.h"

#include "enum.h"
#include "alloc.h"
#include "date-time/time_local.h"

#include <string.h>
#include <stdlib.h>

#define CASES_TABLENAME "ZCASES"

#define TO_STR_(X) #X
#define TO_STR(X) TO_STR_(X)

/* case_list */
static const char *
_prozubi_cases_list_string =
"["
	"["
		"\"Вид осмотра\", " TO_STR("CASENAME")", \"comboBox\","
			"[\"Первичный приём\", \"Текущий приём\", \"Этапный осмотр\"]"
	"],"
	"["
		"\"дата\", " TO_STR("CASEDATE")", \"date\""
	"],"
	"["
		"\"жалобы\", " TO_STR("CASEZHALOBI")", \"text\""
	"],"	
	"["
		"\"анамнез жизни\", " TO_STR("CASEANAMNEZVITAE")", \"text\""
	"],"		
	"["
		"\"анамнез заболевания\", " TO_STR("CASEANAMNEZMORBI")", \"text\""
	"],"	
	"["
		"\"аллергический анамнез\", " TO_STR("CASEALLERGANAMNEZ")", \"text\""
	"],"	
	"{"
		"\"parent\": \"Общий осмотр\","
		"\"children\": "
			"["
				"["
					"\"состояние\", " TO_STR("CASESOSTOYANIYE")", \"comboBox\"," 
						"[\"удовлетворительное\", \"средней степени тяжести\", \"тяжёлое\"]"
					
				"],"
				"["
					"\"сознание\", " TO_STR("CASESOSOZNANIYE")", \"comboBox\"," 
						"[\"ясное\", \"спутанное\", \"изменённое\", \"ступор\", \"сопор\"]"
					
				"],"			
				"["
					"\"положение\", " TO_STR("CASEPOLOZHENIYE")", \"comboBox\"," 
						"[\"активное\", \"пассивное\", \"вынужденное\"]"
					
				"],"				
				"["
					"\"со стороны внутренних органов\",  " TO_STR("CASESOSTORONIVNUTRENNIHORGANOV")", \"text\""
				"]"	
			"]"
	"},"
	"["
		"\"местно\", " TO_STR("CASEMESTNO")", \"text\""
	"],"	
	"["
		"\"зубная формула\", \"\", \"zubnayaFormula\""
	"],"
	"["
		"\"инструментальные и рентгенологические методы исследования\",  " TO_STR("CASEINSTRUMENTANDRENTGEN")", \"text\""
	"],"		
	"["
		"\"прикрепить фото/рентген\", \"\", \"xray\""
	"],"		
	"["
		"\"диагноз\",  " TO_STR("CASEDIAGNOZIS")", \"text\""
	"],"		
	"["
		"\"план лечения\",  " TO_STR("CASEPLANLECHENIYA")", \"planLecheniya\""
	"],"		
	"["
		"\"проведено лечение\", " TO_STR("CASELECHENIYE")", \"text\""
	"],"		
	"["
		"\"рекомендации\",  " TO_STR("CASERECOMENDACII")", \"text\""
	"],"		
	"["
		"\"Следующий визит\", " TO_STR("CASEDATEOFNEXT")", \"date\""
	"],"		
	"["
		"\"Выставить счёт\",  " TO_STR("CASEBILL")", \"bill\""
	"]"		
"]";

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
{
#define CASES_COLUMN_DATE(member, number, title) DECL_ENUM_ELEMENT(number), 
#define CASES_COLUMN_TEXT(member, number, title) DECL_ENUM_ELEMENT(number), 
#define CASES_COLUMN_DATA(member, number, title, type) DECL_ENUM_ELEMENT(number), 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA

	DECL_ENUM_ELEMENT(CASES_COLS_NUM),
} 
END_ENUM(CASES)

BEGIN_ENUM_STRING(CASES)
{	
#define CASES_COLUMN_DATE(member, number, title) DECL_ENUM_STRING_ELEMENT(number), 
#define CASES_COLUMN_TEXT(member, number, title) DECL_ENUM_STRING_ELEMENT(number), 
#define CASES_COLUMN_DATA(member, number, title, type) DECL_ENUM_STRING_ELEMENT(number), 
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA
}
END_ENUM_STRING(CASES)	

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
	strftime(date, 11, "%m.%d.%Y", &tm);

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
prozubi_case_new(){
	/* allocate case_t */
	struct case_t *c = NEW(struct case_t, 
			ERR("%s", "can't allocate struct case_t"), NULL);
	
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

	/* allocate and init case_t */
	struct case_t *c = prozubi_case_new();
	if (!c)
		return;

	/* get cases list children */
	cJSON *cases_list_children = cJSON_Parse(_prozubi_cases_list_string);
	if (!cases_list_children) {
		ERR("%s", "can't get cJSON from _prozubi_cases_list_string");
		return;
	}
	
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
	if (patient_id)
		strcat(SQL, STR(" WHERE ZPATIENTID = '%s'", patient_id));

	/* start SQLite request */
	int res;
	sqlite3_stmt *stmt;
	
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db));	
		free(c);
		return;
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
						char *str = malloc(len + 1);\
						if (!str){\
							ERR("can't allocate string with len: %ld", len+1);\
							break;\
						}\
						strncpy(str, (const char *)value, len);\
						str[len] = 0;\
						c->member = str;\
						c->len_##member = len;\
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
#define CASES_COLUMN_DATA(member, number, title, type) if(c->member) free(c->member); 
		CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA			
		
		free(c);
	}
}


#define CASES_COLUMN_DATE(member, number, title)\
	time_t prozubi_case_get_##number(struct case_t *c, size_t *len){\
		*len = 0;\
		return c->member;\
	}
#define CASES_COLUMN_TEXT(member, number, title)\
	char * prozubi_case_get_##number(struct case_t *c, size_t *len){\
		*len = c->len_##member;\
		return c->member;\
	}
   	
#define CASES_COLUMN_DATA(member, number, title, type)\
	void * prozubi_case_get_##number(struct case_t *c, size_t *len){\
		*len = c->len_##member;\
		return c->member;\
	}\
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA

#define PROZUBI_CASE_GET(c, name)\
		prozubi_case_get_##name(c)

#endif /* ifndef CASES_H */
