/**
 * File              : planlecheniya.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.04.2023
 * Last Modified Date: 07.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PLANLECHENIYA_H
#define PLANLECHENIYA_H

#include <stdio.h>
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
			int price,
			int count,
			int total
			)
		)
{
	if (!cJSON_IsArray(planlecheniya)){
		ERR("%s", "can't read planlecheniya json");
		return;	
	}

	int total_price = 0;
	int total_duration = 0;
	int stage_i = 0;
	cJSON *stage;
	cJSON_ArrayForEach(stage, planlecheniya){
		if (!cJSON_IsObject(stage)){
			ERR("can't read planlecheniya stage %d", stage_i);
			continue;	
		}
		
		cJSON *time = cJSON_GetObjectItem(stage, "time");
		if (!cJSON_IsString(time)){
			ERR("can't read planlecheniya time of stage %d", stage_i);
			continue;	
		}		
		char * duration_str = cJSON_GetStringValue(time);
		if (!duration_str) duration_str = "";
		int duration = atoi(duration_str);
		total_duration += duration;

		/* callback stage */
		void *stage_ptr = callback(
				userdata, NULL, stage, PLANLECHENIYA_TYPE_STAGE,
				stage_i, STR("Этап №%d", stage_i + 1), "", 0, 0, 0);
		
		cJSON *array = cJSON_GetObjectItem(stage, "array");
		if (!cJSON_IsArray(array)){
			ERR("can't read planlecheniya array of stage %d", stage_i);
			continue;	
		}		
		
		int stage_price = 0;
		int item_i = 0;
		cJSON *item;
		cJSON_ArrayForEach(item, array){
			if (!cJSON_IsObject(item)){
				ERR("can't read planlecheniya item %d of stage %d", item_i, stage_i);
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

			/* callback item */
			callback(userdata, stage_ptr, item, PLANLECHENIYA_TYPE_ITEM,
					item_i, title, kod, 
					price, count, total);
			
			stage_price += total; 
			total_price += total;

			item_i++;
		}

		/* callback stage price and duration */
		callback(userdata, stage_ptr, item, PLANLECHENIYA_TYPE_STAGE_PRICE,
				item_i, STR("Итого за %d этап:", stage_i + 1), "", 
				0, 0, stage_price);

		callback(userdata, stage_ptr, item, PLANLECHENIYA_TYPE_STAGE_DURATION,
				item_i, "Продолжительность этапа (мес.):", "", 
				0, duration, 0);
		

		stage_i++;
	}

	/* callback total price and duration */
	callback(userdata, NULL, planlecheniya, PLANLECHENIYA_TYPE_TOTAL_PRICE,
			0, "Общая стоимость по плану:", "", 
			0, 0, total_price);

	callback(userdata, NULL, planlecheniya, PLANLECHENIYA_TYPE_TOTAL_DURATION,
			0, "Общая продолжительность лечения (мес.):", "", 
			0, total_duration, 0);
	
}


#endif /* ifndef PLANLECHENIYA_H */

