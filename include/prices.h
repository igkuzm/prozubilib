/**
 * File              : prices.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 24.08.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PRICES_H
#define PRICES_H

#include "prozubilib_conf.h"

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

extern void	
prozubi_prices_table_init(struct kdata2_table **prices); 

/* allocate and init new price */
extern struct price_t *
prozubi_price_new(
		kdata2_t *kdata,
#define PRICES_COLUMN_TEXT(member, number, title) const char * member, 
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT
		const char *id
		);

/* callback all prices */
extern void 
prozubi_price_foreach(
		kdata2_t   *kdata,
		const char *predicate,
		void       *user_data,
		int        (*callback)(void *user_data, struct price_t *p)
		);

#define PRICES_COLUMN_TEXT(member, number, title)\
extern int prozubi_prices_set_##number (kdata2_t *p, struct price_t *c,\
		const char *text, bool update);

		PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT			

extern int prozubi_prices_set_text(
		PRICES key, kdata2_t *p, struct price_t *c, const char *text, bool update);

extern void
prozubi_prices_free(struct price_t *d);

static int prozubi_price_remove(
		kdata2_t *p, struct price_t *c
		);

#endif /* ifndef PRICES_H */
