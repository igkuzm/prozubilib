/**
 * File              : planlecheniya.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.04.2023
 * Last Modified Date: 09.10.2025
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PLANLECHENIYA_H
#define PLANLECHENIYA_H

#include "prozubilib_conf.h"
#include "cases.h"

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

extern void
prozubi_planlecheniya_free(struct planlecheniya_t *t);

extern void
prozubi_planlecheniya_foreach(
		prozubi_t *p,
		cJSON *planlecheniya,
		void * userdata,
		void * (*callback)(
			void *userdata,
			void *parent,
			struct planlecheniya_t *t)
		);

extern cJSON *
prozubi_planlecheniya_add_stage(
		prozubi_t *p,
		cJSON *planlecheniya
		);

extern void
prozubi_planlecheniya_remove_stage(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index
		);

extern cJSON *
_planlecheniya_get_stage(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index);

extern cJSON *
prozubi_planlecheniya_insert_item(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		const char *title,
		const char *kod,
		int price,
		int count
		);

extern cJSON *
prozubi_planlecheniya_add_item(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		const char *title,
		const char *kod,
		int price,
		int count
		);

extern void
prozubi_planlecheniya_remove_item(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index
		);

extern cJSON_bool
prozubi_planlecheniya_set_item_title(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		const char *title
		);

extern cJSON_bool
prozubi_planlecheniya_set_item_kod(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		const char *kod
		);

extern cJSON_bool
prozubi_planlecheniya_set_item_price(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		int price
		);

extern cJSON_bool
prozubi_planlecheniya_set_item_count(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		int count
		);

extern cJSON_bool
prozubi_planlecheniya_set_stage_duration(
		prozubi_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int duration
		);

extern size_t prozubi_planlecheniya_to_rtf(
		prozubi_t *p, cJSON *planlecheniya, char **rtf);

extern int prozubi_planlecheniya_get_price_total(
		prozubi_t *p, cJSON *planlecheniya);

extern int prozubi_planlecheniya_get_duration_total(
		prozubi_t *p, cJSON *planlecheniya);

extern int
prozubi_planlecheniya_zformula_image(
		prozubi_t *p, struct case_t *c, 
		const char *imagepath, 
		void **data, size_t *len);
#endif /* ifndef PLANLECHENIYA_H */
