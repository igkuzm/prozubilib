/**
 * File              : planlecheniya.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.04.2023
 * Last Modified Date: 25.04.2026
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

extern void EXPORTDLL
prozubi_planlecheniya_free(struct planlecheniya_t *t);

extern void EXPORTDLL
prozubi_planlecheniya_foreach(
		kdata2_t *p,
		cJSON *planlecheniya,
		void * userdata,
		void * (*callback)(
			void *userdata,
			void *parent,
			struct planlecheniya_t *t)
		);

extern cJSON EXPORTDLL *
prozubi_planlecheniya_add_stage(
		kdata2_t *p,
		cJSON *planlecheniya
		);

extern void EXPORTDLL
prozubi_planlecheniya_remove_stage(
		kdata2_t *p,
		cJSON *planlecheniya,
		int stage_index
		);

extern cJSON EXPORTDLL *
_planlecheniya_get_stage(
		kdata2_t *p,
		cJSON *planlecheniya,
		int stage_index);

extern cJSON EXPORTDLL *
prozubi_planlecheniya_insert_item(
		kdata2_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		const char *title,
		const char *kod,
		int price,
		int count
		);

extern cJSON EXPORTDLL *
prozubi_planlecheniya_add_item(
		kdata2_t *p,
		cJSON *planlecheniya,
		int stage_index,
		const char *title,
		const char *kod,
		int price,
		int count
		);

extern void EXPORTDLL
prozubi_planlecheniya_remove_item(
		kdata2_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index
		);

extern cJSON_bool EXPORTDLL
prozubi_planlecheniya_set_item_title(
		kdata2_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		const char *title
		);

extern cJSON_bool EXPORTDLL
prozubi_planlecheniya_set_item_kod(
		kdata2_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		const char *kod
		);

extern cJSON_bool EXPORTDLL
prozubi_planlecheniya_set_item_price(
		kdata2_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		int price
		);

extern cJSON_bool EXPORTDLL
prozubi_planlecheniya_set_item_count(
		kdata2_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int item_index,
		int count
		);

extern cJSON_bool EXPORTDLL
prozubi_planlecheniya_set_stage_duration(
		kdata2_t *p,
		cJSON *planlecheniya,
		int stage_index,
		int duration
		);

extern size_t EXPORTDLL prozubi_planlecheniya_to_rtf(
		kdata2_t *p, cJSON *planlecheniya, char **rtf);

extern int EXPORTDLL prozubi_planlecheniya_get_price_total(
		kdata2_t *p, cJSON *planlecheniya);

extern int EXPORTDLL prozubi_planlecheniya_get_duration_total(
		kdata2_t *p, cJSON *planlecheniya);

extern int EXPORTDLL
prozubi_planlecheniya_zformula_image(
		kdata2_t *p, struct case_t *c, 
		const char *imagepath, 
		void **data, size_t *len);
#endif /* ifndef PLANLECHENIYA_H */
