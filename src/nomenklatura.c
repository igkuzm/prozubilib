/**
 * File              : nomenklatura.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.05.2023
 * Last Modified Date: 13.09.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/nomenklatura.h"

static nomenklatura_t *
_nomenklatura_new(prozubi_t *p)
{
	nomenklatura_t *c = NEW(nomenklatura_t);
	if (c == NULL){
			if (p->on_error)
				p->on_error(p->on_error_data,		
				STR("%s", "cant allocate memory"));
			return NULL;
	}
	
	#define NOMENKLATURA_COLUMN_INT(title) c->title = -1;
	#define NOMENKLATURA_COLUMN_TEX(title) c->title = NULL;
	NOMENKLATURA_COLUMNS
	#undef NOMENKLATURA_COLUMN_INT
	#undef NOMENKLATURA_COLUMN_TEX
	
	return c;
}

 void
prozubi_nomenklatura_foreach(
		prozubi_t *p,
		const char *predicate,
		void * user_data,
		void * (*callback)(
			void * user_data,
			void * parent,
			nomenklatura_t *ptr
			)
		)
{
	sqlite3 *db;
	sqlite3_stmt *stmt_p;
	char SQL[2*BUFSIZ] = "SELECT ";
	
	if (!p)
		return;

	if (!callback){
		if (p->on_error)
			p->on_error(p->on_error_data,		
		STR_ERR("%s", "callback is NULL"));
		return;
	}

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
	if (predicate){
		#define NOMENKLATURA_COLUMN_INT(title) strcat(SQL, #title); strcat(SQL, ", "); 
		#define NOMENKLATURA_COLUMN_TEX(title) strcat(SQL, #title); strcat(SQL, ", "); 
		NOMENKLATURA_COLUMNS
		#undef NOMENKLATURA_COLUMN_INT
		#undef NOMENKLATURA_COLUMN_TEX
		strcat(SQL, "1 FROM ");
		strcat(SQL, NOMENKLATURA_TABL);
		strcat(SQL, " ");
		strcat(SQL, predicate);
	}else{
		strcat(SQL, "headName FROM ");
		strcat(SQL, NOMENKLATURA_TABL);
		strcat(SQL, " WHERE parent = 0 ");
	}

	/* start SQLite request for parent*/
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

		if (predicate)	{
			/* iterate columns */
				int i;
				for (i = 0; i < NOMENKLATURA_COLS_NUM; ++i) {
					/* handle values */
					switch (i) {

#define NOMENKLATURA_COLUMN_TEX(title) \
						case NOMENKLATURA_##title:\
						{\
							size_t len = sqlite3_column_bytes(stmt_p, i);\
							const unsigned char *value = sqlite3_column_text(stmt_p, i);\
							c->title = strdup((const char *)value);\
							break;\
						};				
#define NOMENKLATURA_COLUMN_INT(title) \
						case NOMENKLATURA_##title:\
						{\
							int value = sqlite3_column_int(stmt_p, i);\
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
				callback(user_data, NULL, c);

		} else {
			/* handle values */
			char SQL_c[2*BUFSIZ] = "SELECT ";
			sqlite3_stmt *stmt_c;
			void *parent;
			size_t len = sqlite3_column_bytes(stmt_p, 0);
			const unsigned char *value = sqlite3_column_text(stmt_p, 0);
			c->name = strdup((const char *)value);
			c->kod  = strdup("");
			c->headName  = strdup("");

			/* callback */
			parent = callback(user_data, NULL, c);

			/* get children */
			
			/* create SQL string */
			
			#define NOMENKLATURA_COLUMN_INT(title) strcat(SQL_c, #title); strcat(SQL_c, ", "); 
			#define NOMENKLATURA_COLUMN_TEX(title) strcat(SQL_c, #title); strcat(SQL_c, ", "); 
			NOMENKLATURA_COLUMNS
			#undef NOMENKLATURA_COLUMN_INT
			#undef NOMENKLATURA_COLUMN_TEX
			
			strcat(SQL_c, "1 FROM ");
			strcat(SQL_c, NOMENKLATURA_TABL);
			strcat(SQL_c, " WHERE headName = '");
			strcat(SQL_c, c->name);
			strcat(SQL_c, "'");

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
				int i;
				nomenklatura_t *c = _nomenklatura_new(p);
				if (!c)
					break;
				
				/* iterate columns */
				for (i = 0; i < NOMENKLATURA_COLS_NUM; ++i) {
					/* handle values */
					switch (i) {

#define NOMENKLATURA_COLUMN_TEX(title) \
						case NOMENKLATURA_##title:\
						{\
							size_t len = sqlite3_column_bytes(stmt_c, i);\
							const unsigned char *value = sqlite3_column_text(stmt_c, i);\
							c->title = strdup((char*)value);\
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
	}
	sqlite3_finalize(stmt_p);
	sqlite3_close(db);
}

 void
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

