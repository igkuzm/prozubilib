/**
 * File              : planlecheniya.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.04.2023
 * Last Modified Date: 25.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PLANLECHENIYA_H
#define PLANLECHENIYA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kdata2/kdata2.h"
#include "log.h"
#include "kdata2/cYandexDisk/cJSON.h"

typedef enum PLANLECHENIYA_TYPE {
	PLANLECHENIYA_TYPE_STAGE,
	PLANLECHENIYA_TYPE_ITEM,
	PLANLECHENIYA_TYPE_STAGE_PRICE,
	PLANLECHENIYA_TYPE_STAGE_DURATION,
	PLANLECHENIYA_TYPE_TOTAL_PRICE,
	PLANLECHENIYA_TYPE_TOTAL_DURATION,
	
} PLANLECHENIYA_TYPE;


static void
prozubi_planlecheniya_foreach(
		kdata2_t *kdata,
		cJSON *planlecheniya,
		void * userdata,
		void * (*callback)(
			void *userdata,
			void *parent,
			cJSON *json,
			PLANLECHENIYA_TYPE type,
			int index,
			char * title,
			char * kod,
			char * price,
			char * count,
			char * total
			)
		)
{
	if (!kdata)
		return NULL;

	if (!cJSON_IsArray(planlecheniya)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
		STR_ERR("%s", "can't read planlecheniya json"));
		return;	
	}

	int total_price = 0;
	int total_duration = 0;
	int stage_i = 0;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
			STR_ERR("can't read planlecheniya stage %d", stage_i));
			continue;	
		}
		
		cJSON *time = cJSON_GetObjectItem(stage, "time");
		if (!cJSON_IsString(time)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
			STR_ERR("can't read planlecheniya time of stage %d", stage_i));
			continue;	
		}		
		char * duration_str = cJSON_GetStringValue(time);
		if (!duration_str) duration_str = "";
		int duration = atoi(duration_str);
		total_duration += duration;

		/* callback stage */
		char stage_str[64];
		sprintf(stage_str, "Этап №%d", stage_i + 1);
		cJSON *stage_to_return = stage;
		void *stage_ptr = callback(
				userdata, NULL, stage_to_return, PLANLECHENIYA_TYPE_STAGE,
				stage_i, stage_str, "", 0, 0, 0);
		
		cJSON *array = cJSON_GetObjectItem(stage, "array");
		if (!cJSON_IsArray(array)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
			STR_ERR("can't read planlecheniya array of stage %d", stage_i));
			continue;	
		}		
		
		int stage_price = 0;
		int item_i = 0;
		cJSON *item;
		cJSON_ArrayForEach(item, array){
			if (!cJSON_IsObject(item)){
				if (kdata->on_error)
					kdata->on_error(kdata->on_error_data,				
				STR_ERR("can't read planlecheniya item %d of stage %d", item_i, stage_i));
				continue;	
			}

			char *kod    =     cJSON_GetStringValue(
							   cJSON_GetObjectItem(item, "kod"));
			if (!kod) kod = "";

			char *title  =     cJSON_GetStringValue(
							   cJSON_GetObjectItem(item, "title"));
			if (!title) title = "";

			char *price_str   =cJSON_GetStringValue(
							   cJSON_GetObjectItem(item, "price"));
			if (!price_str) price_str = "";

			char *count_str   =cJSON_GetStringValue(
							   cJSON_GetObjectItem(item, "count"));
			if (!count_str) count_str = "";

			int price = atoi(price_str);
			int count = atoi(count_str);
			int total = price * count;
			char total_str[32];
			sprintf(total_str, "%d", total);

			/* callback item */
			cJSON *item_to_return = item;
			callback(userdata, stage_ptr, item_to_return, PLANLECHENIYA_TYPE_ITEM,
					item_i, title, kod, 
					price_str, count_str, total_str);
			
			stage_price += total; 
			total_price += total;

			item_i++;
		}

		/* callback stage price and duration */
		char stage_price_title[64];
		sprintf(stage_price_title, "Итого за %d этап:", stage_i + 1);
		char stage_price_str[32];
		sprintf(stage_price_str, "%d", stage_price);
		callback(userdata, stage_ptr, item, PLANLECHENIYA_TYPE_STAGE_PRICE,
				item_i, stage_price_title, "", 
				0, 0, stage_price_str);

		char stage_duration_str[32];
		sprintf(stage_duration_str, "%d", duration);
		callback(userdata, stage_ptr, item, PLANLECHENIYA_TYPE_STAGE_DURATION,
				item_i, "Продолжительность этапа (мес.):", "", 
				0, stage_duration_str, 0);
		

		stage_i++;
	}

	/* callback total price and duration */
	char total_price_str[32];
	sprintf(total_price_str, "%d", total_price);
	callback(userdata, NULL, planlecheniya, PLANLECHENIYA_TYPE_TOTAL_PRICE,
			0, "Общая стоимость по плану:", "", 
			0, 0, total_price_str);

	char total_duration_str[32];
	sprintf(total_duration_str, "%d", total_duration);
	callback(userdata, NULL, planlecheniya, PLANLECHENIYA_TYPE_TOTAL_DURATION,
			0, "Общая продолжительность лечения (мес.):", "", 
			0, total_duration_str, 0);
	
}

static cJSON *
prozubi_planlecheniya_add_stage(
		kdata2_t *kdata,
		cJSON *planlecheniya
		)
{
	if (!kdata)
		return NULL;

	if (!cJSON_IsArray(planlecheniya)){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("%s", "can't read planlecheniya json"));
		return NULL;	
	}

	/* add new stage */
	cJSON *stage = cJSON_CreateObject();
	cJSON_AddItemToObject(stage, "time", cJSON_CreateString("3"));
	cJSON *array = cJSON_CreateArray();
	cJSON_AddItemToObject(stage, "array", array);

	cJSON_AddItemToArray(planlecheniya, stage);

	return stage;
}

static cJSON *
prozubi_planlecheniya_add_item(
		kdata2_t *kdata,
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
			STR_ERR("%s", "can't read planlecheniya json"));
		return NULL;	
	}

	int i;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
			STR_ERR("can't read planlecheniya stage %d", i));
			continue;	
		}
		if (stage_index == i){
			cJSON *array = cJSON_GetObjectItem(stage, "array");
			if (!cJSON_IsArray(array)){
				if (kdata->on_error)
					kdata->on_error(kdata->on_error_data,				
					STR_ERR("can't read array of stage %d", i));
				return NULL;	
			}
			int index = cJSON_GetArraySize(array);

			cJSON *item = cJSON_CreateObject();

			char number[32];
			sprintf(number, "%d", index+1);
			cJSON_AddItemToObject(item, "number", cJSON_CreateString(number));
			cJSON_AddItemToObject(item, "isItem", cJSON_CreateString("1"));
			cJSON_AddItemToObject(item, "kod", cJSON_CreateString(kod));
			cJSON_AddItemToObject(item, "title", cJSON_CreateString(title));
			char price_str[32];
			sprintf(price_str, "%d", price);
			cJSON_AddItemToObject(item, "price", cJSON_CreateString(price_str));
			char count_str[32];
			sprintf(count_str, "%d", count);
			cJSON_AddItemToObject(item, "count", cJSON_CreateString(count_str));
			char child_str[32];
			sprintf(child_str, "%d", index);
			cJSON_AddItemToObject(item, "childIndex", cJSON_CreateString(child_str));
			char total_str[32];
			sprintf(total_str, "%d", price * count);
			cJSON_AddItemToObject(item, "total", cJSON_CreateString(total_str));
			char parent_str[32];
			sprintf(parent_str, "%d", i);
			cJSON_AddItemToObject(item, "parentIndex", cJSON_CreateString(parent_str));

			cJSON_AddItemToArray(array, item);

			return item;
			}		
	}
	return NULL;
}

static cJSON_bool
prozubi_planlecheniya_set_item_title(
		kdata2_t *kdata,
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

	int i;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
			STR_ERR("can't read planlecheniya stage %d", i));
			continue;	
		}
		if (stage_index == i){
			cJSON *array = cJSON_GetObjectItem(stage, "array");
			if (!cJSON_IsArray(array)){
				if (kdata->on_error)
					kdata->on_error(kdata->on_error_data,				
					STR_ERR("can't read array of stage %d", i));
				return cJSON_False;	
			}

			i = 0;
			cJSON *item;
			cJSON_ArrayForEach(item, array){
				if (!cJSON_IsObject(item)){
					if (kdata->on_error)
						kdata->on_error(kdata->on_error_data,					
						STR_ERR("can't read item %d", i));
					continue;	
				}
				if (item_index == i){
					return 
						cJSON_ReplaceItemInObject(item, "title", cJSON_CreateString(title));
				}
			}
		}
	}
	return cJSON_False;
}

static cJSON_bool
prozubi_planlecheniya_set_item_kod(
		kdata2_t *kdata,
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
			STR_ERR("%s", "can't read planlecheniya json"));
		return cJSON_False;
	}

	int i;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
				STR_ERR("can't read planlecheniya stage %d", i));
			continue;	
		}
		if (stage_index == i){
			cJSON *array = cJSON_GetObjectItem(stage, "array");
			if (!cJSON_IsArray(array)){
				if (kdata->on_error)
					kdata->on_error(kdata->on_error_data,				
					STR_ERR("can't read array of stage %d", i));
				return -1;	
			}

			i = 0;
			cJSON *item;
			cJSON_ArrayForEach(item, array){
				if (!cJSON_IsObject(item)){
					if (kdata->on_error)
						kdata->on_error(kdata->on_error_data,					
						STR_ERR("can't read item %d", i));
					continue;	
				}
				if (item_index == i){
					return 
						cJSON_ReplaceItemInObject(item, "kod", cJSON_CreateString(kod));
				}
			}
		}
	}
	return cJSON_False;
}

static cJSON_bool
prozubi_planlecheniya_set_item_price(
		kdata2_t *kdata,
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
			STR_ERR("%s", "can't read planlecheniya json"));
		return cJSON_False;
	}

	int i;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
				STR_ERR("can't read planlecheniya stage %d", i));
			continue;	
		}
		if (stage_index == i){

			cJSON *array = cJSON_GetObjectItem(stage, "array");
			if (!cJSON_IsArray(array)){
				if (kdata->on_error)
					kdata->on_error(kdata->on_error_data,				
					STR_ERR("can't read array of stage %d", i));
				return cJSON_False;	
			}

			i = 0;
			cJSON *item;
			cJSON_ArrayForEach(item, array){
				if (!cJSON_IsObject(item)){
					if (kdata->on_error)
						kdata->on_error(kdata->on_error_data,					
						STR_ERR("can't read item %d", i));
					continue;	
				}
				if (item_index == i){
					char *count;
					cJSON *count_j =  cJSON_GetObjectItem(item, "count"); 
					if (count_j)
						count = cJSON_GetStringValue(count_j);
					if (!count)
						count = "";

					char price_str[32];
					sprintf(price_str, "%d", price);
					if (!cJSON_ReplaceItemInObject(item, "price", cJSON_CreateString(price_str)))
						return cJSON_False;

					char total[32];
					sprintf(total, "%d", price * atoi(count));
					if (!cJSON_ReplaceItemInObject(item, "total", cJSON_CreateString(total)))
						return cJSON_False;
					
					return cJSON_True;
				}
			}
		}
	}
	return cJSON_False;
}

static cJSON_bool
prozubi_planlecheniya_set_item_count(
		kdata2_t *kdata,
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
			STR_ERR("%s", "can't read planlecheniya json"));
		return cJSON_False;
	}

	int i;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
				STR_ERR("can't read planlecheniya stage %d", i));
			continue;	
		}
		if (stage_index == i){

			cJSON *array = cJSON_GetObjectItem(stage, "array");
			if (!cJSON_IsArray(array)){
				if (kdata->on_error)
					kdata->on_error(kdata->on_error_data,				
					STR_ERR("can't read array of stage %d", i));
				return cJSON_False;	
			}

			i = 0;
			cJSON *item;
			cJSON_ArrayForEach(item, array){
				if (!cJSON_IsObject(item)){
					if (kdata->on_error)
						kdata->on_error(kdata->on_error_data,					
						STR_ERR("can't read item %d", i));
					continue;	
				}
				if (item_index == i){
					char *price;
					cJSON *price_j =  cJSON_GetObjectItem(item, "price"); 
					if (price_j)
						price = cJSON_GetStringValue(price_j);
					if (!price)
						price = "";

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
			}
		}
	}
	return cJSON_False;
}

#endif /* ifndef PLANLECHENIYA_H */
