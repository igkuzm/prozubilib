/**
 * File              : prozubilib.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 17.04.2023
 * Last Modified Date: 13.09.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "prozubilib.h"
#include "kdata2/kdata2.h"

prozubi_t *
prozubi_init(
		const char *filepath,
		const char *token,
		void       *on_error_data,
		void      (*on_error)      (void *on_error_data, const char *error),
		void       *on_log_data,
		void      (*on_log)        (void *on_log_data, const char *message)		
		)
{
	if (on_log)
		on_log(on_log_data, STR_ERR("%s", "init..."));
	
	/* check filepath */
	if (!filepath) {
		if (on_error)
			on_error(on_error_data, STR_ERR("%s", "filepath is NULL"));
		return NULL;
	}

	/* check token */
	if (!token)
		if (on_log)
			on_log(on_log_data, STR_ERR("%s", "token is NULL"));

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
	if (kdata2_init(&kdata, filepath, token,
		   on_error_data, on_error, on_log_data, on_log,	
			60,
			zcases, zdoctors, zimages, zpassport, zprices, ztemplates,
			NULL))
	  return NULL;

	return kdata;
}

int _prozubi_check_lib(prozubi_t *p){
	/* check prozubi */
	if (!p) {
		return -1;
	}
	/* check SQLite database */
	if (!p->db) {
		ON_ERR(p, "SQLite database is NULL");
		return -1;
	}	

	return 0;
}

int
prozubi_set_token(
		prozubi_t *p,
		const char *token
		)
{
	/* check prozubi */
	if (_prozubi_check_lib(p))
		return -1;
	
	/* check token */
	if (!token){
		ON_ERR(p, "token is NULL");
		return -1;
	}
	
	return kdata2_set_access_token(p, token);
}

int
prozubi_stop_sync(
		prozubi_t *p
		)
{
	/* check prozubi */
	if (_prozubi_check_lib(p))
		return -1;
	
	if (!p->tid){
		ON_ERR(p, "thread id is NULL");
		return -1;
	}
	
	p->do_update = false;
	return 0;
}

int
prozubi_start_sync(
		prozubi_t *p
		)
{
	/* check prozubi */
	if (_prozubi_check_lib(p))
		return -1;

	// try to join thread
	p->do_update = false;
	if (p->tid)
		pthread_join(p->tid, NULL);
	
	_yd_daemon_init(p);	
	return 0;
}
