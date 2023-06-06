/**
 * File              : nomenklatura.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.05.2023
 * Last Modified Date: 06.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef NOMENKLATURA_H
#define NOMENKLATURA_H

#include "alloc.h"
#include "prozubilib_conf.h"
#include "enum.h"
#include "log.h"
#include "kdata2/sqlite3.h"
#include <stdio.h>
#include <stdlib.h>


#define NOMENKLATURA_BASE "nomenklatura.sqlite"
#define NOMENKLATURA_TABL "nomenklatura"

#define NOMENKLATURA_COLUMNS \
	NOMENKLATURA_COLUMN_INT(iD) \
	NOMENKLATURA_COLUMN_INT(parent) \
	NOMENKLATURA_COLUMN_TEX(kod) \
	NOMENKLATURA_COLUMN_TEX(name) \
	NOMENKLATURA_COLUMN_TEX(headName)

typedef struct NOMENKLATURA_t {
	#define NOMENKLATURA_COLUMN_INT(title) int    title;
	#define NOMENKLATURA_COLUMN_TEX(title) char * title;
	NOMENKLATURA_COLUMNS
	#undef NOMENKLATURA_COLUMN_INT
	#undef NOMENKLATURA_COLUMN_TEX
} nomenklatura_t;

BEGIN_ENUM(NOMENKLATURA)
	#define NOMENKLATURA_COLUMN_INT(title) NOMENKLATURA_##title,
	#define NOMENKLATURA_COLUMN_TEX(title) NOMENKLATURA_##title,
	NOMENKLATURA_COLUMNS
	#undef NOMENKLATURA_COLUMN_INT
	#undef NOMENKLATURA_COLUMN_TEX
	NOMENKLATURA_COLS_NUM
END_ENUM(NOMENKLATURA)

BEGIN_ENUM_STRING(NOMENKLATURA)
	#define NOMENKLATURA_COLUMN_INT(title) "NOMENKLATURA_" #title,
	#define NOMENKLATURA_COLUMN_TEX(title) "NOMENKLATURA_" #title,
	NOMENKLATURA_COLUMNS
	#undef NOMENKLATURA_COLUMN_INT
	#undef NOMENKLATURA_COLUMN_TEX
END_ENUM_STRING(NOMENKLATURA)

static nomenklatura_t *
_nomenklatura_new(prozubi_t *p)
{
	nomenklatura_t *c = NEW(nomenklatura_t,
			if (p->on_error)
				p->on_error(p->on_error_data,		
				STR_ERR("%s", "cant allocate memory")),
			return NULL);
	
	#define NOMENKLATURA_COLUMN_INT(title) c->title = -1;
	#define NOMENKLATURA_COLUMN_TEX(title) c->title = NULL;
	NOMENKLATURA_COLUMNS
	#undef NOMENKLATURA_COLUMN_INT
	#undef NOMENKLATURA_COLUMN_TEX
	
	return c;
}

static void
prozubi_nomenklatura_foreach(
		prozubi_t *p,
		void * user_data,
		void * (*callback)(
			void * user_data,
			void * parent,
			nomenklatura_t *ptr
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
	
	if (sqlite3_open_v2(NOMENKLATURA_BASE, &db, 
				SQLITE_OPEN_READONLY, NULL))
	{
		if (p->on_error)
			p->on_error(p->on_error_data,		
			STR_ERR("SQLite: Failed to create '%s': %s", 
				NOMENKLATURA_BASE, sqlite3_errmsg(db)));
		return;	
	}
	
	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT headName FROM ";
	strcat(SQL, NOMENKLATURA_TABL);
	strcat(SQL, " WHERE parent = 0");

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
		nomenklatura_t *c = _nomenklatura_new(p);
		if (!c)
			break;
	
		/* handle values */
		size_t len = sqlite3_column_bytes(stmt_p, 0);
		const unsigned char *value = sqlite3_column_text(stmt_p, 0);
		c->name = MALLOC(len + 1, 
				if (p->on_error)
					p->on_error(p->on_error_data,		
					STR_ERR("cant allocate memory len: %ld", len + 1)),
				break);
		strncpy(c->name, (const char *)value, len);
		c->name[len] = 0;
		c->kod  = strdup("");
		c->headName  = strdup("");

		/* callback */
		void *parent = callback(user_data, NULL, c);

		/* get children */
		sqlite3_stmt *stmt_c;
		
		/* create SQL string */
		char SQL_c[BUFSIZ] = "SELECT ";
		
		#define NOMENKLATURA_COLUMN_INT(title) strcat(SQL_c, #title); strcat(SQL_c, ", "); 
		#define NOMENKLATURA_COLUMN_TEX(title) strcat(SQL_c, #title); strcat(SQL_c, ", "); 
		NOMENKLATURA_COLUMNS
		#undef NOMENKLATURA_COLUMN_INT
		#undef NOMENKLATURA_COLUMN_TEX
		
		strcat(SQL_c, "1 FROM ");
		strcat(SQL_c, NOMENKLATURA_TABL);
		
		sprintf(SQL_c, "%s WHERE headName = '%s'", SQL_c, c->name);

		/* start SQLite request */
		if(sqlite3_prepare_v2(db, SQL_c, -1, &stmt_c, NULL)){
			if (p->on_error)
				p->on_error(p->on_error_data,			
				STR_ERR("sqlite3_prepare_v2: %s: %s", SQL_c, sqlite3_errmsg(db)));	
			sqlite3_close(db);
			return;
		};

		while (sqlite3_step(stmt_c) != SQLITE_DONE) {
			nomenklatura_t *c = _nomenklatura_new(p);
			if (!c)
				break;
			
			/* iterate columns */
			int i;
			for (i = 0; i < NOMENKLATURA_COLS_NUM; ++i) {
				/* handle values */
				switch (i) {

#define NOMENKLATURA_COLUMN_TEX(title) \
					case NOMENKLATURA_##title:\
					{\
						size_t len = sqlite3_column_bytes(stmt_c, i);\
						const unsigned char *value = sqlite3_column_text(stmt_c, i);\
						c->title = MALLOC(len + 1,\
								if (p->on_error)\
									p->on_error(p->on_error_data,\
									STR_ERR("cant allocate memory len: %ld", len + 1)),\
								break);\
						strncpy(c->title, (const char *)value, len);\
						c->title[len] = 0;\
						break;\
					};				
#define NOMENKLATURA_COLUMN_INT(title) \
					case NOMENKLATURA_##title:\
					{\
						int value = sqlite3_column_int(stmt_c, i);\
						c->title = value;\
						break;\
					};				

				NOMENKLATURA_COLUMNS

#undef NOMENKLATURA_COLUMN_TEX			
#undef NOMENKLATURA_COLUMN_INT			

					default:
						break;					
				}
			}		
			/* callback */
			callback(user_data, parent, c);
		}	
		sqlite3_finalize(stmt_c);
	}
	sqlite3_finalize(stmt_p);
	sqlite3_close(db);
}

static void
prozubi_nomenklatura_free(nomenklatura_t *c){
	if (c){

#define NOMENKLATURA_COLUMN_INT(title) 
#define NOMENKLATURA_COLUMN_TEX(title) if(c->title) free(c->title); 
		NOMENKLATURA_COLUMNS

#undef NOMENKLATURA_COLUMN_TEX			
#undef NOMENKLATURA_COLUMN_INT			

		free(c);
		c = NULL;
	}
}

#endif // NOMENKLATURA_H	
