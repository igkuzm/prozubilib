/**
 * File              : prozubilib.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 17.04.2023
 * Last Modified Date: 02.02.2026
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../prozubilib.h"
#include "../include/thread.h"
#include "../include/yclients.h"

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
	kdata2_t *kdata;

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
	prozubi_cases_table_init(&zcases); 
	prozubi_doctors_table_init(&zdoctors);
	prozubi_images_table_init(&zimages);
	prozubi_passport_table_init(&zpassport);
	prozubi_prices_table_init(&zprices);
	prozubi_templates_table_init(&ztemplates);

	/* init kdata */ 
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
	if (p->db == NULL) {
		ON_ERR(p,  "SQLite database is NULL");
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
	//if (p->tid)
		//pthread_join(p->tid, NULL);
	
	_yd_daemon_init(p);	
	return 0;
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
}
