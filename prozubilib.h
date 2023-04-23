/**
 * File              : prozubilib.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 23.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PROZUBILIB_H
#define PROZUBILIB_H

#include <time.h>
#include <stdint.h>

#include "prozubilib_conf.h"

#include "passport.h"
#include "cases.h"
#include "doctors.h"
#include "template.h"
#include "images.h"
#include "prices.h"

typedef kdata2_t prozubi_t;

/* init prozubi database and start yandex disk sync */
static prozubi_t *
prozubi_init(
		const char *filepath, // path to store SQLite database
		const char *token     // yandex disk token
		);

/* set Yandex Disk token */
static int
prozubi_set_token(
		prozubi_t *p,
		const char *token
		);

/* start Yandex Disk sync */
static int
prozubi_start_sync(
		prozubi_t *p
		);

/* stop Yandex Disk sync */
static int
prozubi_stop_sync(
		prozubi_t *p
		);


#endif /* ifndef PROZUBILIB_H */