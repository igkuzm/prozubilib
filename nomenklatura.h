/**
 * File              : nomenklatura.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.05.2023
 * Last Modified Date: 25.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef NOMENKLATURA_H
#define NOMENKLATURA_H

#include "kdata2/kdata2.h"
#include "enum.h"
#include "log.h"
#include "kdata2/sqlite3.h"
#include <stdio.h>


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
	#define NOMENKLATURA_COLUMN_TEX(title) const char * title;
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

static void
prozubi_nomenklatura_foreach(
		kdata2_t *kdata,
		void * user_data,
		void * (*callback)(
			void * user_data,
			void * parent,
			nomenklatura_t *p
			)
		)
{
	if (!kdata)
		return;

	if (!callback){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
		STR_ERR("%s", "callback is NULL"));
		return;
	}

	sqlite3 *db;
	
	if (sqlite3_open_v2(NOMENKLATURA_BASE, &db, 
				SQLITE_OPEN_READONLY, NULL))
	{
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("SQLite: Failed to create '%s': %s", 
				NOMENKLATURA_BASE, sqlite3_errmsg(db)));
		return;	
	}
	
	/* create SQL string */
	char SQL_p[BUFSIZ] = "";
	char SQL[BUFSIZ] = "SELECT ";

	#define NOMENKLATURA_COLUMN_INT(title) strcat(SQL, #title); strcat(SQL, ", "); 
	#define NOMENKLATURA_COLUMN_TEX(title) strcat(SQL, #title); strcat(SQL, ", "); 
	NOMENKLATURA_COLUMNS
	#undef NOMENKLATURA_COLUMN_INT
	#undef NOMENKLATURA_COLUMN_TEX
	
	strcat(SQL, "FROM ");
	strcat(SQL, NOMENKLATURA_TABL);

	strcat(SQL_p, SQL);
	strcat(SQL_p, " WHERE parent = 0");

	/* start SQLite request for parent*/
	sqlite3_stmt *stmt_p;
	if(sqlite3_prepare_v2(db, SQL, -1, &stmt_p, NULL)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,				
			STR_ERR("sqlite3_prepare_v2: %s: %s", SQL_p, sqlite3_errmsg(db)));	
		sqlite3_close(db);
		return;
	};

	while (sqlite3_step(stmt_p) != SQLITE_DONE) {
		nomenklatura_t p;
	
		/* iterate columns */
		int i;
		for (i = 0; i < NOMENKLATURA_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define NOMENKLATURA_COLUMN_TEX(title) \
				case NOMENKLATURA_##title:\
				{\
					const unsigned char *value = sqlite3_column_text(stmt_p, i);\
					p.title = (const char *)value;\
					break;\
				};				
#define NOMENKLATURA_COLUMN_INT(title) \
				case NOMENKLATURA_##title:\
				{\
					int value = sqlite3_column_int(stmt_p, i);\
					p.title = value;\
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
		void *parent = callback(user_data, NULL, &p);

		/* get children */
		sqlite3_stmt *stmt_c;
		
		/* create SQL string */
		char SQL_c[BUFSIZ] = "";
		sprintf(SQL_c, "%s WHERE headName = '%s'", SQL, p.name);

		/* start SQLite request */
		if(sqlite3_prepare_v2(db, SQL_c, -1, &stmt_c, NULL)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
				STR_ERR("sqlite3_prepare_v2: %s: %s", SQL_c, sqlite3_errmsg(db)));	
			sqlite3_close(db);
			return;
		};

		while (sqlite3_step(stmt_c) != SQLITE_DONE) {
			nomenklatura_t c;
		
			/* iterate columns */
			int i;
			for (i = 0; i < NOMENKLATURA_COLS_NUM; ++i) {
				/* handle values */
				switch (i) {

#define NOMENKLATURA_COLUMN_TEX(title) \
					case NOMENKLATURA_##title:\
					{\
						const unsigned char *value = sqlite3_column_text(stmt_c, i);\
						c.title = (const char *)value;\
						break;\
					};				
#define NOMENKLATURA_COLUMN_INT(title) \
					case NOMENKLATURA_##title:\
					{\
						int value = sqlite3_column_int(stmt_c, i);\
						c.title = value;\
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
			callback(user_data, parent, &c);
		}	
		sqlite3_finalize(stmt_c);
	}
	sqlite3_finalize(stmt_p);
	sqlite3_close(db);
}

#endif // NOMENKLATURA_H	
