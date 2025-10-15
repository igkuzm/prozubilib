/**
 * File              : cases.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 06.05.2023
 * Last Modified Date: 01.10.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef CASES_H
#define CASES_H

#ifndef UUIDCOLUMN
#define UUIDCOLUMN "ZRECORDNAME"
#endif /* ifndef UUIDCOLUMN */

#include "prozubilib_conf.h"

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
#define CASES_LIST_TYPE_D(element) (char*)#element, 
	CASES_LIST_TYPES
#undef CASES_LIST_TYPE_D 
END_ENUM_STRING(CASES_LIST_TYPE)	

/* case_list */
static const char *
_prozubi_cases_list_string =
"["
	"["
		"\"Вид осмотра\", " TO_STR("CASENAME")", " TO_STR("CASES_LIST_TYPE_COMBOBOX")", "
			"[\"Первичный приём\", \"Текущий приём\", \"Этапный осмотр\"]"
	"],"
	"["
		"\"дата\", " TO_STR("CASEDATE")", " TO_STR("CASES_LIST_TYPE_DATE")
	"],"
	"["
		"\"жалобы\", " TO_STR("CASEZHALOBI")", " TO_STR("CASES_LIST_TYPE_TEXT")
	"],"	
	"["
		"\"анамнез жизни\", " TO_STR("CASEANAMNEZVITAE")", " TO_STR("CASES_LIST_TYPE_TEXT")
	"],"		
	"["
		"\"анамнез заболевания\", " TO_STR("CASEANAMNEZMORBI")", " TO_STR("CASES_LIST_TYPE_TEXT")
	"],"	
	"["
		"\"аллергический анамнез\", " TO_STR("CASEALLERGANAMNEZ")", " TO_STR("CASES_LIST_TYPE_TEXT")
	"],"	
	"{"
		"\"parent\": \"Общий осмотр\","
		"\"children\": "
			"["
				"["
					"\"состояние\", " TO_STR("CASESOSTOYANIYE")", " TO_STR("CASES_LIST_TYPE_COMBOBOX")", " 
						"[\"удовлетворительное\", \"средней степени тяжести\", \"тяжёлое\"]"
					
				"],"
				"["
					"\"сознание\", " TO_STR("CASESOSOZNANIYE")", " TO_STR("CASES_LIST_TYPE_COMBOBOX")", " 
						"[\"ясное\", \"спутанное\", \"изменённое\", \"ступор\", \"сопор\"]"
					
				"],"			
				"["
					"\"положение\", " TO_STR("CASEPOLOZHENIYE")", " TO_STR("CASES_LIST_TYPE_COMBOBOX")", " 
						"[\"активное\", \"пассивное\", \"вынужденное\"]"
					
				"],"				
				"["
					"\"со стороны внутренних органов\",  " TO_STR("CASESOSTORONIVNUTRENNIHORGANOV")", " TO_STR("CASES_LIST_TYPE_TEXT")
				"]"	
			"]"
	"},"
	"["
		"\"местно\", " TO_STR("CASEMESTNO")", " TO_STR("CASES_LIST_TYPE_TEXT")
	"],"	
	"["
		"\"зубная формула\", \"\", " TO_STR("CASES_LIST_TYPE_ZFORMULA")
	"],"
	"["
		"\"инструментальные и рентгенологические методы исследования\",  " TO_STR("CASEINSTRUMENTANDRENTGEN")", " TO_STR("CASES_LIST_TYPE_TEXT")
	"],"		
	"["
		"\"прикрепить фото/рентген\", \"\", " TO_STR("CASES_LIST_TYPE_XRAY")
	"],"		
	"["
		"\"диагноз\",  " TO_STR("CASEDIAGNOZIS")", " TO_STR("CASES_LIST_TYPE_TEXT")
	"],"		
	"["
		"\"план лечения\",  " TO_STR("CASEPLANLECHENIYA")", " TO_STR("CASES_LIST_TYPE_PLANLECHENIYA")
	"],"		
	"["
		"\"проведено лечение\", " TO_STR("CASELECHENIYE")", " TO_STR("CASES_LIST_TYPE_TEXT")
	"],"		
	"["
		"\"рекомендации\",  " TO_STR("CASERECOMENDACII")", " TO_STR("CASES_LIST_TYPE_TEXT")
	"],"		
	"["
		"\"Следующий визит\", " TO_STR("CASEDATEOFNEXT")", " TO_STR("CASES_LIST_TYPE_DATE")
	"],"		
	"["
		"\"Выставить счёт\",  " TO_STR("CASEBILL")", " TO_STR("CASES_LIST_TYPE_BILL")
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
	DECL_ENUM_ELEMENT(ZUBFORMULA_TYPES_NUM),
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
};
	
#define ZUBFORMULA_TEETH_UP\
	ZUBFORMULA_TOOTH_UP(Z18)\
	ZUBFORMULA_TOOTH_UP(Z17)\
	ZUBFORMULA_TOOTH_UP(Z16)\
	ZUBFORMULA_TOOTH_UP(Z15)\
	ZUBFORMULA_TOOTH_UP(Z14)\
	ZUBFORMULA_TOOTH_UP(Z13)\
	ZUBFORMULA_TOOTH_UP(Z12)\
	ZUBFORMULA_TOOTH_UP(Z11)\
	ZUBFORMULA_TOOTH_UP(Z21)\
	ZUBFORMULA_TOOTH_UP(Z22)\
	ZUBFORMULA_TOOTH_UP(Z23)\
	ZUBFORMULA_TOOTH_UP(Z24)\
	ZUBFORMULA_TOOTH_UP(Z25)\
	ZUBFORMULA_TOOTH_UP(Z26)\
	ZUBFORMULA_TOOTH_UP(Z27)\
	ZUBFORMULA_TOOTH_UP(Z28)\

#define ZUBFORMULA_TEETH_DOWN\
	ZUBFORMULA_TOOTH_DOWN(Z48)\
	ZUBFORMULA_TOOTH_DOWN(Z47)\
	ZUBFORMULA_TOOTH_DOWN(Z46)\
	ZUBFORMULA_TOOTH_DOWN(Z45)\
	ZUBFORMULA_TOOTH_DOWN(Z44)\
	ZUBFORMULA_TOOTH_DOWN(Z43)\
	ZUBFORMULA_TOOTH_DOWN(Z42)\
	ZUBFORMULA_TOOTH_DOWN(Z41)\
	ZUBFORMULA_TOOTH_DOWN(Z31)\
	ZUBFORMULA_TOOTH_DOWN(Z32)\
	ZUBFORMULA_TOOTH_DOWN(Z33)\
	ZUBFORMULA_TOOTH_DOWN(Z34)\
	ZUBFORMULA_TOOTH_DOWN(Z35)\
	ZUBFORMULA_TOOTH_DOWN(Z36)\
	ZUBFORMULA_TOOTH_DOWN(Z37)\
	ZUBFORMULA_TOOTH_DOWN(Z38)\
	
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
DECL_ENUM_ELEMENT(CASES_COLS_NUM), END_ENUM(CASES)

BEGIN_ENUM_STRING(CASES)
#define CASES_COLUMN_DATE(member, number, title)  DECL_ENUM_STRING_ELEMENT(number),
#define CASES_COLUMN_TEXT(member, number, title)  DECL_ENUM_STRING_ELEMENT(number),
#define CASES_COLUMN_DATA(member, number, title, type) DECL_ENUM_STRING_ELEMENT(number),
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA
END_ENUM_STRING(CASES)


static const char *gn_CASES[] = {
#define CASES_COLUMN_DATE(member, number, title) (const char *)#member,
#define CASES_COLUMN_TEXT(member, number, title) (const char *)#member,
#define CASES_COLUMN_DATA(member, number, title, type) (const char *)#member,
   CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA
   NULL
};											   

extern const char EXPORTDLL * 
getNameCASES(enum tagCASES index);

extern void	EXPORTDLL
prozubi_cases_table_init(struct kdata2_table **cases); 

extern cJSON EXPORTDLL *
_prozubi_cases_list_new(
		struct case_t *c,
		cJSON *cases_list_children
		);

/* allocate and init new case */
extern struct case_t EXPORTDLL *
_prozubi_case_new();

extern cJSON EXPORTDLL *
prozubi_planlecheniya_new(struct case_t *c);

extern cJSON EXPORTDLL *
prozubi_bill_new(struct case_t *c);

#define CASES_COLUMN_DATE(member, number, title)\
extern int EXPORTDLL prozubi_case_set_##number(\
		prozubi_t *p, struct case_t *c, time_t t);

#define CASES_COLUMN_DATA(member, number, title, type)\
extern int EXPORTDLL prozubi_case_set_##number(\
		prozubi_t *p, struct case_t *c,\
	   	type *data, size_t len);

#define CASES_COLUMN_TEXT(member, number, title)\
extern int EXPORTDLL prozubi_case_set_##number(\
		prozubi_t *p, struct case_t *c, const char *text);
		CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT			
#undef CASES_COLUMN_DATA			

extern struct case_t EXPORTDLL *
prozubi_case_new_for_patient(prozubi_t *p, char patientid[37]);

extern struct case_t EXPORTDLL *
prozubi_cases_from_sql(
		prozubi_t *p,
		sqlite3_stmt *stmt);

/* callback all cases with patientid (set patient id to 
 * NULL to get all cases from database) */
extern EXPORTDLL void 
prozubi_cases_foreach(
		prozubi_t  *p,
		const char *patient_id,
		const char *predicate,
		void       *user_data,
		int       (*callback)(void *user_data, struct case_t *c)
		);

extern EXPORTDLL void
prozubi_case_free(struct case_t *c);

#define CASES_COLUMN_DATE(member, number, title)\
	extern time_t EXPORTDLL prozubi_case_get_##number(\
			struct case_t *c);
	
#define CASES_COLUMN_TEXT(member, number, title)\
	extern char EXPORTDLL * prozubi_case_get_##number(\
			struct case_t *c);\
	extern size_t EXPORTDLL prozubi_case_get_len_##number(\
			struct case_t *c);

#define CASES_COLUMN_DATA(member, number, title, type)\
	extern void EXPORTDLL * prozubi_case_get_##number(\
			struct case_t *c);\
	extern size_t EXPORTDLL prozubi_case_get_len_##number(\
			struct case_t *c);
		
	CASES_COLUMNS
#undef CASES_COLUMN_DATE
#undef CASES_COLUMN_TEXT
#undef CASES_COLUMN_DATA


extern void EXPORTDLL * 
prozubi_case_get_for_key(struct case_t *c, CASES key);

extern size_t EXPORTDLL
prozubi_case_get_len(struct case_t *c, const char *name);

struct case_list_node {
	struct case_t *c;
	void *allocated_ptr;
	char * title;
	enum tagCASES key;
	enum tagCASES_LIST_TYPE type;
	char ** array;
};

extern struct case_list_node  EXPORTDLL * 
_case_list_node_new(
			prozubi_t *p,
			struct case_t *c,
			void *allocated_ptr,
			char * title,
			int key,
			int type,
			char ** array
		);

#define prozubi_case_list_node_array_foreach(\
		caseListNode, element)\
	char ** __ptr__ = \
		((struct case_list_node *)caseListNode)->array;\
	for (element = *__ptr__;\
		   *__ptr__; \
			 *__ptr__++, element = *__ptr__)

extern void EXPORTDLL
prozubi_case_list_node_free(\
		prozubi_t *p, struct case_list_node *n);

extern void EXPORTDLL
prozubi_cases_list_foreach(
		prozubi_t     *p,
		struct case_t *c,
		void * user_data,
		void * (*item_callback)(
			void *user_data,
			void * parent,
			struct case_list_node *n
			)	
		);

extern int EXPORTDLL prozubi_case_set_text(
		CASES key, prozubi_t *p, struct case_t *c, 
		const char *text);

extern int EXPORTDLL prozubi_case_set_date(
		CASES key, prozubi_t *p, struct case_t *c, time_t t);

extern int EXPORTDLL prozubi_case_set_data(
		CASES key, prozubi_t *p, struct case_t *c, 
		void *data, size_t len);

extern struct case_t EXPORTDLL * prozubi_case_get(
		prozubi_t *p, const char *uuid);

extern int EXPORTDLL prozubi_case_update(
		prozubi_t *p, struct case_t *c
		);

extern int EXPORTDLL prozubi_case_remove(
		prozubi_t *p, struct case_t *c);

extern void EXPORTDLL prozubi_case_list_node_free_with_case(
		prozubi_t *p, struct case_list_node *n);

/* convert zubformula to RTF string */
extern size_t EXPORTDLL prozubi_case_zubformula_to_rtf(
	prozubi_t *p, struct case_t *c, char **rtf);

#endif /* ifndef CASES_H */
