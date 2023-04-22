/**
 * File              : prozubilib.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 21.04.2023
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

static kdata2_t *
prozubilib_init(
		const char *filepath,
		const char *token
		);


#endif /* ifndef PROZUBILIB_H */
