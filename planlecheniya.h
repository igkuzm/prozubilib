/**
 * File              : planlecheniya.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.04.2023
 * Last Modified Date: 27.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PLANLECHENIYA_H
#define PLANLECHENIYA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kdata2/kdata2.h"
#include "prozubilib_conf.h"
#include "log.h"
#include "alloc.h"
#include "kdata2/cYandexDisk/cJSON.h"
#include "rtf.h"
#include "str.h"

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
							"can't allocate planlecheniya_t")), 
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
	if (!p)
		return;

	if (!cJSON_IsArray(planlecheniya)){
		if (p->on_log)
			p->on_log(p->on_log_data,		
		STR_ERR("%s", "no planlecheniya json"));
		return;	
	}

	int total_price = 0;
	int total_duration = 0;
	int stage_i = 0;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage)){
			if (p->on_error)
				p->on_error(p->on_error_data,			
			STR_ERR("can't read planlecheniya stage %d",
				stage_i));
			continue;	
		}
		
		cJSON *time = cJSON_GetObjectItem(
				stage, "time");
		if (!cJSON_IsString(time)){
			if (p->on_error)
				p->on_error(p->on_error_data,			
			STR_ERR(
				"can't read planlecheniya time of stage %d", 
				stage_i));
			continue;	
		}		
		char * duration_str = cJSON_GetStringValue(time);
		if (!duration_str) duration_str = "";
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
		if (!cJSON_IsArray(array)){
			if (p->on_error)
				p->on_error(p->on_error_data,			
			STR_ERR(
				"can't read planlecheniya array of"
				" stage: %d", stage_i));
			continue;	
		}		
		
		int stage_price = 0;
		int item_i = 0;
		cJSON *item;
		cJSON_ArrayForEach(item, array){
			if (!cJSON_IsObject(item)){
				if (p->on_error)
					p->on_error(p->on_error_data,				
							STR_ERR(
								"can't read planlecheniya item %d"
								" of stage %d", item_i, stage_i));
				continue;	
			}

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
		prozubi_t *kdata,
		cJSON *planlecheniya
		)
{
	if (!kdata)
		return NULL;

	if (!cJSON_IsArray(planlecheniya)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR(
				"%s", "can't read planlecheniya json"));
		return NULL;	
	}

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
		prozubi_t *kdata,
		cJSON *planlecheniya,
		int stage_index
		)
{
	if (!kdata)
		return;

	if (!cJSON_IsArray(planlecheniya)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR(
				"%s", "can't read planlecheniya json"));
		return;	
	}
	cJSON_DeleteItemFromArray(
			planlecheniya, stage_index);			
}


static cJSON *
prozubi_planlecheniya_add_item(
		prozubi_t *kdata,
		cJSON *planlecheniya,
		int stage_index,
		const char *title,
		const char *kod,
		int price,
		int count
		)
{
	if (!kdata)
		return NULL;

	if (!cJSON_IsArray(planlecheniya)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR(
				"%s", "can't read planlecheniya json"));
		return NULL;	
	}

	int i = 0;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
			STR_ERR(
				"can't read planlecheniya stage %d", i));
			continue;	
		}
		if (stage_index == i){
			cJSON *array = 
				cJSON_GetObjectItem(stage, "array");
			if (!cJSON_IsArray(array)){
				if (kdata->on_error)
					kdata->on_error(kdata->on_error_data,				
					STR_ERR(
						"can't read array of stage %d", i));
				return NULL;	
			}
			int index = cJSON_GetArraySize(array);

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
			sprintf(parent_str, "%d", i);
			cJSON_AddItemToObject(
					item, "parentIndex", 
					cJSON_CreateString(parent_str));

			cJSON_AddItemToArray(array, item);

			return item;
			}		
		//iterate
		i++;
	}
	return NULL;
}

static void
prozubi_planlecheniya_remove_item(
		prozubi_t *kdata,
		cJSON *planlecheniya,
		int stage_index,
		int item_index
		)
{
	if (!kdata)
		return;

	if (!cJSON_IsArray(planlecheniya)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR(
				"%s", "can't read planlecheniya json"));
		return;	
	}

	cJSON *stage = 
		cJSON_GetArrayItem(
				planlecheniya, stage_index);
	if (!cJSON_IsObject(stage)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,			
		STR_ERR(
			"can't read planlecheniya stage %d", 
			stage_index));
		return;	
	}
	
	cJSON *array = 
		cJSON_GetObjectItem(stage, "array");
	if (!cJSON_IsArray(array)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,				
			STR_ERR(
				"can't read array of stage %d", 
				stage_index));
		return;	
	}
	
	cJSON_DeleteItemFromArray(array, item_index);			
}


static cJSON_bool
prozubi_planlecheniya_set_item_title(
		prozubi_t *kdata,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		const char *title
		)
{
	if (!kdata)
		return 0;

	if (!cJSON_IsArray(planlecheniya)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
		STR_ERR("%s", "can't read planlecheniya json"));
		return cJSON_False;	
	}

	cJSON *stage;
		cJSON_GetArrayItem(
				planlecheniya, stage_index);
	if (stage){
		cJSON *array = 
			cJSON_GetObjectItem(stage, "array");
		if (!cJSON_IsArray(array)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,				
				STR_ERR("can't read array of stage %d", 
					stage_index));
			return cJSON_False;	
		}

		cJSON *item =
			cJSON_GetArrayItem(array, item_index);
		if (item_index){
			return cJSON_ReplaceItemInObject(
					item, "title", 
					cJSON_CreateString(title));
		}
	}
	return cJSON_False;
}

static cJSON_bool
prozubi_planlecheniya_set_item_kod(
		prozubi_t *kdata,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		const char *kod
		)
{
	if (!kdata)
		return 0;

	if (!cJSON_IsArray(planlecheniya)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR(
				"%s", "can't read planlecheniya json"));
		return cJSON_False;
	}

	cJSON *stage =
		cJSON_GetArrayItem(
				planlecheniya, stage_index);
	if (stage){
		cJSON *array = 
			cJSON_GetObjectItem(stage, "array");
		if (!cJSON_IsArray(array)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,				
				STR_ERR("can't read array of stage %d", 
					stage_index));
			return cJSON_False;	
		}

		cJSON *item =
			cJSON_GetArrayItem(array, item_index);
		if (item){
			return cJSON_ReplaceItemInObject(
					item, "kod", 
					cJSON_CreateString(kod));
		}
	}
	return cJSON_False;
}

static cJSON_bool
prozubi_planlecheniya_set_item_price(
		prozubi_t *kdata,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		int price
		)
{
	if (!kdata)
		return 0;

	if (!cJSON_IsArray(planlecheniya)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR(
				"%s", "can't read planlecheniya json"));
		return cJSON_False;
	}

	cJSON *stage =
		cJSON_GetArrayItem(
				planlecheniya, stage_index);
	if (stage){
		cJSON *array = 
			cJSON_GetObjectItem(stage, "array");
		if (!cJSON_IsArray(array)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,				
				STR_ERR("can't read array of stage %d", 
					stage_index));
			return cJSON_False;	
		}

		cJSON *item =
			cJSON_GetArrayItem(array, item_index);
		if (item){
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
	}
	return cJSON_False;
}

static cJSON_bool
prozubi_planlecheniya_set_item_count(
		prozubi_t *kdata,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		int count
		)
{
	if (!kdata)
		return 0;

	if (!cJSON_IsArray(planlecheniya)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR(
				"%s", "can't read planlecheniya json"));
		return cJSON_False;
	}

	cJSON *stage =
		cJSON_GetArrayItem(
				planlecheniya, stage_index);
	if (stage){
		cJSON *array = 
			cJSON_GetObjectItem(stage, "array");
		if (!cJSON_IsArray(array)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,				
				STR_ERR("can't read array of stage %d", 
					stage_index));
			return cJSON_False;	
		}

		cJSON *item =
			cJSON_GetArrayItem(array, item_index);
		if (item){
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
	}
	return cJSON_False;
}

static cJSON_bool
prozubi_planlecheniya_set_stage_duration(
		prozubi_t *kdata,
		cJSON *planlecheniya,
		int stage_index,
		int duration
		)
{
	if (!kdata)
		return 0;

	if (!cJSON_IsArray(planlecheniya)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR(
				"%s", "can't read planlecheniya json"));
		return cJSON_False;
	}

	cJSON *stage =
		cJSON_GetArrayItem(
				planlecheniya, stage_index);
		if (stage){
			char duration_str[32];
			sprintf(duration_str, "%d", duration);
			if (!cJSON_ReplaceItemInObject(stage, 
						"time", 
						cJSON_CreateString(
							duration_str)))
				return cJSON_False;
			return cJSON_True;
		}
	return cJSON_False;
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
	if (str_init(&s, BUFSIZ)){
		if(p->on_error)
			p->on_error(p->on_error_data, 
					STR("can't allocate memory"));
		return 0;
	}

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
	if (!cJSON_IsArray(planlecheniya)){
		if (p->on_log)
			p->on_log(p->on_log_data,		
		STR_ERR("%s", "no planlecheniya json"));
		return -1;	
	}

	int total_price = 0;
	int stage_i = 0;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage)){
			if (p->on_error)
				p->on_error(p->on_error_data,			
			STR_ERR(
				"can't read planlecheniya stage %d", 
				stage_i));
			continue;	
		}
		cJSON *array = 
			cJSON_GetObjectItem(stage, "array");
		if (!cJSON_IsArray(array)){
			if (p->on_error)
				p->on_error(p->on_error_data,			
			STR_ERR(
				"can't read planlecheniya array of"
				" stage: %d", stage_i));
			continue;	
		}		
		
		int stage_price = 0;
		int item_i = 0;
		cJSON *item;
		cJSON_ArrayForEach(item, array){
			if (!cJSON_IsObject(item)){
				if (p->on_error)
					p->on_error(p->on_error_data,				
					STR_ERR(
						"can't read planlecheniya item %d"
						" of stage %d", 
						item_i, stage_i));
				continue;	
			}

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
	if (!cJSON_IsArray(planlecheniya)){
		if (p->on_log)
			p->on_log(p->on_log_data,		
		STR_ERR("%s", "no planlecheniya json"));
		return -1;	
	}

	int total_duration = 0;
	int stage_i = 0;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage)){
			if (p->on_error)
				p->on_error(p->on_error_data,			
			STR_ERR(
				"can't read planlecheniya stage %d", 
				stage_i));
			continue;	
		}
		
		cJSON *time = 
			cJSON_GetObjectItem(stage, "time");
		if (!cJSON_IsString(time)){
			if (p->on_error)
				p->on_error(p->on_error_data,			
			STR_ERR(
				"can't read planlecheniya time of stage %d", 
				stage_i));
			continue;	
		}		
		char * duration_str = cJSON_GetStringValue(time);
		if (!duration_str) duration_str = (char*)"";
		int duration = atoi(duration_str);
		total_duration += duration;

		stage_i++;
	}
	return total_duration;
}

#endif /* ifndef PLANLECHENIYA_H */
