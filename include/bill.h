/**
 * File              : bill.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.12.2023
 * Last Modified Date: 13.09.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef BILL_H
#define BILL_H

#include "prozubilib_conf.h"

typedef enum BILL_TYPE {
	BILL_TYPE_ITEM,
	BILL_TYPE_TOTAL_PRICE,
	
} BILL_TYPE;

struct bill_t {
	cJSON *bill;
	BILL_TYPE type;
	int itemIndex;
	char * title;
	char * kod;
	char * price;
	char * count;
	char * total;
};

struct bill_t *
_bill_new(
		prozubi_t *p,
		cJSON *bill,
		BILL_TYPE type,
		int itemIndex,
		char * title,
		char * kod,
		char * price,
		char * count,
		char * total);

extern void
prozubi_bill_free(struct bill_t *t);

extern void
prozubi_bill_foreach(
		prozubi_t *p,
		cJSON *bill,
		void * userdata,
		void (*callback)(
			void *userdata,
			struct bill_t *t)
		);

extern cJSON *
prozubi_bill_add_item(
		prozubi_t *kdata,
		cJSON *bill,
		const char *title,
		const char *kod,
		int price,
		int count
		);

extern void
prozubi_bill_remove_item(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index
		);

extern cJSON_bool
prozubi_bill_set_item_title(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		const char *title
		);

extern cJSON_bool
prozubi_bill_set_item_kod(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		const char *kod
		);

extern cJSON_bool
prozubi_bill_set_item_price(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		int price
		);

extern cJSON_bool
prozubi_bill_set_item_count(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		int count
		);

extern void _prozubi_bill_to_rtf_cb(
		void *d, struct bill_t *t);

extern size_t prozubi_bill_to_rtf(
		prozubi_t *p, cJSON *bill, char **rtf);

#endif /* ifndef BILL_H */
