/**
 * File              : mkb.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.05.2023
 * Last Modified Date: 13.09.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PROZUBILIB_MKB_H
#define PROZUBILIB_MKB_H

#include "alloc.h"
#include "prozubilib_conf.h"
#include "enum.h"
#include "kdata2/cYandexDisk/log.h"
#include "kdata2/sqlite3.h"
#include <stdio.h>
#include <stdlib.h>


#define MKB_BASE "MKBCodes.sqlite"
#define MKB_TABL "kodiMKB"

#define MKB_COLUMNS \
	MKB_COLUMN_INT(iD) \
	MKB_COLUMN_INT(parent) \
	MKB_COLUMN_TEX(kod) \
	MKB_COLUMN_TEX(name) \

typedef struct MKB_t {
	#define MKB_COLUMN_INT(title) int    title;
	#define MKB_COLUMN_TEX(title) char * title;
	MKB_COLUMNS
	#undef MKB_COLUMN_INT
	#undef MKB_COLUMN_TEX
} mkb_t;

BEGIN_ENUM(MKB)
	#define MKB_COLUMN_INT(title) MKB_##title,
	#define MKB_COLUMN_TEX(title) MKB_##title,
	MKB_COLUMNS
	#undef MKB_COLUMN_INT
	#undef MKB_COLUMN_TEX
	MKB_COLS_NUM
END_ENUM(MKB)

BEGIN_ENUM_STRING(MKB)
	#define MKB_COLUMN_INT(title) "MKB_" #title,
	#define MKB_COLUMN_TEX(title) "MKB_" #title,
	MKB_COLUMNS
	#undef MKB_COLUMN_INT
	#undef MKB_COLUMN_TEX
END_ENUM_STRING(MKB)

static mkb_t *
_mkb_new(prozubi_t *p)
{
	mkb_t *c = NEW(mkb_t,
			if (p->on_error)
				p->on_error(p->on_error_data,		
				STR("%s", "cant allocate memory"));
			return NULL);
	
	#define MKB_COLUMN_INT(title) c->title = -1;
	#define MKB_COLUMN_TEX(title) c->title = NULL;
	MKB_COLUMNS
	#undef MKB_COLUMN_INT
	#undef MKB_COLUMN_TEX
	
	return c;
}

static void
_prozubi_mkb_get_child(
		prozubi_t *p,
		sqlite3 *db,	
		int parent_id,
		void *user_data,
		void *parent,
		void * (*callback)(
			void * user_data,
			void * parent,
			mkb_t *ptr
			)
		)
{
	/* get children */
	sqlite3_stmt *stmt_c;
	
	/* create SQL string */
	char SQL_c[BUFSIZ] = "SELECT ";
	
	#define MKB_COLUMN_INT(title) strcat(SQL_c, #title); strcat(SQL_c, ", "); 
	#define MKB_COLUMN_TEX(title) strcat(SQL_c, #title); strcat(SQL_c, ", "); 
	MKB_COLUMNS
	#undef MKB_COLUMN_INT
	#undef MKB_COLUMN_TEX

	sprintf(SQL_c, "%s 1 FROM %s WHERE parent = %d", SQL_c, MKB_TABL, parent_id);	

	if (p->on_log)
		p->on_log(p->on_log_data, STR("SQL: %s\n", SQL_c));		
	
	/* start SQLite request */
	if(sqlite3_prepare_v2(db, SQL_c, -1, &stmt_c, NULL)){
		if (p->on_error)
			p->on_error(p->on_error_data,			
			STR_ERR("sqlite3_prepare_v2: %s: %s", SQL_c, sqlite3_errmsg(db)));	
		sqlite3_close(db);
		return;
	};

	while (sqlite3_step(stmt_c) != SQLITE_DONE) {
		mkb_t *c = _mkb_new(p);
		if (!c)
			break;
		
		/* iterate columns */
		int i;
		for (i = 0; i < MKB_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define MKB_COLUMN_TEX(title) \
				case MKB_##title:\
				{\
					size_t len = sqlite3_column_bytes(stmt_c, i);\
					const unsigned char *value = sqlite3_column_text(stmt_c, i);\
					c->title = strdup((char*)value);\
					c->title[len] = 0;\
					break;\
				};				
#define MKB_COLUMN_INT(title) \
				case MKB_##title:\
				{\
					int value = sqlite3_column_int(stmt_c, i);\
					c->title = value;\
					break;\
				};				

			MKB_COLUMNS

#undef MKB_COLUMN_TEX			
#undef MKB_COLUMN_INT			

				default:
					break;					
			}
		}		
		/* callback */
		parent = callback(user_data, parent, c);

		// loop
		_prozubi_mkb_get_child(p, db, c->iD, user_data, parent, callback);
	}	
	sqlite3_finalize(stmt_c);
}

static void
prozubi_mkb_foreach(
		prozubi_t *p,
		const char *predicate,
		void * user_data,
		void * (*callback)(
			void * user_data,
			void * parent,
			mkb_t *ptr
			)
		)
{
	if (!p)
		return;

	if (!callback){
		if (p->on_error)
			p->on_error(p->on_error_data,		
		STR_ERR("%s", "callback is NULL"));
		return;
	}

	sqlite3 *db;
	
	if (sqlite3_open_v2(MKB_BASE, &db, 
				SQLITE_OPEN_READONLY, NULL))
	{
		if (p->on_error)
			p->on_error(p->on_error_data,		
			STR_ERR("SQLite: Failed to create '%s': %s", 
				MKB_BASE, sqlite3_errmsg(db)));
		return;	
	}
	
	/* create SQL string */
	char *SQL = (char *)malloc(BUFSIZ);
	if (SQL == NULL)
		return;
	strcpy(SQL, "");
	strcat(SQL, "SELECT ");
#define MKB_COLUMN_INT(title) strcat(SQL, #title); strcat(SQL, ", "); 
#define MKB_COLUMN_TEX(title) strcat(SQL, #title); strcat(SQL, ", "); 
	MKB_COLUMNS
#undef MKB_COLUMN_INT
#undef MKB_COLUMN_TEX
	strcat(SQL, "1 FROM ");
	strcat(SQL, MKB_TABL);
	if (predicate){
		strcat(SQL, " ");
		strcat(SQL, predicate);
	}else{
		strcat(SQL, " WHERE parent = 0 ");
	}

	/* start SQLite request for parent*/
	sqlite3_stmt *stmt_p;
	if(sqlite3_prepare_v2(db, SQL, -1, &stmt_p, NULL)){
		if (p->on_error)
			p->on_error(p->on_error_data,				
			STR_ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(db)));	
		sqlite3_close(db);
		return;
	};
	while (sqlite3_step(stmt_p) != SQLITE_DONE) {
		mkb_t *c = _mkb_new(p);
		if (!c)
			break;

		/* iterate columns */
		int i;
		for (i = 0; i < MKB_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define MKB_COLUMN_TEX(title) \
				case MKB_##title:\
				{\
					size_t len = sqlite3_column_bytes(stmt_p, i);\
					const unsigned char *value = sqlite3_column_text(stmt_p, i);\
					c->title = strdup((const char *)value);\
					break;\
				};				
#define MKB_COLUMN_INT(title) \
				case MKB_##title:\
				{\
					int value = sqlite3_column_int(stmt_p, i);\
					c->title = value;\
					break;\
				};				

			MKB_COLUMNS

#undef MKB_COLUMN_TEX			
#undef MKB_COLUMN_INT			

				default:
					break;					
			}
		}

		/* callback */
		void *parent = callback(user_data, NULL, c);

		// loop
		_prozubi_mkb_get_child(p, db, c->iD, user_data, parent, callback);
	}
	
	free(SQL);
	sqlite3_finalize(stmt_p);
	sqlite3_close(db);
}

static void
prozubi_mkb_free(mkb_t *c){
	if (c){

#define MKB_COLUMN_INT(title) 
#define MKB_COLUMN_TEX(title) if(c->title) free(c->title); 
		MKB_COLUMNS

#undef MKB_COLUMN_TEX			
#undef MKB_COLUMN_INT			

		free(c);
		c = NULL;
	}
}

#endif // PROZUBILIB_MKB_H	
