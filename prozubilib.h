/**
 * File              : prozubilib.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 30.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PROZUBILIB_H
#define PROZUBILIB_H

#include <time.h>

#include "include/prozubilib_conf.h"

#include "include/bill.h"
#include "include/cases.h"
#include "include/diagnosis.h"
#include "include/doctors.h"
#include "include/documents.h"
#include "include/images.h"
#include "include/mkb.h"
#include "include/nomenklatura.h"
#include "include/passport.h"
#include "include/planlecheniya.h"
#include "include/prices.h"
#include "include/template.h"
#include "include/rtf.h"

/* init prozubi database and start yandex disk sync */
prozubi_t *
prozubi_init(
		const char *filepath, // path to store SQLite database
		const char *token,   // yandex disk token
		void       *on_error_data,
		void      (*on_error)      (void *on_error_data, const char *error),
		void       *on_log_data,
		void      (*on_log)        (void *on_log_data, const char *message)
		);

/* set Yandex Disk token */
int
prozubi_set_token(
		prozubi_t *p,
		const char *token
		);

/* start Yandex Disk sync */
int
prozubi_start_sync(
		prozubi_t *p
		);

/* stop Yandex Disk sync */
int
prozubi_stop_sync(
		prozubi_t *p
		);

#endif /* ifndef PROZUBILIB_H */
