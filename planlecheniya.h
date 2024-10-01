/**
 * File              : planlecheniya.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.04.2023
 * Last Modified Date: 01.10.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PLANLECHENIYA_H
#define PLANLECHENIYA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kdata2/kdata2.h"
#include "prozubilib_conf.h"
#include "alloc.h"
#include "kdata2/cYandexDisk/cJSON.h"
#include "rtf.h"
#include "str.h"
#include "images.h"
#include "cases.h"
#include "text_on_bitmap.h"

typedef enum PLANLECHENIYA_TYPE {
	PLANLECHENIYA_TYPE_STAGE,
	PLANLECHENIYA_TYPE_ITEM,
	PLANLECHENIYA_TYPE_STAGE_PRICE,
	PLANLECHENIYA_TYPE_STAGE_DURATION,
	PLANLECHENIYA_TYPE_TOTAL_PRICE,
	PLANLECHENIYA_TYPE_TOTAL_DURATION,
	
} PLANLECHENIYA_TYPE;

struct planlecheniya_t {
	cJSON *planlecheniya;
	PLANLECHENIYA_TYPE type;
	int stageIndex;
	int itemIndex;
	char * title;
	char * kod;
	char * price;
	char * count;
	char * total;
};

#define _planlecheniya_on_error(p, ret, ...)\
({\
 if (p->on_error){\
	char _s[BUFSIZ];\
	sprintf(_s, __VA_ARGS__);\
	p->on_error(p->on_error_data, _s);\
 }\
	ret;\
})

static struct planlecheniya_t *
_planlecheniya_new(
		prozubi_t *p,
		cJSON *planlecheniya,
		PLANLECHENIYA_TYPE type,
		int stageIndex,
		int itemIndex,
		char * title,
		char * kod,
		char * price,
		char * count,
		char * total)
{
	struct planlecheniya_t *t =
			NEW(struct planlecheniya_t,
					if (p->on_error)
						p->on_error(p->on_error_data,
						STR_ERR(
							"can't allocate planlecheniya_t")); 
					return NULL);

	t->planlecheniya = planlecheniya;
	t->type          = type;
	t->stageIndex    = stageIndex;
	t->itemIndex     = itemIndex;
	t->title         = title;
	t->kod           = kod;
	t->price         = price;
	t->count         = count;
	t->total         = total;

	return t;
}

static void
prozubi_planlecheniya_free(struct planlecheniya_t *t)
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

static int _planlecheniya_check(
		prozubi_t *p,
		cJSON *planlecheniya)
{
	if (!p)
		return -1;

	if (!cJSON_IsArray(planlecheniya))
		_planlecheniya_on_error(p, return -1, 
				"no planlecheniya json");
	return 0;
}

static void
prozubi_planlecheniya_foreach(
		prozubi_t *p,
		cJSON *planlecheniya,
		void * userdata,
		void * (*callback)(
			void *userdata,
			void *parent,
			struct planlecheniya_t *t)
		)
{
	if (_planlecheniya_check(p, planlecheniya))
		return;

	int total_price = 0;
	int total_duration = 0;
	int stage_i = 0;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage))
			_planlecheniya_on_error(p, continue, 
					"can't read planlecheniya stage %d", stage_i);
		
		cJSON *time = cJSON_GetObjectItem(
				stage, "time");
		if (!cJSON_IsString(time))
			_planlecheniya_on_error(p, continue, 
					"can't read planlecheniya time of stage %d", stage_i);
		
		char * duration_str = cJSON_GetStringValue(time);
		if (!duration_str) duration_str = (char *)"";
		int duration = atoi(duration_str);
		total_duration += duration;

		/* callback stage */
		char *stage_str = (char*)malloc(32);
		sprintf(stage_str, "Этап №%d", stage_i + 1);
		void *stage_ptr = callback(
				userdata, NULL, _planlecheniya_new(
					p, planlecheniya, PLANLECHENIYA_TYPE_STAGE,
					stage_i, -1, 
					stage_str, NULL, NULL,
					NULL, NULL));
		
		cJSON *array = cJSON_GetObjectItem(
				stage, "array");
		if (!cJSON_IsArray(array))
			_planlecheniya_on_error(p, continue, 
					"can't read planlecheniya array of stage %d", stage_i);
		
		int stage_price = 0;
		int item_i = 0;
		cJSON *item;
		cJSON_ArrayForEach(item, array){
			if (!cJSON_IsObject(item))
				_planlecheniya_on_error(p, continue, 
						"can't read planlecheniya item %d of stage %d", 
						item_i, stage_i);

			char *kod = 
				cJSON_GetStringValue(
					cJSON_GetObjectItem(
						item, "kod"));
			if (!kod) kod = strdup("");

			char *title = 
				cJSON_GetStringValue(
					cJSON_GetObjectItem(
						item, "title"));
			if (!title) title = strdup("");

			char *price_str =
				cJSON_GetStringValue(
					cJSON_GetObjectItem(
						item, "price"));
			if (!price_str) price_str = strdup("");

			char *count_str =
				cJSON_GetStringValue(
					cJSON_GetObjectItem(
						item, "count"));
			if (!count_str) count_str = strdup("");

			int price = atoi(price_str);
			int count = atoi(count_str);
			int total = price * count;
			char *total_str = (char*)malloc(32);
			if (!total_str) 
				return;
			sprintf(total_str, "%d", total);

			/* callback item */
			callback(
				userdata, stage_ptr, _planlecheniya_new(
					p, planlecheniya, PLANLECHENIYA_TYPE_ITEM,
					stage_i, item_i, 
					title, kod, price_str, count_str,
					total_str));
			
			stage_price += total; 
			total_price += total;

			item_i++;
		}

		/* callback stage price and duration */
		char *stage_price_title = (char*)malloc(64);
		if (!stage_price_title)
			return;
		sprintf(stage_price_title,
				"Итого за %d этап:", stage_i + 1);
		char *stage_price_str = (char*)malloc(32);
		if (!stage_price_str)
			return;
		sprintf(stage_price_str, "%d", stage_price);
		callback(
			userdata, stage_ptr, _planlecheniya_new(
				p, planlecheniya, 
				PLANLECHENIYA_TYPE_STAGE_PRICE, 
				stage_i, -1, 
				stage_price_title, NULL, 
				NULL, NULL, stage_price_str));
		
		char *stage_duration_str = (char*)malloc(32);
		if (!stage_duration_str)
			return;
		sprintf(stage_duration_str, "%d", duration);
		callback(
			userdata, stage_ptr, _planlecheniya_new(
				p, planlecheniya,
				PLANLECHENIYA_TYPE_STAGE_DURATION,
				stage_i, -1, 
				strdup(
					"Продолжительность этапа (мес.):"), 
				NULL, NULL, 	stage_duration_str,
				(char*)""));
		
		stage_i++;
	}

	/* callback total price and duration */
	char *total_price_str = (char*)malloc(32);
	if (!total_price_str)
		return;
	sprintf(total_price_str, "%d", total_price);
	callback(
		userdata, NULL, _planlecheniya_new(
			p, planlecheniya,
			PLANLECHENIYA_TYPE_TOTAL_PRICE, 
			-1, -1, 
			strdup(
				"Общая стоимость по плану:"), 
			NULL, NULL, NULL, 
			total_price_str));
		
	char *total_duration_str = (char*)malloc(32);
	if (!total_duration_str)
		return;
	sprintf(
			total_duration_str, "%d", total_duration);
	callback(
		userdata, NULL, _planlecheniya_new(
			p, planlecheniya,
			PLANLECHENIYA_TYPE_TOTAL_DURATION,
			-1, -1, 
			strdup(
				"Общая продолжительность лечения (мес.):"),
			NULL, NULL, total_duration_str, 
			NULL));
}

static cJSON *
prozubi_planlecheniya_add_stage(
		prozubi_t *p,
		cJSON *planlecheniya
		)
{
	if (_planlecheniya_check(p, planlecheniya))
		return NULL;

	/* add new stage */
	cJSON *stage = cJSON_CreateObject();
	cJSON_AddItemToObject(stage, "time",
			cJSON_CreateString("3"));
	cJSON *array = cJSON_CreateArray();
	cJSON_AddItemToObject(stage, "array",
			array);

	cJSON_AddItemToArray(planlecheniya, stage);

	return stage;
}

static void
prozubi_planlecheniya_remove_stage(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index
		)
{
	if (_planlecheniya_check(p, planlecheniya))
		return;

	cJSON_DeleteItemFromArray(
			planlecheniya, stage_index);			
}

static cJSON *
_planlecheniya_get_stage(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index)
{
	if (_planlecheniya_check(p, planlecheniya))
		return NULL;
	
	cJSON *stage = 
		cJSON_GetArrayItem(planlecheniya, stage_index);

	if (!stage)	
		_planlecheniya_on_error(p, return NULL, 
				"can't read planlecheniya array of stage %d", 
				stage_index);
	
	return stage;
}

static cJSON *
_planlecheniya_get_stage_items_array(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index)
{
	cJSON *stage = 
		_planlecheniya_get_stage(p, planlecheniya, stage_index);
	if (!stage)
		return NULL;

	cJSON *items_array = 
		cJSON_GetObjectItem(stage, "array");
	if (!cJSON_IsArray(items_array))
		_planlecheniya_on_error(p, return NULL, 
				"can't get items array of stage %d", 
				stage_index);
	return items_array;
}

static cJSON *
_planlecheniya_get_item(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index)
{
	cJSON *items_array = 
		_planlecheniya_get_stage_items_array(
				p, planlecheniya, stage_index);
	if (!items_array)
		return NULL;

	cJSON *item = 
		cJSON_GetArrayItem(items_array, item_index);
	if (!cJSON_IsObject(item))
		_planlecheniya_on_error(p, return NULL, 
				"can't get item %d of stage %d",
				item_index, stage_index);	
	return item;
}

static cJSON *
prozubi_planlecheniya_insert_item(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		const char *title,
		const char *kod,
		int price,
		int count
		)
{
	cJSON *items_array = 
		_planlecheniya_get_stage_items_array(
				p, planlecheniya, stage_index);
	if (!items_array)
		return NULL;

	int index = item_index;
	if (item_index == -1)
		index = cJSON_GetArraySize(items_array);
	
	cJSON *item = cJSON_CreateObject();

	char number[32];
	sprintf(number, "%d", index+1);
	cJSON_AddItemToObject(
			item, "number", 
			cJSON_CreateString(number));
	cJSON_AddItemToObject(
			item, "isItem", 
			cJSON_CreateString("1"));
	cJSON_AddItemToObject(
			item, "kod", 
			cJSON_CreateString(kod));
	cJSON_AddItemToObject(
			item, "title", 
			cJSON_CreateString(title));
	char price_str[32];
	sprintf(price_str, "%d", price);
	cJSON_AddItemToObject(
			item, "price", 
			cJSON_CreateString(price_str));
	char count_str[32];
	sprintf(count_str, "%d", count);
	cJSON_AddItemToObject(
			item, "count", 
			cJSON_CreateString(count_str));
	char child_str[32];
	sprintf(child_str, "%d", index);
	cJSON_AddItemToObject(
			item, "childIndex", 
			cJSON_CreateString(child_str));
	char total_str[32];
	sprintf(total_str, "%d", price * count);
	cJSON_AddItemToObject(
			item, "total", 
			cJSON_CreateString(total_str));
	char parent_str[32];
	sprintf(parent_str, "%d", stage_index);
	cJSON_AddItemToObject(
			item, "parentIndex", 
			cJSON_CreateString(parent_str));

	if (item_index == -1)
		cJSON_AddItemToArray(items_array, item);
	else 
		cJSON_InsertItemInArray(
			items_array, item_index, item);

	return item;
}


static cJSON *
prozubi_planlecheniya_add_item(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		const char *title,
		const char *kod,
		int price,
		int count
		)
{
	return prozubi_planlecheniya_insert_item(
			p, planlecheniya, stage_index, -1, 
			title, kod, price, count);
}

static void
prozubi_planlecheniya_remove_item(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index
		)
{
	cJSON *items_array = 
		_planlecheniya_get_stage_items_array(
				p, planlecheniya, stage_index);
	if (!items_array)
		return;
	
	cJSON_DeleteItemFromArray(
			items_array, item_index);			
}


static cJSON_bool
prozubi_planlecheniya_set_item_title(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		const char *title
		)
{
	cJSON *item = 
		_planlecheniya_get_item(
				p, planlecheniya, stage_index, item_index);
	if (!item)
		return cJSON_False;
	
	return cJSON_ReplaceItemInObject(
			item, "title", 
			cJSON_CreateString(title));
}

static cJSON_bool
prozubi_planlecheniya_set_item_kod(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		const char *kod
		)
{
	cJSON *item = 
		_planlecheniya_get_item(
				p, planlecheniya, stage_index, item_index);
	if (!item)
		return cJSON_False;
	
	return cJSON_ReplaceItemInObject(
			item, "kod", 
			cJSON_CreateString(kod));
}

static cJSON_bool
prozubi_planlecheniya_set_item_price(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		int price
		)
{
	cJSON *item = 
		_planlecheniya_get_item(
				p, planlecheniya, stage_index, item_index);
	if (!item)
		return cJSON_False;
	
	char *count;
	cJSON *count_j = 
		cJSON_GetObjectItem(item, "count"); 
	if (count_j)
		count = cJSON_GetStringValue(count_j);
	if (!count)
		count = (char*)"";

	char price_str[32];
	sprintf(price_str, "%d", price);
	if (!cJSON_ReplaceItemInObject(
				item, "price", 
				cJSON_CreateString(price_str)))
		return cJSON_False;

	char total[32];
	sprintf(total, "%d", 
			price * atoi(count));
	if (!cJSON_ReplaceItemInObject(
				item, "total", 
				cJSON_CreateString(total)))
		return cJSON_False;
			
	return cJSON_True;
}

static cJSON_bool
prozubi_planlecheniya_set_item_count(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		int count
		)
{
	cJSON *item = 
		_planlecheniya_get_item(
				p, planlecheniya, stage_index, item_index);
	if (!item)
		return cJSON_False;
	
	char *price;
	cJSON *price_j =
		cJSON_GetObjectItem(item, "price"); 
	if (price_j)
		price = cJSON_GetStringValue(price_j);
	if (!price)
		price = (char*)"";

	char count_str[32];
	sprintf(count_str, "%d", count);
	if (!cJSON_ReplaceItemInObject(item, "count", 
				cJSON_CreateString(count_str)))
		return cJSON_False;

	char total[32];
	sprintf(total, "%d", 
			count * atoi(price));
	if (!cJSON_ReplaceItemInObject(
				item, "total", 
				cJSON_CreateString(total)))
		return cJSON_False;
			
	return cJSON_True;
}

static cJSON_bool
prozubi_planlecheniya_set_stage_duration(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int duration
		)
{
	cJSON *stage = 
		_planlecheniya_get_stage(p, planlecheniya, stage_index);
	if (!stage)
		return cJSON_False;

	char duration_str[32];
	sprintf(duration_str, "%d", duration);
	if (!cJSON_ReplaceItemInObject(stage, 
				"time", 
				cJSON_CreateString(
					duration_str)))
		return cJSON_False;
	
	return cJSON_True;
}

#define ZFORMULA_COORDS \
	ZFORMULA_COORD(z18, 17,  102)\
	ZFORMULA_COORD(z17, 58,  102)\
	ZFORMULA_COORD(z16, 98,  102)\
	ZFORMULA_COORD(z15, 134, 102)\
	ZFORMULA_COORD(z14, 168, 102)\
	ZFORMULA_COORD(z13, 203, 102)\
	ZFORMULA_COORD(z12, 236, 102)\
	ZFORMULA_COORD(z11, 269, 102)\
	ZFORMULA_COORD(z21, 314, 102)\
	ZFORMULA_COORD(z22, 348, 102)\
	ZFORMULA_COORD(z23, 379, 102)\
	ZFORMULA_COORD(z24, 416, 102)\
	ZFORMULA_COORD(z25, 447, 102)\
	ZFORMULA_COORD(z26, 485, 102)\
	ZFORMULA_COORD(z27, 522, 102)\
	ZFORMULA_COORD(z28, 566, 102)\
	ZFORMULA_COORD(z48, 25,  165)\
	ZFORMULA_COORD(z47, 69,  165)\
	ZFORMULA_COORD(z46, 112, 165)\
	ZFORMULA_COORD(z45, 151, 165)\
	ZFORMULA_COORD(z44, 185, 165)\
	ZFORMULA_COORD(z43, 216, 165)\
	ZFORMULA_COORD(z42, 249, 165)\
	ZFORMULA_COORD(z41, 278, 165)\
	ZFORMULA_COORD(z31, 308, 165)\
	ZFORMULA_COORD(z32, 336, 165)\
	ZFORMULA_COORD(z33, 367, 165)\
	ZFORMULA_COORD(z34, 399, 165)\
	ZFORMULA_COORD(z35, 432, 165)\
	ZFORMULA_COORD(z36, 469, 165)\
	ZFORMULA_COORD(z37, 511, 165)\
	ZFORMULA_COORD(z38, 555, 165)


static int
_prozubi_zformula_image(
		prozubi_t *p, struct case_t *c, 
		const char *imagepath, 
		void **data, size_t *len)
{
	// open image
	int i, w, h;
	stbi_uc *img = 
		stbi_load(imagepath, &w, &h, 
				NULL, 4);
	if (!img)
		_planlecheniya_on_error(p, return 1, 
				"can't load image with path: %s\n", imagepath);

	// write zubformula in image
#define ZFORMULA_COORD(num, x, y) \
	if (c->num && c->num[0]){\
		if (text_on_bitmap(\
				img, w, h, 4, \
				c->num, 0xFF, "FreeSans.ttf", 15, x, y))\
		_planlecheniya_on_error(p,\
				stbi_image_free(img);\
				return 1, \
				"can't print text on image"); \
		;\
	}
	ZFORMULA_COORDS
#undef ZFORMULA_COORD

	// convert bitmap to jpeg
	struct prozubi_image_jpg_write_s s;
	s.data = malloc(1);
	if (!s.data){
		_planlecheniya_on_error(p,
				return 1, "can't allocate memory");
	}
	s.len = 0;
	s.p = p;

	if (stbi_write_png_to_func(
			_prozubi_image_jpg_write_func,
			&s, 
			w, h, 4, 
			img, 0) == 0)
		_planlecheniya_on_error(p, 
				stbi_image_free(img);
				if (s.data)
					free(s.data);
				return 1,
				"can't convert bitmap to png");
	
	// free bitmap	
	stbi_image_free(img);	
	
	// return data
	*data = s.data;
	*len = s.len;

	return 0;
}

static void * _prozubi_planlecheniya_to_rtf_cb(
		void *d, void *p, struct planlecheniya_t *t)
{
	struct str *s = (struct str *)d;
	switch (t->type) {
		case PLANLECHENIYA_TYPE_STAGE:
			{
				str_appendf(s,
					"\\pard\\par\\ql \\b %s \\b0 \\ \n", 
					t->title);

				const char *titles[] =
				{
					"\\b № \\b0",
					"\\b Наименование работы (услуги) \\b0"
					"\\b Количество \\b0"
					"\\b Цена \\b0"
					"\\b Сумма \\b0"
				};
				int width[] = 
					{400, 6854, 1000, 1000, 1000};
				char *tbl = 
					rtf_table_header(5, titles, width);
				
				str_append(
						s, tbl, strlen(tbl));
				free(tbl);
				break;
			}
		case PLANLECHENIYA_TYPE_ITEM:
			{
				char row[256];
				sprintf(row, 
						"%d=%s=%s=%s=%s"
						, t->itemIndex + 1
						, t->title
						, t->count
						, t->price
						, t->total
				);
				char *tbl = 
					rtf_table_row_from_string(row, "=");
				str_append(
						s, tbl, strlen(tbl));
				free(tbl);
				break;
			}
		case PLANLECHENIYA_TYPE_STAGE_PRICE:
			{
				char row[256];
				sprintf(row, 
						"=\\b %s \\b0==\\b %s руб. \\b0="
						, t->title
						, t->total
				);
				char *tbl = 
					rtf_table_row_from_string(row, "=");
				str_append(
						s, tbl, strlen(tbl));
				free(tbl);
				break;
			}
		case PLANLECHENIYA_TYPE_STAGE_DURATION:
			{
				char row[256];
				sprintf(row, 
						"==\\b %s \\b0===\\b %s мес. \\b0"
						, t->title
						, t->count
				);
				char *tbl = 
					rtf_table_row_from_string(row, "=");
				str_append(
						s, tbl, strlen(tbl));
				free(tbl);
				break;
			}
		case PLANLECHENIYA_TYPE_TOTAL_PRICE:
			{
				char str[256];
				sprintf(str, 
						"\\lastrow\n\n%s: %s руб.\n"
						, t->title
						, t->total
				);
				char *rtf_string = 
					rtf_from_utf8(str);
				str_append(
						s, rtf_string, strlen(rtf_string));
				free(rtf_string);
				break;
			}
		case PLANLECHENIYA_TYPE_TOTAL_DURATION:
			{
				char str[256];
				sprintf(str, 
						"%s: %s\n"
						, t->title
						, t->count
				);
				char *rtf_string = 
					rtf_from_utf8(str);
				str_append(
						s, rtf_string, strlen(rtf_string));
				free(rtf_string);
				break;
			}

		default:
			break;
	}
	return NULL;
}

static size_t prozubi_planlecheniya_to_rtf(
		prozubi_t *p, cJSON *planlecheniya, char **rtf)
{
	struct str s;
	if (str_init(&s))
		_planlecheniya_on_error(p, return 0, 
				"can't allocate memory");

	// create RTF table
	prozubi_planlecheniya_foreach(
			p, planlecheniya, &s, 
			_prozubi_planlecheniya_to_rtf_cb);

	if (rtf)
		*rtf = s.str;
	else
		free(s.str);
	
	return s.len;
}

static int prozubi_planlecheniya_get_price_total(
		prozubi_t *p, cJSON *planlecheniya)
{
	if (_planlecheniya_check(p, planlecheniya))
		return -1;

	int total_price = 0;
	int stage_i = 0;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage))
			_planlecheniya_on_error(p, continue, 
				"can't read planlecheniya stage %d", stage_i);
		
		cJSON *array = 
			cJSON_GetObjectItem(stage, "array");
		if (!cJSON_IsArray(array))
			_planlecheniya_on_error(p, continue, 
				"can't read items array of stage %d", stage_i);
		
		int stage_price = 0;
		int item_i = 0;
		cJSON *item;
		cJSON_ArrayForEach(item, array){
			if (!cJSON_IsObject(item))
				_planlecheniya_on_error(p, continue, 
						"can't read planlecheniya item %d"
						" of stage %d", 
						item_i, stage_i);

			char *price_str = 
				cJSON_GetStringValue(
						cJSON_GetObjectItem(
							item, "price"));
			if (!price_str) price_str = strdup("");

			char *count_str = 
				cJSON_GetStringValue(
						cJSON_GetObjectItem(
							item, "count"));
			if (!count_str) count_str = strdup("");

			int price = atoi(price_str);
			int count = atoi(count_str);
			int total = price * count;

			total_price += total;

			item_i++;
		}
		stage_i++;
	}
	return total_price;
}

static int prozubi_planlecheniya_get_duration_total(
		prozubi_t *p, cJSON *planlecheniya)
{
	if (_planlecheniya_check(p, planlecheniya))
		return -1;
	
	int total_duration = 0;
	int stage_i = 0;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage))
			_planlecheniya_on_error(p, continue, 
				"can't read planlecheniya stage %d", stage_i);
					
		cJSON *time = 
			cJSON_GetObjectItem(stage, "time");
		if (!cJSON_IsString(time))
			_planlecheniya_on_error(p, continue, 
				"can't read time of stage %d", stage_i);
		
		char * duration_str = cJSON_GetStringValue(time);
		if (!duration_str) duration_str = (char*)"";
		int duration = atoi(duration_str);
		total_duration += duration;

		stage_i++;
	}
	return total_duration;
}

#endif /* ifndef PLANLECHENIYA_H */
