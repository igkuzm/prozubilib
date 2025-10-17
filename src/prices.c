/**
 * File              : prices.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 24.08.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/prices.h"

 void	
prozubi_prices_table_init(struct kdata2_table **prices){
	kdata2_table_init(prices, PRICES_TABLENAME,

#define PRICES_COLUMN_TEXT(member, number, title) KDATA2_TYPE_TEXT, title, 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
			
			NULL); 
} 

/* allocate and init new price */
 struct price_t *
prozubi_price_new(
		kdata2_t *kdata,
#define PRICES_COLUMN_TEXT(member, number, title) const char * member, 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
		const char *id
		)
{
	struct price_t *p;
	
	if (!kdata)
		return NULL;

	/* allocate case_t */
	p = NEW(struct price_t);
	if (p == NULL){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,			
			STR_ERR("%s", "can't allocate struct price_t")); 
		return NULL;
	}	

	if (!id){
		/* create new uuid */
		uuid4_init();
		uuid4_generate(p->id);
	} else
		strcpy(p->id, id);

	/* set values */
#define PRICES_COLUMN_TEXT(member, number, title)\
  if (member){\
		kdata2_set_text_for_uuid(kdata, PRICES_TABLENAME, title, member, p->id);\
		p->member = strdup(member);\
	}
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
	
	return p;
}

/* callback all prices */
 void 
prozubi_price_foreach(
		kdata2_t   *kdata,
		const char *predicate,
		void       *user_data,
		int        (*callback)(void *user_data, struct price_t *p)
		)
{
	char SQL[2*BUFSIZ] = "SELECT ";
	int res;
	sqlite3_stmt *stmt;
	const unsigned char *value;
	
	/* check kdata */
	if (!kdata){
		return;
	}
	if (!kdata->db){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("%s", "kdata->db is NULL"));
		return;
	}

	/* create SQL string */
#define PRICES_COLUMN_TEXT(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, PRICES_TABLENAME);
	strcat(SQL, " ");
	if (predicate)
		strcat(SQL, predicate);

	/* start SQLite request */
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db)));	
		return;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
		int i;
		struct price_t *p = NEW(struct price_t);
		if (p == NULL){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,				
				STR_ERR("%s", "can't allocate struct price_t")); 
			return;
		}	

		/* iterate columns */
		for (i = 0; i < PRICES_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define PRICES_COLUMN_TEXT(member, number, title) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					if (value){\
						p->member = strdup((char*)value);\
						p->len_##member = len;\
					} else {\
						p->member = NULL;\
					}\
					break;\
				}; 

			PRICES_COLUMNS

#undef PRICES_COLUMN_TEXT			

				default:
					break;					
			}
		}		

		/* handle price id */
		value = sqlite3_column_text(stmt, i);				
		strncpy(p->id, (const char *)value, sizeof(p->id) - 1);
		p->id[sizeof(p->id) - 1] = 0;		

		/* callback */
		if (callback)
			if (callback(user_data, p))
				break;		
	}	

	sqlite3_finalize(stmt);
}

#define PRICES_COLUMN_TEXT(member, number, title)\
 int prozubi_prices_set_##number (kdata2_t *p, struct price_t *c,\
		const char *text, bool update)\
{\
	if (update)\
		if (!kdata2_set_text_for_uuid(p, PRICES_TABLENAME, title, text, c->id))\
			return -1;\
	if(c->member)\
		free(c->member);\
	c->member = strdup(text);\
	c->len_##member = strlen(text);\
	return 0;\
}
		PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT			

 int prozubi_prices_set_text(
		PRICES key, kdata2_t *p, struct price_t *c, const char *text, bool update)
{
	switch (key) {
#define PRICES_COLUMN_TEXT(member, number, title) case number:\
		return prozubi_prices_set_##number(p, c, text, update);\
		
		PRICES_COLUMNS

#undef PRICES_COLUMN_TEXT			
		
		default:
			break;
	}
	return -1;
}

 void
prozubi_prices_free(struct price_t *d){
	if (d){

#define PRICES_COLUMN_TEXT(member, number, title) if(d->member) free(d->member); 
		PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT			
		
		free(d);
		d = NULL;
	}
}

 int prozubi_price_remove(
		kdata2_t *p, struct price_t *c
		)
{
	return kdata2_remove_for_uuid(p, PRICES_TABLENAME, c->id);
}
