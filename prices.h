/**
 * File              : prices.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 24.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PRICES_H
#define PRICES_H

#include "prozubilib_conf.h"

#include "enum.h"

#include <string.h>
#include <stdlib.h>

#define PRICES_TABLENAME "ZPRICES"

/*
 * PRICES_COLUMN_TEXT(struct member, enum number, SQLite column title, size)
 */
#define PRICES_COLUMNS \
	PRICES_COLUMN_TEXT(about,         PRICEABOUT,         "ZABOUT",         512)\
	PRICES_COLUMN_TEXT(category,      PRICECATEGORY,      "ZCATEGORY",      128)\
	PRICES_COLUMN_TEXT(kod,           PRICEKOD,           "ZKOD",           32)\
	PRICES_COLUMN_TEXT(price,         PRICEPRICE,         "ZPRICE",         32)\
	PRICES_COLUMN_TEXT(title,         PRICETITLE,         "ZTITLE",         128)

struct price_t {
	uuid4_str id;         /* uuid of the price */

#define PRICES_COLUMN_TEXT(member, number, title, size) char member[size]; 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
};


BEGIN_ENUM(PRICES) {
#define PRICES_COLUMN_TEXT(member, number, title, size) DECL_ENUM_ELEMENT(number), 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT

	PRICES_COLS_NUM,
}
END_ENUM(PRICES)

BEGIN_ENUM_STRING(PRICES) {
#define PRICES_COLUMN_TEXT(member, number, title, size) DECL_ENUM_STRING_ELEMENT(number), 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
}
END_ENUM_STRING(PRICES)	

static void	
prozubi_prices_table_init(struct kdata2_table **prices){
	kdata2_table_init(prices, PRICES_TABLENAME,

#define PRICES_COLUMN_TEXT(member, number, title, size) KDATA2_TYPE_TEXT, title, 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
			
			NULL); 
} 

/* allocate and init new price */
static struct price_t *
prozubi_price_new(
		kdata2_t *kdata,
#define PRICES_COLUMN_TEXT(member, number, title, size) const char * member, 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
		const char *id
		)
{
	/* allocate case_t */
	struct price_t *p = malloc(sizeof(struct price_t));
	if (!p){
		ERR("%s", "can't allocate struct price_t");
		return NULL;
	}

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
#define PRICES_COLUMN_TEXT(member, number, title, size)\
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

	struct price_t p;
	
	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define PRICES_COLUMN_TEXT(member, number, title, size) strcat(SQL, title); strcat(SQL, ", "); 
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
		/* iterate columns */
		int i;
		for (i = 0; i < PRICES_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define PRICES_COLUMN_TEXT(member, number, title, size) \
				case number:\
				{\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					if (value){\
						strncpy(p.member, (const char *)value, size - 1);\
						p.member[size - 1] = 0;\
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
		strncpy(p.id, (const char *)value, sizeof(p.id) - 1);
		p.id[sizeof(p.id) - 1] = 0;		

		/* callback */
		if (callback)
			if (callback(user_data, &p))
				break;		
	}	

	sqlite3_finalize(stmt);
}

#endif /* ifndef PRICES_H */
