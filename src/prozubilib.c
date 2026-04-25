/**
 * File              : prozubilib.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 17.04.2023
 * Last Modified Date: 25.04.2026
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../prozubilib.h"
#include "../include/thread.h"
#include "../include/yclients.h"
#include "../kdata2/modules/yandexdisk/yandexdisk.h"
#include <stdlib.h>
#include <string.h>

static const CYCUser *user;
static const CYC2fa  *user2fa;

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
	struct kdata2_table *zcases;	
	struct kdata2_table *zdoctors;	
	struct kdata2_table *zimages;	
	struct kdata2_table *zpassport;	
	struct kdata2_table *zprices;	
	struct kdata2_table *ztemplates;	
	kdata2_t *kdata = NULL;
	prozubi_t *prozubi = NULL;

	if (on_log)
		on_log(on_log_data, STR("%s", "init..."));
	
	/* check filepath */
	if (!filepath) {
		if (on_error)
			on_error(on_error_data, STR("%s", "filepath is NULL"));
		return NULL;
	}

	/* check token */
	if (!token)
		if (on_log)
			on_log(on_log_data, STR("%s", "token is NULL"));

	/* init tables */
	prozubi_cases_table_init(&zcases); 
	prozubi_doctors_table_init(&zdoctors);
	prozubi_images_table_init(&zimages);
	prozubi_passport_table_init(&zpassport);
	prozubi_prices_table_init(&zprices);
	prozubi_templates_table_init(&ztemplates);

	/* init kdata */ 
	if (kdata2_init(&kdata, filepath, 
		   on_error_data, on_error, on_log_data, on_log,	
			zcases, zdoctors, zimages, zpassport, zprices, ztemplates,
			NULL))
	  return NULL;

	prozubi = realloc(kdata, sizeof(prozubi_t));
	if (prozubi)
		prozubi->ydm = NULL;

	return prozubi;
}

int _prozubi_check_lib(prozubi_t *p){
	/* check prozubi */
	kdata2_t *kdata2 = (kdata2_t *)p;
	if (!p) {
		return -1;
	}
	/* check SQLite database */
	if (p->kdata2.db == NULL) {
		ON_ERR(kdata2,  "SQLite database is NULL");
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
	kdata2_t *kdata2 = (kdata2_t *)p;
	/* check prozubi */
	if (_prozubi_check_lib(p))
		return -1;
	
	/* check token */
	if (!token){
		ON_ERR(kdata2, "token is NULL");
		return -1;
	}

	return yandex_disk_set_token(p->ydm, token);
}

int
prozubi_stop_yandex_disk_sync(
		prozubi_t *p
		)
{
	/* check prozubi */
	if (_prozubi_check_lib(p))
		return -1;
	
	yandex_disk_module_unload(p->ydm);
	
	return 0;
}

int
prozubi_start_yandex_disk_sync(
		prozubi_t *p,
		const char *token,
		void *progressp,
		int (*progress)(void *progressp, pphase, int current, int total)
		)
{
	/* check prozubi */
	if (_prozubi_check_lib(p))
		return 1;

	p->ydm = yandex_disk_module_load(
			&p->kdata2, 
			token, 
			progressp, progress);

	return p->ydm == NULL;
}

int 
prozubi_yclients_sync(
		prozubi_t *p,
		const char *login,
		const char *password,
		void *on_code_data,
		char *on_code(void *on_code_data)
		)
{
	/*
	CYCLIENTS_AUTH auth = CYCLIENTS_AUTH_ERROR;
	struct yclients *t = NULL;

	auth = cyclients_login(login, password,
		 	&user, &user2fa);
	
	if (auth == CYCLIENTS_AUTH_2FA){
		// login with 2fa
		if (user2fa == NULL){
			if (p->on_error)
				p->on_error(p->on_error_data, "yclients: Unknown ERROR!");
			return 1;
		}

		if (user2fa->attempts_left < 1){
			if (p->on_error)
				p->on_error(p->on_error_data, "yclients: You have no attempts left");
			return 1;
		}

		while (user2fa->attempts_left > 0 && 
				auth != CYCLIENTS_AUTH_AUTHORIZED)
		{
			char *secret = NULL; 
			auth = CYCLIENTS_AUTH_ERROR;
			if (p->on_log)
				p->on_log(p->on_log_data, 
						STR("yclients: Please enter code, "
							"that was send you by %s: %s",
				user2fa->transport.type, user2fa->transport.recipient));
			if (on_code)
				secret = on_code(on_code_data);
			else
				break;

			if (secret){
				auth = cyclients_login_2fa(
						login, password, 
						user2fa->uuid,
						secret, &user);
			}
		}
	}
		
	if (auth == CYCLIENTS_AUTH_ERROR) {
			if (p->on_error)
				p->on_error(p->on_error_data, 
						"yclients: Can't login to Yclients");
		return 1;
	}

	if (user == NULL){
		if (p->on_error)
			p->on_error(p->on_error_data, "yclients: Unknown ERROR!");
		return 1;
	}

	if (p->on_log)
			p->on_log(p->on_log_data, 
					STR("yclients: Authorized as: %s\n", user->name));

	if ((t = NEW(struct yclients)))
	{
		t->p = p;
		t->user = user;
		if (run_in_thread(t, yclients_main_loop))
			return 0;
	}

	return 1;
	*/
}
