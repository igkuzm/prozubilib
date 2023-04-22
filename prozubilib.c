/**
 * File              : prozubilib.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 17.04.2023
 * Last Modified Date: 21.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "prozubilib.h"

kdata2_t *
prozubilib_init(
		const char *filepath,
		const char *token
		)
{
	/* check filepath */
	if (!filepath) {
		ERR("%s", "filepath is NULL");
		return NULL;
	}

	/* check token */
	if (!filepath)
		LOG("%s", "token is NULL");

	/* init tables */
	struct kdata2_table *zcases;	
	prozubi_cases_table_init(&zcases); 

	struct kdata2_table *zdoctors;	
	prozubi_doctors_table_init(&zdoctors);
	
	struct kdata2_table *zimages;	
	prozubi_images_table_init(&zimages);

	struct kdata2_table *zpassport;	
	prozubi_passport_table_init(&zpassport);

	struct kdata2_table *zprices;	
	prozubi_prices_table_init(&zprices);

	struct kdata2_table *ztemplates;	
	prozubi_templates_table_init(&ztemplates);

	/* init kdata */ 
	kdata2_t *kdata;
	kdata2_init(&kdata, filepath, token, 60,
			zcases, zdoctors, zimages, zpassport, zprices, ztemplates,
			NULL);

	return kdata;
}

