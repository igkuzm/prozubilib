/**
 * File              : cases.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 21.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef CASES_H
#define CASES_H

#include "log.h"
#include "kdata_conf.h"
#include "kdata2/sqlite3.h"
#include "kdata2/cYandexDisk/cJSON.h"

#include <string.h>
#include <stdlib.h>

struct case_id {
	char   id[37];        /* uuid of the case */
	time_t date;          /* date and time of visit */
	char   name[128];     /* name of case */
	char   patientid[37]; /* patient uuid recordname */
};

/*! \struct cases_t
*  \brief every visit to clinic is a case
*
*  Detailed description
*/
struct case_t {
	char   id[37];       /* uuid of the case */
	time_t date;         /* date and time of visit */
	char   *name;        /* name of case */
	char   *patientid;   /* patient uuid recordname */
	
	time_t dateofnext;  /* next visit reserve */

	char   *z11; /* tooth number */
	char   *z12; /* tooth number */ 
	char   *z13; /* tooth number */
	char   *z14; /* tooth number */
	char   *z15; /* tooth number */
	char   *z16; /* tooth number */
	char   *z17; /* tooth number */
	char   *z18; /* tooth number */

	char   *z21; /* tooth number */
	char   *z22; /* tooth number */
	char   *z23; /* tooth number */
	char   *z24; /* tooth number */
	char   *z25; /* tooth number */
	char   *z26; /* tooth number */
	char   *z27; /* tooth number */
	char   *z28; /* tooth number */

	char   *z31; /* tooth number */
	char   *z32; /* tooth number */
	char   *z33; /* tooth number */
	char   *z34; /* tooth number */
	char   *z35; /* tooth number */
	char   *z36; /* tooth number */
	char   *z37; /* tooth number */
	char   *z38; /* tooth number */
	
	char   *z41; /* tooth number */
	char   *z42; /* tooth number */
	char   *z43; /* tooth number */
	char   *z44; /* tooth number */
	char   *z45; /* tooth number */
	char   *z46; /* tooth number */
	char   *z47; /* tooth number */
	char   *z48; /* tooth number */

	char   *alleganamnez; /* anamnez of allergy */
	char   *anamnezvitae; /* anamnez vitae */
	char   *anamnezmorbi; /* anamnez morbi */
	
	char   *diagnozis;    /* diagnozis */
	
	char   *instrumentandrentgen; /* instrumental and x-ray data */
	
	char   *lecheniye;    /* treatment text */
	
	char   *mestno;       /* local status */
	
	
	char   *polozheniye; 
	char   *recomendacii; 
	char   *sostoyanievnutrennihorganov; 
	char   *sostoyanie; 
	char   *zhalobi; 
	cJSON  *bill; 
	cJSON  *planlecheniya; 

} /* optional variable list */;


/*! \enum cases_column_number
*
*  Detailed description
*/
enum cases_column_number {
	ZCASEDATE = 0,
	ZCASEDATEOFNEXT,
	Z11, Z12, Z13, Z14, Z15, Z16, Z17, Z18,
	Z21, Z22, Z23, Z24, Z25, Z26, Z27, Z28,
    Z31, Z32, Z33, Z34, Z35, Z36, Z37, Z38,
    Z41, Z42, Z43, Z44, Z45, Z46, Z47, Z48,
	ZALLERGANAMNEZ,
	ZANAMNEZMORBI,
	ZANAMNEZVITAE,
	ZDIAGNOZIS,
	ZINSTRUMENTANDRENTGEN,
	ZLECHENIYE,
	ZMESTNO,
	ZCASENAME,
	ZCASEPATIENTID,
	ZPOLOZHENIYE,
	ZRECOMENDACII,
	ZSOSTORONIVNUTRENNIHORGANOV,
	ZSOSTOYANIYE,
	ZZHALOBI,
	ZBILL,
	ZPLANLECHENIYADATA,

	CASES_COLS_NUM,
};

struct cases_column {
	enum cases_column_number number;
	char name[32];	
	enum KDATA2_TYPE type;
};

struct cases_column cases_columns[] = 
{
	{ZCASEDATE,       "ZDATE",       KDATA2_TYPE_NUMBER},
	{ZCASEDATEOFNEXT, "ZDATEOFNEXT", KDATA2_TYPE_NUMBER},
	
	{Z11, "Z11", KDATA2_TYPE_TEXT},
	{Z12, "Z12", KDATA2_TYPE_TEXT},
	{Z13, "Z13", KDATA2_TYPE_TEXT},
	{Z14, "Z14", KDATA2_TYPE_TEXT},
	{Z15, "Z15", KDATA2_TYPE_TEXT},
	{Z16, "Z16", KDATA2_TYPE_TEXT},
	{Z17, "Z17", KDATA2_TYPE_TEXT},
	{Z18, "Z18", KDATA2_TYPE_TEXT},

	{Z21, "Z21", KDATA2_TYPE_TEXT},
	{Z22, "Z22", KDATA2_TYPE_TEXT},
	{Z23, "Z23", KDATA2_TYPE_TEXT},
	{Z24, "Z24", KDATA2_TYPE_TEXT},
	{Z25, "Z25", KDATA2_TYPE_TEXT},
	{Z26, "Z26", KDATA2_TYPE_TEXT},
	{Z27, "Z27", KDATA2_TYPE_TEXT},
	{Z28, "Z28", KDATA2_TYPE_TEXT},

	{Z31, "Z31", KDATA2_TYPE_TEXT},
	{Z32, "Z32", KDATA2_TYPE_TEXT},
	{Z33, "Z33", KDATA2_TYPE_TEXT},
	{Z34, "Z34", KDATA2_TYPE_TEXT},
	{Z35, "Z35", KDATA2_TYPE_TEXT},
	{Z36, "Z36", KDATA2_TYPE_TEXT},
	{Z37, "Z37", KDATA2_TYPE_TEXT},
	{Z38, "Z38", KDATA2_TYPE_TEXT},
	
	{Z41, "Z41", KDATA2_TYPE_TEXT},
	{Z42, "Z42", KDATA2_TYPE_TEXT},
	{Z43, "Z43", KDATA2_TYPE_TEXT},
	{Z44, "Z44", KDATA2_TYPE_TEXT},
	{Z45, "Z45", KDATA2_TYPE_TEXT},
	{Z46, "Z46", KDATA2_TYPE_TEXT},
	{Z47, "Z47", KDATA2_TYPE_TEXT},
	{Z48, "Z48", KDATA2_TYPE_TEXT},
	
	{ZALLERGANAMNEZ,              "ZALLERGANAMNEZ",              KDATA2_TYPE_TEXT},
	{ZANAMNEZVITAE,               "ZANAMNEZVITAE",               KDATA2_TYPE_TEXT},
	{ZANAMNEZMORBI,               "ZANAMNEZMORBI",               KDATA2_TYPE_TEXT},
	
	{ZDIAGNOZIS,                  "ZDIAGNOZIS",                  KDATA2_TYPE_TEXT},
	{ZINSTRUMENTANDRENTGEN,       "ZINSTRUMENTANDRENTGEN",       KDATA2_TYPE_TEXT},
	{ZLECHENIYE,                  "ZLECHENIYE",                  KDATA2_TYPE_TEXT},
	{ZMESTNO,                     "ZMESTNO",                     KDATA2_TYPE_TEXT},
	
	{ZCASENAME,                   "ZNAME",                       KDATA2_TYPE_TEXT},
	{ZCASEPATIENTID,              "ZPATIENTID",                  KDATA2_TYPE_TEXT},
	
	{ZPOLOZHENIYE,                "ZPOLOZHENIYE",                KDATA2_TYPE_TEXT},
	{ZRECOMENDACII,               "ZRECOMENDACII",               KDATA2_TYPE_TEXT},
	
	{ZSOSTORONIVNUTRENNIHORGANOV, "ZSOSTORONIVNUTRENNIHORGANOV", KDATA2_TYPE_TEXT},
	
	{ZSOSTOYANIYE,                "ZSOSTOYANIYE",                KDATA2_TYPE_TEXT},
	{ZZHALOBI,                    "ZZHALOBI",                    KDATA2_TYPE_TEXT},
	
	{ZBILL,                       "ZBILL",                       KDATA2_TYPE_DATA},
	{ZPLANLECHENIYADATA,          "ZPLANLECHENIYADATA",          KDATA2_TYPE_DATA},

};

#define CASE_COL_NAME(number) cases_columns[number].name
#define CASE_COL_TYPE(number) cases_columns[number].type
#define CASE_COL_TYPE_AND_NAME(number) (CASE_COL_TYPE(number), CASE_COL_NAME(number))

static void	
cases_table_init(struct kdata2_table **cases){
	kdata2_table_init(cases, "ZCASES",
			CASE_COL_TYPE_AND_NAME(ZCASEDATE), 
			CASE_COL_TYPE_AND_NAME(ZCASEDATEOFNEXT), 
			
			CASE_COL_TYPE_AND_NAME(Z11), 
			CASE_COL_TYPE_AND_NAME(Z12), 
			CASE_COL_TYPE_AND_NAME(Z13), 
			CASE_COL_TYPE_AND_NAME(Z14), 
			CASE_COL_TYPE_AND_NAME(Z15), 
			CASE_COL_TYPE_AND_NAME(Z16), 
			CASE_COL_TYPE_AND_NAME(Z17), 
			CASE_COL_TYPE_AND_NAME(Z18), 

			CASE_COL_TYPE_AND_NAME(Z21), 
			CASE_COL_TYPE_AND_NAME(Z22), 
			CASE_COL_TYPE_AND_NAME(Z23), 
			CASE_COL_TYPE_AND_NAME(Z24), 
			CASE_COL_TYPE_AND_NAME(Z25), 
			CASE_COL_TYPE_AND_NAME(Z26), 
			CASE_COL_TYPE_AND_NAME(Z27), 
			CASE_COL_TYPE_AND_NAME(Z28),			

			CASE_COL_TYPE_AND_NAME(Z31), 
			CASE_COL_TYPE_AND_NAME(Z32), 
			CASE_COL_TYPE_AND_NAME(Z33), 
			CASE_COL_TYPE_AND_NAME(Z34), 
			CASE_COL_TYPE_AND_NAME(Z35), 
			CASE_COL_TYPE_AND_NAME(Z36), 
			CASE_COL_TYPE_AND_NAME(Z37), 
			CASE_COL_TYPE_AND_NAME(Z38),			

			CASE_COL_TYPE_AND_NAME(Z41), 
			CASE_COL_TYPE_AND_NAME(Z42), 
			CASE_COL_TYPE_AND_NAME(Z43), 
			CASE_COL_TYPE_AND_NAME(Z44), 
			CASE_COL_TYPE_AND_NAME(Z45), 
			CASE_COL_TYPE_AND_NAME(Z46), 
			CASE_COL_TYPE_AND_NAME(Z47), 
			CASE_COL_TYPE_AND_NAME(Z48),			
			
			CASE_COL_TYPE_AND_NAME(ZALLERGANAMNEZ), 
			CASE_COL_TYPE_AND_NAME(ZANAMNEZMORBI), 
			CASE_COL_TYPE_AND_NAME(ZANAMNEZVITAE), 

			CASE_COL_TYPE_AND_NAME(ZDIAGNOZIS), 

			CASE_COL_TYPE_AND_NAME(ZINSTRUMENTANDRENTGEN), 
			
			CASE_COL_TYPE_AND_NAME(ZLECHENIYE), 
			CASE_COL_TYPE_AND_NAME(ZMESTNO), 
			CASE_COL_TYPE_AND_NAME(ZCASENAME), 
			CASE_COL_TYPE_AND_NAME(ZCASEPATIENTID), 
			CASE_COL_TYPE_AND_NAME(ZPOLOZHENIYE), 
			CASE_COL_TYPE_AND_NAME(ZRECOMENDACII), 
			CASE_COL_TYPE_AND_NAME(ZSOSTORONIVNUTRENNIHORGANOV), 
			CASE_COL_TYPE_AND_NAME(ZSOSTOYANIYE), 
			CASE_COL_TYPE_AND_NAME(ZZHALOBI), 
			CASE_COL_TYPE_AND_NAME(ZBILL), 
			CASE_COL_TYPE_AND_NAME(ZPLANLECHENIYADATA), 

			NULL); 
} 

/* do callback for each case for patientid in database; set patientid to NULL to get all */
static void 
prozubi_cases_foreach(
		kdata2_t *kdata,
		char     *patientid,
		void     *user_data,
		int      *(callback)(void *user_data, struct case_id *c)
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

	char SQL[BUFSIZ] = "SELECT ZRECORDNAME, ZDATE, ZNAME, ZPATIENTID FROM ZCASES";
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
		} else {
			/*! TODO: NSDate
			*  \todo NSDate
			*/
			// do magic to convert NSDate to time_t
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
				goto prozubi_cases_foreach_finish;
	}	
	
prozubi_cases_foreach_finish:;
	sqlite3_finalize(stmt);
}

static struct case_t *
prozubi_case_new(char *id){
	/* allocate case_t */
	struct case_t *c = malloc(sizeof(struct case_t));
	if (!c){
		ERR("%s", "can't allocate struct case_t");
		return NULL;
	}
	strcpy(c->id, id);

	/* init values to NULL */
	c->name                        = NULL;
	c->patientid                   = NULL;
	c->z11                         = NULL;
	c->z12                         = NULL;
	c->z13                         = NULL;
	c->z14                         = NULL;
	c->z15                         = NULL;
	c->z16                         = NULL;
	c->z17                         = NULL;
	c->z18                         = NULL;
	c->z21                         = NULL;
	c->z22                         = NULL;
	c->z23                         = NULL;
	c->z24                         = NULL;
	c->z25                         = NULL;
	c->z26                         = NULL;
	c->z27                         = NULL;
	c->z28                         = NULL;
	c->z31                         = NULL;
	c->z32                         = NULL;
	c->z33                         = NULL;
	c->z34                         = NULL;
	c->z35                         = NULL;
	c->z36                         = NULL;
	c->z37                         = NULL;
	c->z38                         = NULL;
	c->z41                         = NULL;
	c->z42                         = NULL;
	c->z43                         = NULL;
	c->z44                         = NULL;
	c->z45                         = NULL;
	c->z46                         = NULL;
	c->z47                         = NULL;
	c->z48                         = NULL;
	c->alleganamnez                = NULL;
	c->anamnezvitae                = NULL;
	c->anamnezmorbi                = NULL;
	c->diagnozis                   = NULL;
	c->instrumentandrentgen        = NULL;
	c->lecheniye                   = NULL;
	c->mestno                      = NULL;
	c->polozheniye                 = NULL;
	c->recomendacii                = NULL;
	c->sostoyanievnutrennihorganov = NULL;
	c->sostoyanie                  = NULL;
	c->zhalobi                     = NULL;
	c->bill                        = NULL;
	c->planlecheniya               = NULL;
	
	return c;
}

/* get case with id */
static struct case_t * 
prozubi_case_get(
		kdata2_t *kdata,
		char     *id)
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

	int i;
	for (i = 0; i < CASES_COLS_NUM; ++i) {
		strcat(SQL, cases_columns[i].name);	
		if (i != CASES_COLS_NUM - 1)
			strcat(SQL, ", ");
	}

	strcat(SQL, STR(" FROM ZCASES WHERE ZRECORDNAME = '%s'", id));

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
		for (i = 0; i < CASES_COLS_NUM; ++i) {
			/* handle date */
			if (i == ZCASEDATE || i == ZCASEDATEOFNEXT){
				int col_type = sqlite3_column_type(stmt, i);
				if (col_type == SQLITE_INTEGER) {
					if (i == ZCASEDATE)
						c->date = sqlite3_column_int64(stmt, i);
					else
						c->dateofnext = sqlite3_column_int64(stmt, i);					
				} else {
					/*! TODO: NSDate
					*  \todo NSDate
					*/
					// do magic to convert NSDate to time_t
				}		
				continue;
			}

			/* handle text */
			if (cases_columns[i].type == KDATA2_TYPE_TEXT){
				size_t len = sqlite3_column_bytes(stmt, i); 
				const unsigned char *value = sqlite3_column_text(stmt, i);				
				/* allocate string */
				char *str = malloc(len);
				if (!str){
					ERR("can't allocate string with len: %ld", len);
					continue;	
				}
				/* copy value */
				strncpy(str, (const char *)value, len - 1);
				str[len - 1] = 0;
				/* cast string */
				switch (cases_columns[i].number) {
					case Z11: c->z11 = str; break; 
					case Z12: c->z12 = str; break; 
					case Z13: c->z13 = str; break; 
					case Z14: c->z14 = str; break; 
					case Z15: c->z15 = str; break; 
					case Z16: c->z16 = str; break; 
					case Z17: c->z17 = str; break; 
					case Z18: c->z18 = str; break; 

					case Z21: c->z21 = str; break; 
					case Z22: c->z22 = str; break; 
					case Z23: c->z23 = str; break; 
					case Z24: c->z24 = str; break; 
					case Z25: c->z25 = str; break; 
					case Z26: c->z26 = str; break; 
					case Z27: c->z27 = str; break; 
					case Z28: c->z28 = str; break; 

					case Z31: c->z31 = str; break; 
					case Z32: c->z32 = str; break; 
					case Z33: c->z33 = str; break; 
					case Z34: c->z34 = str; break; 
					case Z35: c->z35 = str; break; 
					case Z36: c->z36 = str; break; 
					case Z37: c->z37 = str; break; 
					case Z38: c->z38 = str; break; 

					case Z41: c->z41 = str; break; 
					case Z42: c->z42 = str; break; 
					case Z43: c->z43 = str; break; 
					case Z44: c->z44 = str; break; 
					case Z45: c->z45 = str; break; 
					case Z46: c->z46 = str; break; 
					case Z47: c->z47 = str; break; 
					case Z48: c->z48 = str; break; 
					
					case ZALLERGANAMNEZ: c->alleganamnez                = str; break;
					case ZANAMNEZVITAE: c->anamnezvitae                 = str; break;
					case ZANAMNEZMORBI: c->anamnezmorbi                 = str; break;
					case ZDIAGNOZIS: c->diagnozis                       = str; break;
					case ZINSTRUMENTANDRENTGEN: c->instrumentandrentgen = str; break;
					case ZLECHENIYE: c->lecheniye                       = str; break;
					case ZMESTNO: c->mestno                             = str; break;
					case ZCASENAME: c->name                             = str; break;
					case ZCASEPATIENTID: c->patientid                   = str; break;
					case ZPOLOZHENIYE: c->polozheniye                   = str; break;
					case ZRECOMENDACII: c->recomendacii                 = str; break;
					case ZSOSTOYANIYE: c->sostoyanie                    = str; break;
					case ZZHALOBI: c->zhalobi                           = str; break;
					
					case ZSOSTORONIVNUTRENNIHORGANOV: 
								   c->sostoyanievnutrennihorganov = str; break; 
							  

					default:
						break;
				}
			}
			/* handle data */
			if (cases_columns[i].type == KDATA2_TYPE_DATA){
				size_t len = sqlite3_column_bytes(stmt, i); 
				const void *value = sqlite3_column_blob(stmt, i);
				/* allocate data */
				char *data = malloc(len);
				if (!data){
					ERR("can't allocate data with len: %ld", len);
					continue;	
				}
				/* copy data */
				memcpy(data, value, len - 1);
				/* cast data */
				switch (cases_columns[i].number) {
					case ZBILL: c->bill                       = (cJSON *)data; break;
					case ZPLANLECHENIYADATA: c->planlecheniya = (cJSON *)data; break;
					
					default:
						break;
				}
			}			
		}		
	}	
	
	sqlite3_finalize(stmt);
	return c;
}

static void
prozubi_case_free(struct case_t *c){
	if (c){
		if (c->name) free(c->name);		
		if (c->patientid) free(c->patientid);   
	
		if (c->z11) free(c->z11);
		if (c->z12) free(c->z12);
		if (c->z13) free(c->z13);
		if (c->z14) free(c->z14);
		if (c->z15) free(c->z15);
		if (c->z16) free(c->z16);
		if (c->z17) free(c->z17);
		if (c->z18) free(c->z18);

		if (c->z21) free(c->z21);
		if (c->z22) free(c->z22);
		if (c->z23) free(c->z23);
		if (c->z24) free(c->z24);
		if (c->z25) free(c->z25);
		if (c->z26) free(c->z26);
		if (c->z27) free(c->z27);
		if (c->z28) free(c->z28);

		if (c->z31) free(c->z31);
		if (c->z32) free(c->z32);
		if (c->z33) free(c->z33);
		if (c->z34) free(c->z34);
		if (c->z35) free(c->z35);
		if (c->z36) free(c->z36);
		if (c->z37) free(c->z37);
		if (c->z38) free(c->z38);

		if (c->z41) free(c->z41);
		if (c->z42) free(c->z42);
		if (c->z43) free(c->z43);
		if (c->z44) free(c->z44);
		if (c->z45) free(c->z45);
		if (c->z46) free(c->z46);
		if (c->z47) free(c->z47);
		if (c->z48) free(c->z48);
		
		if (c->alleganamnez) free(c->alleganamnez); 
		if (c->anamnezvitae) free(c->anamnezvitae); 
		if (c->anamnezmorbi) free(c->anamnezmorbi); 
	
		if (c->diagnozis) free(c->diagnozis);    
	
		if (c->instrumentandrentgen) free(c->instrumentandrentgen);
	
		if (c->lecheniye) free(c->lecheniye);   
	
		if (c->mestno) free(c->mestno);     
	
	
		if (c->polozheniye) free(c->polozheniye); 
		if (c->recomendacii) free(c->recomendacii); 
		if (c->sostoyanievnutrennihorganov) free(c->sostoyanievnutrennihorganov); 
		if (c->sostoyanie) free(c->sostoyanie); 
		if (c->zhalobi) free(c->zhalobi); 

		if (c->bill) cJSON_free(c->bill); 
		if (c->planlecheniya) cJSON_free(c->planlecheniya); 
		
		free(c);
	}
}


#endif /* ifndef CASES_H */
