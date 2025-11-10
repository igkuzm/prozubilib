/**
 * File              : bill.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.12.2023
 * Last Modified Date: 13.09.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/bill.h"
#include "../include/rtf.h"

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
		char * total)
{
	struct bill_t *t =
			NEW(struct bill_t);
	if (t == NULL){
		ON_ERR(p, "can't allocate bill_t"); 
		return NULL;
	}

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

 void
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

 void
prozubi_bill_foreach(
		prozubi_t *p,
		cJSON *bill,
		void * userdata,
		void (*callback)(
			void *userdata,
			struct bill_t *t)
		)
{
	int total_price = 0, item_i=0;
	cJSON *item;
	char *kod, *title, *price_str_item, *count_str, *total_str, *price_title, *price_str;
	int price, count, total;

	if (!p)
		return;

	if (!cJSON_IsArray(bill)){
		ON_ERR(p, "no bill json");
		return;	
	}

	cJSON_ArrayForEach(item, bill){
		if (!cJSON_IsObject(item)){
			ON_ERR(p, 
					STR("can't read planlecheniya item %d", item_i));
			continue;	
		}

		kod  = cJSON_GetStringValue(
				cJSON_GetObjectItem(item, "kod"));
		if (!kod) kod = strdup("");

		title = cJSON_GetStringValue(
					cJSON_GetObjectItem(item, "title"));
		if (!title) title = strdup("");

		price_str = cJSON_GetStringValue(
					cJSON_GetObjectItem(item, "price"));
		if (!price_str) price_str = strdup("");

		count_str = cJSON_GetStringValue(
					cJSON_GetObjectItem(item, "count"));
		if (!count_str) count_str = strdup("");

		price = atoi(price_str);
		count = atoi(count_str);
		total = price * count;
		total_str = (char*)malloc(32);
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
	price_title = strdup("Итого:");
	price_str = (char*)malloc(32);
	sprintf(price_str, "%d", total_price);
	callback(
		userdata, _bill_new(
			p, bill, BILL_TYPE_TOTAL_PRICE, -1, 
			price_title, NULL, NULL, NULL, price_str));
}

 cJSON *
prozubi_bill_add_item(
		prozubi_t *kdata,
		cJSON *bill,
		const char *title,
		const char *kod,
		int price,
		int count
		)
{
	int index;
	cJSON *item;
	char number[32];
	char price_str[32];
	char count_str[32];
	char child_str[32];
	char total_str[32];

	if (!kdata)
		return NULL;

	if (!cJSON_IsArray(bill)){
		ON_ERR(kdata, "can't read bill json");
		return NULL;	
	}

	index = cJSON_GetArraySize(bill);
	item = cJSON_CreateObject();

	sprintf(number, "%d", index+1);
	cJSON_AddItemToObject(item, "number", 
			cJSON_CreateString(number));
	cJSON_AddItemToObject(item, "isItem", 
			cJSON_CreateString("1"));
	cJSON_AddItemToObject(item, "kod", 
			cJSON_CreateString(kod));
	cJSON_AddItemToObject(item, "title", 
			cJSON_CreateString(title));
	sprintf(price_str, "%d", price);
	cJSON_AddItemToObject(item, "price", 
			cJSON_CreateString(price_str));
	sprintf(count_str, "%d", count);
	cJSON_AddItemToObject(item, "count", 
			cJSON_CreateString(count_str));
	sprintf(child_str, "%d", index);
	cJSON_AddItemToObject(item, "childIndex",
			cJSON_CreateString(child_str));
	sprintf(total_str, "%d", price * count);
	cJSON_AddItemToObject(item, "total", 
			cJSON_CreateString(total_str));
	cJSON_AddItemToObject(item, "parentIndex", 
			cJSON_CreateString("0"));

	cJSON_AddItemToArray(bill, item);

	return item;
}

 void
prozubi_bill_remove_item(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index
		)
{
	cJSON *item;
	if (!kdata)
		return;

	if (!cJSON_IsArray(bill)){
		ON_ERR(kdata, "can't read bill json");
		return;	
	}

	item = cJSON_GetArrayItem(bill, item_index);
	if (!cJSON_IsObject(item)){
		ON_ERR(kdata, STR("can't read bill item %d", item_index));
		return;	
	}
	
	cJSON_DeleteItemFromArray(bill, item_index);			
}


 cJSON_bool
prozubi_bill_set_item_title(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		const char *title
		)
{
	cJSON *item;

	if (!kdata)
		return cJSON_False;	

	if (!cJSON_IsArray(bill)){
		ON_ERR(kdata, "can't read bill json");
		return cJSON_False;	
	}

	item = cJSON_GetArrayItem(bill, item_index);
	if (!cJSON_IsObject(item)){
		ON_ERR(kdata, STR("can't read bill item %d", item_index));
		return cJSON_False;	
	}
	
	return 
		cJSON_ReplaceItemInObject(item, "title", 
				cJSON_CreateString(title));
}

 cJSON_bool
prozubi_bill_set_item_kod(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		const char *kod
		)
{
	cJSON *item;

	if (!kdata)
		return cJSON_False;	

	if (!cJSON_IsArray(bill)){
		ON_ERR(kdata, "can't read bill json");
		return cJSON_False;	
	}

	item = cJSON_GetArrayItem(bill, item_index);
	if (!cJSON_IsObject(item)){
		ON_ERR(kdata, STR("can't read bill item %d", item_index));
		return cJSON_False;	
	}
	
	return 
		cJSON_ReplaceItemInObject(item, "title", 
				cJSON_CreateString(kod));
}

 cJSON_bool
prozubi_bill_set_item_price(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		int price
		)
{
	cJSON *item, *count_j;
	char *count;
	char price_str[32];
	char total[32];

	if (!kdata)
		return cJSON_False;	

	if (!cJSON_IsArray(bill)){
		ON_ERR(kdata, "can't read bill json");
		return cJSON_False;	
	}

	item = cJSON_GetArrayItem(bill, item_index);
	if (!cJSON_IsObject(item)){
		ON_ERR(kdata, STR("can't read bill item %d", item_index));
		return cJSON_False;	
	}
	
	count_j =  cJSON_GetObjectItem(item, "count"); 
	if (count_j)
		count = cJSON_GetStringValue(count_j);
	if (!count)
		count = (char*)"";

	sprintf(price_str, "%d", price);
	if (!cJSON_ReplaceItemInObject(item, "price",
				cJSON_CreateString(price_str)))
		return cJSON_False;

	sprintf(total, "%d", price * atoi(count));
	if (!cJSON_ReplaceItemInObject(item, "total",
				cJSON_CreateString(total)))
		return cJSON_False;
	
	return cJSON_True;
}

 cJSON_bool
prozubi_bill_set_item_count(
		prozubi_t *kdata,
		cJSON *bill,
		int item_index,
		int count
		)
{
	cJSON *item;
	cJSON *price_j;
	char *price;
	char count_str[32];
	char total[32];

	if (!kdata)
		return cJSON_False;	

	if (!cJSON_IsArray(bill)){
		ON_ERR(kdata, "can't read bill json");
		return cJSON_False;	
	}

	item = cJSON_GetArrayItem(bill, item_index);
	if (!cJSON_IsObject(item)){
		ON_ERR(kdata, STR("can't read bill item %d", item_index));
		return cJSON_False;	
	}
	
	price_j =  cJSON_GetObjectItem(item, "price"); 
	if (price_j)
		price = cJSON_GetStringValue(price_j);
	if (!price)
		price = (char*)"";

	sprintf(count_str, "%d", count);
	if (!cJSON_ReplaceItemInObject(item, "count", cJSON_CreateString(count_str)))
		return cJSON_False;

	sprintf(total, "%d", count * atoi(price));
	if (!cJSON_ReplaceItemInObject(item, "total", cJSON_CreateString(total)))
		return cJSON_False;
	
	return cJSON_True;
}

 void _prozubi_bill_to_rtf_cb(
		void *d, struct bill_t *t)
{
	struct str *s = (struct str *)d;
	if (t->type == BILL_TYPE_ITEM)
	{
		const char *row[5];
		int width[] = 
		{400, 6854, 1000, 1000, 1000};
		char *tbl;
		char index[32];
		sprintf(index, "%d", t->itemIndex + 1);
		row[0] = index;
		row[1] = t->title;
		row[2] = t->count;
		row[3] = t->price;
		row[4] = t->total;
		
		tbl = rtf_table_row(5, row, width);
		
		str_append(s, tbl, strlen(tbl));
	}
	else if (t->type == BILL_TYPE_TOTAL_PRICE)
	{
		char title[BUFSIZ];
		char total[BUFSIZ];
		const char *row[5];
		int width[] = 
		{400, 6854, 1000, 1000, 1000};
		char *tbl;

		snprintf(title,BUFSIZ-1, "\\b %s \\b0", t->title);
		snprintf(total,BUFSIZ-1,"\\b %s руб. \\b0", t->total);

		row[0] = "";
		row[1] = title;
		row[2] = "";
		row[3] = "";
		row[4] = total;
		
		tbl = rtf_table_row(5, row, width);	
		
		str_append(s, tbl, strlen(tbl));
		
		str_appendf(s, "\\lastrow\n");
	}
}

 size_t prozubi_bill_to_rtf(
		prozubi_t *p, cJSON *bill, char **rtf)
{
	struct str s;
	// create RTF table
	const char *titles[] =
	{
		"\\b № \\b0",
		"\\b Наименование работы (услуги) \\b0",
		"\\b Количество \\b0",
		"\\b Цена \\b0",
		"\\b Сумма \\b0",
	};
	int width[] = 
		{400, 6854, 1000, 1000, 1000};
	char *tbl = 
		rtf_table_row(5, titles, width);
	
	if (str_init(&s)){
		ON_ERR(p, "can't allocate memory");
		return 0;
	}

	str_append(&s, tbl, strlen(tbl));
	free(tbl);

	// fill RTF table
	prozubi_bill_foreach(
			p, bill, &s, 
			_prozubi_bill_to_rtf_cb);
	
	// bottom
	str_appendf(&s, "\n\\par");
	
	//fprintf(stderr, "OK: \n");

	if (rtf)
		*rtf = s.str;
	else
		free(s.str);
		
	return s.len;
}
