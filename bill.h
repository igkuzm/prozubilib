/**
 * File              : bill.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.12.2023
 * Last Modified Date: 01.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef BILL_H
#define BILL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kdata2/kdata2.h"
#include "prozubilib_conf.h"
#include "log.h"
#include "alloc.h"
#include "kdata2/cYandexDisk/cJSON.h"

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

static struct bill_t *
_bill_new(
		prozubi_t *p,
		cJSON *bill,
		BILL_TYPE type,
		int itemIndex,
		char * title,
		char * kod,
		char * price,
		char * count,
		char * total)
{
	struct bill_t *t =
			NEW(struct bill_t,
					if (p->on_error)
						p->on_error(p->on_error_data,
						STR_ERR("can't allocate bill_t")), 
					return NULL);

	t->bill          = bill;
	t->type          = type;
	t->itemIndex     = itemIndex;
	t->title         = title;
	t->kod           = kod;
	t->price         = price;
	t->count         = count;
	t->total         = total;

	return t;
}

static void
prozubi_bill_free(struct bill_t *t)
{
	if (!t)
		return;
	if (t->title)
		free(t->title);
	if (t->kod)
		free(t->kod);
	if (t->price)
		free(t->price);
	if (t->count)
		free(t->count);
	if (t->total)
		free(t->total);

	free(t);
}

static void
prozubi_bill_foreach(
		prozubi_t *p,
		cJSON *bill,
		void * userdata,
		void * (*callback)(
			void *userdata,
			struct bill_t *t)
		)
{
	if (!p)
		return;

	if (!cJSON_IsArray(bill)){
		if (p->on_log)
			p->on_log(p->on_log_data,		
		STR_ERR("%s", "no bill json"));
		return;	
	}

	int total_price = 0, item_i=0;
	cJSON *item;
	cJSON_ArrayForEach(item, bill){
		if (!cJSON_IsObject(item)){
			if (p->on_error)
				p->on_error(p->on_error_data,				
				STR_ERR("can't read planlecheniya item %d", item_i));
			continue;	
		}

		char *kod  = 
			cJSON_GetStringValue(
				cJSON_GetObjectItem(item, "kod"));
		if (!kod) kod = strdup("");

		char *title  =     
			cJSON_GetStringValue(
					cJSON_GetObjectItem(item, "title"));
		if (!title) title = strdup("");

		char *price_str =
			cJSON_GetStringValue(
					cJSON_GetObjectItem(item, "price"));
		if (!price_str) price_str = strdup("");

		char *count_str =
			cJSON_GetStringValue(
					cJSON_GetObjectItem(item, "count"));
		if (!count_str) count_str = strdup("");

		int price = atoi(price_str);
		int count = atoi(count_str);
		int total = price * count;
		char *total_str = (char*)malloc(32);
		sprintf(total_str, "%d", total);

		/* callback item */
		callback(
			userdata, _bill_new(
				p, bill, BILL_TYPE_ITEM, item_i, 
				title, kod, price_str, count_str, total_str));
		
		total_price += total;

		item_i++;
	}

	/* callback total price */
	char *price_title = strdup("Итого:");
	char *price_str = (char*)malloc(32);
	sprintf(price_str, "%d", total_price);
	callback(
		userdata, _bill_new(
			p, bill, BILL_TYPE_TOTAL_PRICE, -1, 
			price_title, NULL, NULL, NULL, price_str));
}

static cJSON *
bill_add_item(
		prozubi_t *kdata,
		cJSON *bill,
		const char *title,
		const char *kod,
		int price,
		int count
		)
{
	if (!kdata)
		return NULL;

	if (!cJSON_IsArray(bill)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("%s", "can't read bill json"));
		return NULL;	
	}

	int index = cJSON_GetArraySize(bill);
	cJSON *item = cJSON_CreateObject();

	char number[32];
	sprintf(number, "%d", index+1);
	cJSON_AddItemToObject(item, "number", 
			cJSON_CreateString(number));
	cJSON_AddItemToObject(item, "isItem", 
			cJSON_CreateString("1"));
	cJSON_AddItemToObject(item, "kod", 
			cJSON_CreateString(kod));
	cJSON_AddItemToObject(item, "title", 
			cJSON_CreateString(title));
	char price_str[32];
	sprintf(price_str, "%d", price);
	cJSON_AddItemToObject(item, "price", 
			cJSON_CreateString(price_str));
	char count_str[32];
	sprintf(count_str, "%d", count);
	cJSON_AddItemToObject(item, "count", 
			cJSON_CreateString(count_str));
	char child_str[32];
	sprintf(child_str, "%d", index);
	cJSON_AddItemToObject(item, "childIndex",
			cJSON_CreateString(child_str));
	char total_str[32];
	sprintf(total_str, "%d", price * count);
	cJSON_AddItemToObject(item, "total", 
			cJSON_CreateString(total_str));
	cJSON_AddItemToObject(item, "parentIndex", 
			cJSON_CreateString("0"));

	cJSON_AddItemToArray(bill, item);

	return item;
}

static void
prozubi_bill_remove_item(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index
		)
{
	if (!kdata)
		return;

	if (!cJSON_IsArray(bill)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("%s", "can't read bill json"));
		return;	
	}

	cJSON *item = cJSON_GetArrayItem(bill, item_index);
	if (!cJSON_IsObject(item)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,			
		STR_ERR("can't read bill item %d", item_index));
		return;	
	}
	
	cJSON_DeleteItemFromArray(bill, item_index);			
}


static cJSON_bool
prozubi_bill_set_item_title(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		const char *title
		)
{
	if (!kdata)
		return cJSON_False;	

	if (!cJSON_IsArray(bill)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("%s", "can't read bill json"));
		return cJSON_False;	
	}

	cJSON *item = cJSON_GetArrayItem(bill, item_index);
	if (!cJSON_IsObject(item)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,			
		STR_ERR("can't read bill item %d", item_index));
		return cJSON_False;	
	}
	
	return 
		cJSON_ReplaceItemInObject(item, "title", 
				cJSON_CreateString(title));
}

static cJSON_bool
prozubi_bill_set_item_kod(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		const char *kod
		)
{
	if (!kdata)
		return cJSON_False;	

	if (!cJSON_IsArray(bill)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("%s", "can't read bill json"));
		return cJSON_False;	
	}

	cJSON *item = cJSON_GetArrayItem(bill, item_index);
	if (!cJSON_IsObject(item)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,			
		STR_ERR("can't read bill item %d", item_index));
		return cJSON_False;	
	}
	
	return 
		cJSON_ReplaceItemInObject(item, "title", 
				cJSON_CreateString(kod));
}

static cJSON_bool
prozubi_bill_set_item_price(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		int price
		)
{
	if (!kdata)
		return cJSON_False;	

	if (!cJSON_IsArray(bill)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("%s", "can't read bill json"));
		return cJSON_False;	
	}

	cJSON *item = cJSON_GetArrayItem(bill, item_index);
	if (!cJSON_IsObject(item)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,			
		STR_ERR("can't read bill item %d", item_index));
		return cJSON_False;	
	}
	
	char *count;
	cJSON *count_j =  cJSON_GetObjectItem(item, "count"); 
	if (count_j)
		count = cJSON_GetStringValue(count_j);
	if (!count)
		count = (char*)"";

	char price_str[32];
	sprintf(price_str, "%d", price);
	if (!cJSON_ReplaceItemInObject(item, "price",
				cJSON_CreateString(price_str)))
		return cJSON_False;

	char total[32];
	sprintf(total, "%d", price * atoi(count));
	if (!cJSON_ReplaceItemInObject(item, "total",
				cJSON_CreateString(total)))
		return cJSON_False;
	
	return cJSON_True;
}

static cJSON_bool
prozubi_set_item_count(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		int count
		)
{
	if (!kdata)
		return cJSON_False;	

	if (!cJSON_IsArray(bill)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("%s", "can't read bill json"));
		return cJSON_False;	
	}

	cJSON *item = cJSON_GetArrayItem(bill, item_index);
	if (!cJSON_IsObject(item)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,			
		STR_ERR("can't read bill item %d", item_index));
		return cJSON_False;	
	}
	
	char *price;
	cJSON *price_j =  cJSON_GetObjectItem(item, "price"); 
	if (price_j)
		price = cJSON_GetStringValue(price_j);
	if (!price)
		price = (char*)"";

	char count_str[32];
	sprintf(count_str, "%d", count);
	if (!cJSON_ReplaceItemInObject(item, "count", cJSON_CreateString(count_str)))
		return cJSON_False;

	char total[32];
	sprintf(total, "%d", count * atoi(price));
	if (!cJSON_ReplaceItemInObject(item, "total", cJSON_CreateString(total)))
		return cJSON_False;
	
	return cJSON_True;
}

#endif /* ifndef BILL_H */
