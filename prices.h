/**
 * File              : prices.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 17.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PRICES_H
#define PRICES_H

#include "prozubilib_conf.h"

#include "enum.h"
#include "alloc.h"

#include <string.h>
#include <stdlib.h>

#define PRICES_TABLENAME "ZPRICES"

/*
 * PRICES_COLUMN_TEXT(struct member, enum number, SQLite column title, size)
 */
#define PRICES_COLUMNS \
	PRICES_COLUMN_TEXT(about,         PRICEABOUT,         "ZABOUT")\
	PRICES_COLUMN_TEXT(category,      PRICECATEGORY,      "ZCATEGORY")\
	PRICES_COLUMN_TEXT(kod,           PRICEKOD,           "ZKOD")\
	PRICES_COLUMN_TEXT(price,         PRICEPRICE,         "ZPRICE")\
	PRICES_COLUMN_TEXT(title,         PRICETITLE,         "ZTITLE")

struct price_t {
	uuid4_str id;         /* uuid of the price */

#define PRICES_COLUMN_TEXT(member, number, title) char * member; size_t len_##member; 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
};


BEGIN_ENUM(PRICES)
#define PRICES_COLUMN_TEXT(member, number, title) DECL_ENUM_ELEMENT(number), 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT

	PRICES_COLS_NUM,
END_ENUM(PRICES)

BEGIN_ENUM_STRING(PRICES)
#define PRICES_COLUMN_TEXT(member, number, title) DECL_ENUM_STRING_ELEMENT(number), 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
END_ENUM_STRING(PRICES)	

static void	
prozubi_prices_table_init(struct kdata2_table **prices){
	kdata2_table_init(prices, PRICES_TABLENAME,

#define PRICES_COLUMN_TEXT(member, number, title) KDATA2_TYPE_TEXT, title, 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
			
			NULL); 
} 

/* allocate and init new price */
static struct price_t *
prozubi_price_new(
		kdata2_t *kdata,
#define PRICES_COLUMN_TEXT(member, number, title) const char * member, 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
		const char *id
		)
{
	/* allocate case_t */
	struct price_t *p = NEW(struct price_t, 
			ERR("%s", "can't allocate struct price_t"), return NULL);

	if (!id){
		/* create new uuid */
		UUID4_STATE_T state; UUID4_T identifier;
		uuid4_seed(&state);
		uuid4_gen(&state, &identifier);
		if (!uuid4_to_s(identifier, p->id, 37)){
			ERR("%s", "can't generate uuid");
			return NULL;
		}
	} else
		strcpy(p->id, id);

	/* set values */
#define PRICES_COLUMN_TEXT(member, number, title)\
   	if (member)\
		kdata2_set_text_for_uuid(kdata, PRICES_TABLENAME, title, member, p->id);	
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
	
	return p;
}

/* callback all prices */
static void 
prozubi_price_foreach(
		kdata2_t   *kdata,
		void       *user_data,
		int        (*callback)(void *user_data, struct price_t *p)
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

	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define PRICES_COLUMN_TEXT(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, PRICES_TABLENAME);

	/* start SQLite request */
	int res;
	sqlite3_stmt *stmt;
	
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db));	
		return;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
	
		struct price_t *p = NEW(struct price_t, 
				ERR("%s", "can't allocate struct price_t"), return);

		/* iterate columns */
		int i;
		for (i = 0; i < PRICES_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define PRICES_COLUMN_TEXT(member, number, title) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					if (value){\
						char *str = MALLOC(len + 1,\
							ERR("can't allocate string with len: %ld", len+1), break);\
						strncpy(str, (const char *)value, len);\
						str[len] = 0;\
						p->member = str;\
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
		const unsigned char *value = sqlite3_column_text(stmt, i);				
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
static int prozubi_prices_set_##number (kdata2_t *p, struct price_t *c,\
		const char *text, bool update)\
{\
	if (update)\
		if (kdata2_set_text_for_uuid(p, PRICES_TABLENAME, title, text, c->id))\
			return -1;\
	if(c->member)\
		free(c->member);\
	size_t len = strlen(text);\
   	c->member = MALLOC(len + 1, ERR("can't allocate size: %ld", len + 1), return -1);\
	strncpy(c->member, text, len);\
	c->len_##member = len;\
	return 0;\
}
		PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT			

static int prozubi_prices_set_text(
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

static void
prozubi_prices_free(struct price_t *d){
	if (d){

#define PRICES_COLUMN_TEXT(member, number, title) if(d->member) free(d->member); 
		PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT			
		
		free(d);
		d = NULL;
	}
}

#endif /* ifndef PRICES_H */
