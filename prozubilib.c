/**
 * File              : prozubilib.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 17.04.2023
 * Last Modified Date: 21.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include <stddef.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "kdata_conf.h"

#include "log.h"

#include "cases.h"

typedef char uuid4_t[37];

kdata2_t *
prozubilib_init(
		const char *filepath,
		const char *token
		)
{
	/* check filepath */
	if (!filepath) {
		ERR("%s", "filepath is NULL");
		return NULL;
	}

	/* check token */
	if (!filepath)
		LOG("%s", "token is NULL");

	/* init tables */
	struct kdata2_table *zcases;	
	cases_table_init(&zcases); 

	struct kdata2_table *zdoctors;	
	kdata2_table_init(&zdoctors, "ZDOCTORS", 
			
			KDATA2_TYPE_TEXT,   "ZEVENTCALENDAR", 
			KDATA2_TYPE_TEXT,   "ZFIO", 
			KDATA2_TYPE_TEXT,   "ZICLOUDID", 
			KDATA2_TYPE_TEXT,   "ZTEL", 

			KDATA2_TYPE_DATA,   "ZZAPIS",			

			NULL);	

	struct kdata2_table *zimages;	
	kdata2_table_init(&zimages, "ZIMAGES", 
			
			KDATA2_TYPE_TEXT,   "ZLASTMODIFIED", 
			
			KDATA2_TYPE_TEXT,   "ZCASEID", 
			KDATA2_TYPE_TEXT,   "ZTITLE", 

			KDATA2_TYPE_DATA,   "ZIMAGEDATA",			

			NULL);	

	struct kdata2_table *zpassport;	
	kdata2_table_init(&zpassport, "ZPASSPORT", 
			
			KDATA2_TYPE_TEXT,   "ZDATEOFBIRTH", 
			
			KDATA2_TYPE_TEXT,   "ZADDRESS", 
			KDATA2_TYPE_TEXT,   "ZCOMMENT", 
			KDATA2_TYPE_TEXT,   "ZDOCUMENT", 
			KDATA2_TYPE_TEXT,   "ZEMAIL", 
			KDATA2_TYPE_TEXT,   "ZFAMILIYA", 
			KDATA2_TYPE_TEXT,   "ZIMIA", 
			KDATA2_TYPE_TEXT,   "ZOTCHESTVO", 
			KDATA2_TYPE_TEXT,   "ZTEL", 

			NULL);	

	struct kdata2_table *zprices;	
	kdata2_table_init(&zprices, "ZPRICES", 
			
			KDATA2_TYPE_TEXT,   "ZABOUT", 
			KDATA2_TYPE_TEXT,   "ZCATEGORY", 
			KDATA2_TYPE_TEXT,   "ZKOD", 
			KDATA2_TYPE_TEXT,   "ZPRICE", 
			KDATA2_TYPE_TEXT,   "ZTITLE", 

			NULL);	

	struct kdata2_table *ztemplates;	
	kdata2_table_init(&ztemplates, "ZTEMPLATES", 
			
			KDATA2_TYPE_TEXT,   "ZTEMPLATENAME", 
			KDATA2_TYPE_TEXT,   "ZTEXT", 
			KDATA2_TYPE_TEXT,   "ZTITLE", 

			NULL);	

	/* init kdata */ 
	kdata2_t *kdata;
	kdata2_init(&kdata, filepath, token, 60,
			zcases, zdoctors, zimages, zpassport, zprices, ztemplates,
			NULL);

	return kdata;
}

/* ptient struct */
struct patient {
	time_t dateofbirth;
	char address[256];
	char comment[256];
	char document[512];
	char email[128];
	char familiya[128];
	char imia[128];
	char otchestvo[128];
	uuid4_t patientid;
	uint64_t tel;
};

static
void prozubilib_get_patients(
		void *user_data,
		int (*callback)(struct patient * patient, void *userdata, char *error)
		)
{
	char filename[] = "/Users/kuzmich/Library/Containers/kuzm.ig.ProZubi/Data/Documents/ProZubi.sqlite";
	sqlite3 *db;
	sqlite3_open_v2(filename, &db, SQLITE_OPEN_READONLY, NULL);
	/*! TODO: make SQLite3 function to fill and callback patients
	*  \todo make SQLite3 function to fill and callback patients
	*/

	/* start SQLite request */
	int res;
	char *errmsg = NULL;
	sqlite3_stmt *stmt;
	
	char *SQL = STR("SELECT ZDATEOFBIRTH, ZADDRESS, ZCOMMENT, ZDOCUMENT, "
					"ZEMAIL, ZFAMILIYA, ZIMIA, ZOTCHESTVO, ZPATIENTID, ZTEL FROM 'ZPASSPORT'");
	res = sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		fprintf(stderr, "sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(db));	
		return;
	}	

	int num_cols = sqlite3_column_count(stmt); //number of colums
	
	while (sqlite3_step(stmt) != SQLITE_DONE) {
		/* allocate new patient */
		struct patient *patient = malloc(sizeof(struct patient));

		/* iterate columns */
		int i;
		for (i = 0; i < num_cols; ++i) {

			switch (i) {
				// ZDATEOFBIRTH
				case 0: {
					const char * value = (const char *)sqlite3_column_text(stmt, i);
					if (!value) 
						value = "";
					patient->dateofbirth = atol(value);
					break;
				}						
				
				// ZADDRESS
				case 1: {
					strncpy(patient->address, 
							(const char *)sqlite3_column_text(stmt, i), 
									sizeof(patient->address) - 1);
					patient->address[sizeof(patient->address) - 1] = 0;
					break;
				}

				// ZCOMMENT
				case 2: {
					strncpy(patient->comment, 
							(const char *)sqlite3_column_text(stmt, i), 
									sizeof(patient->comment) - 1);
					patient->comment[sizeof(patient->comment) - 1] = 0;
					break;
				}

				// ZDOCUMENT
				case 3: {
					strncpy(patient->document, 
							(const char *)sqlite3_column_text(stmt, i), 
									sizeof(patient->document) - 1);
					patient->document[sizeof(patient->document) - 1] = 0;
					break;
				}
				
				// ZEMAIL
				case 4: {
					strncpy(patient->email, 
							(const char *)sqlite3_column_text(stmt, i), 
									sizeof(patient->email) - 1);
					patient->email[sizeof(patient->email) - 1] = 0;
					break;
				}						

				// ZFAMILIYA
				case 5: {
					strncpy(patient->familiya, 
							(const char *)sqlite3_column_text(stmt, i), 
									sizeof(patient->familiya) - 1);
					patient->familiya[sizeof(patient->familiya) - 1] = 0;
					break;
				}						
						
				// ZIMIA
				case 6: {
					strncpy(patient->imia, 
							(const char *)sqlite3_column_text(stmt, i), 
									sizeof(patient->imia) - 1);
					patient->imia[sizeof(patient->imia) - 1] = 0;
					break;
				}						
						
				// ZOTCHESTVO
				case 7: {
					strncpy(patient->otchestvo, 
							(const char *)sqlite3_column_text(stmt, i), 
									sizeof(patient->otchestvo) - 1);
					patient->otchestvo[sizeof(patient->otchestvo) - 1] = 0;
					break;
				}						
						
				// ZPATIENTID
				case 8: {
					strncpy(patient->patientid, 
							(const char *)sqlite3_column_text(stmt, i), 
									sizeof(patient->patientid) - 1);
					patient->patientid[sizeof(patient->patientid) - 1] = 0;
					break;
				}						
												
				// ZTEL
				case 9: patient->tel = sqlite3_column_int64(stmt, i); break;
			}
		}
		callback(patient, user_data, NULL);
	}	
	sqlite3_finalize(stmt);
 
	sqlite3_close(db);	
}

/* ptient struct */
struct doctor {
	uuid4_t doctorid;
	char calendar[256];
	char fio[128];
	uint64_t tel;
};

static
void prozubilib_get_doctors(
		void *user_data,
		int (*callback)(struct doctor * doctor, void *userdata, char *error)
		)
{
	char filename[] = "/Users/kuzmich/Library/Containers/kuzm.ig.ProZubi/Data/Documents/ProZubi.sqlite";
	sqlite3 *db;
	sqlite3_open_v2(filename, &db, SQLITE_OPEN_READONLY, NULL);
	/*! TODO: make SQLite3 function to fill and callback patients
	*  \todo make SQLite3 function to fill and callback patients
	*/

	/* start SQLite request */
	int res;
	char *errmsg = NULL;
	sqlite3_stmt *stmt;
	
	char *SQL = STR("SELECT ZDOCTORID, ZEVENTCALENDAR, ZFIO, ZTEL FROM 'ZDOCTORS'");
	res = sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		fprintf(stderr, "sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(db));	
		return;
	}	

	int num_cols = sqlite3_column_count(stmt); //number of colums
	
	while (sqlite3_step(stmt) != SQLITE_DONE) {
		/* allocate new patient */
		struct doctor *doctor = malloc(sizeof(struct doctor));

		/* iterate columns */
		int i;
		for (i = 0; i < num_cols; ++i) {

			switch (i) {
				// ZDOCTORID
				case 0: {
					const char * value = (const char *)sqlite3_column_text(stmt, i);
					if (!value) 
						value = "";
					strncpy(doctor->doctorid, value, sizeof(doctor->doctorid) - 1);
					doctor->doctorid[sizeof(doctor->doctorid) - 1] = 0;
					break;
				}

				// ZEVENTCALENDAR
				case 1: {
					const char * value = (const char *)sqlite3_column_text(stmt, i);
					if (!value) 
						value = "";							
					strncpy(doctor->calendar, value, sizeof(doctor->calendar) - 1);
					doctor->calendar[sizeof(doctor->calendar) - 1] = 0;
					break;
				}

				// ZFIO
				case 2: {
					const char * value = (const char *)sqlite3_column_text(stmt, i);
					if (!value) 
						value = "";							
							
					strncpy(doctor->fio, value, sizeof(doctor->fio) - 1);
					doctor->fio[sizeof(doctor->fio) - 1] = 0;
					break;
				}

				// ZTEL
				case 3: doctor->tel = sqlite3_column_int64(stmt, i); break;
				
				
				default: break;
			}
		}
		callback(doctor, user_data, NULL);
	}	
	sqlite3_finalize(stmt);
 
	sqlite3_close(db);	
}
