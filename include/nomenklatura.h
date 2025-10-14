/**
 * File              : nomenklatura.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.05.2023
 * Last Modified Date: 13.09.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef NOMENKLATURA_H
#define NOMENKLATURA_H

#include "prozubilib_conf.h"

#define NOMENKLATURA_BASE "nomenklatura.sqlite"
#define NOMENKLATURA_TABL "nomenklatura"

#define NOMENKLATURA_COLUMNS \
	NOMENKLATURA_COLUMN_INT(iD) \
	NOMENKLATURA_COLUMN_INT(parent) \
	NOMENKLATURA_COLUMN_TEX(kod) \
	NOMENKLATURA_COLUMN_TEX(name) \
	NOMENKLATURA_COLUMN_TEX(headName)

typedef struct NOMENKLATURA_t {
	#define NOMENKLATURA_COLUMN_INT(title) int    title;
	#define NOMENKLATURA_COLUMN_TEX(title) char * title;
	NOMENKLATURA_COLUMNS
	#undef NOMENKLATURA_COLUMN_INT
	#undef NOMENKLATURA_COLUMN_TEX
} nomenklatura_t;

BEGIN_ENUM(NOMENKLATURA)
	#define NOMENKLATURA_COLUMN_INT(title) NOMENKLATURA_##title,
	#define NOMENKLATURA_COLUMN_TEX(title) NOMENKLATURA_##title,
	NOMENKLATURA_COLUMNS
	#undef NOMENKLATURA_COLUMN_INT
	#undef NOMENKLATURA_COLUMN_TEX
	NOMENKLATURA_COLS_NUM
END_ENUM(NOMENKLATURA)

BEGIN_ENUM_STRING(NOMENKLATURA)
	#define NOMENKLATURA_COLUMN_INT(title) "NOMENKLATURA_" #title,
	#define NOMENKLATURA_COLUMN_TEX(title) "NOMENKLATURA_" #title,
	NOMENKLATURA_COLUMNS
	#undef NOMENKLATURA_COLUMN_INT
	#undef NOMENKLATURA_COLUMN_TEX
END_ENUM_STRING(NOMENKLATURA)

extern void EXPORTDLL
prozubi_nomenklatura_foreach(
		prozubi_t *p,
		const char *predicate,
		void * user_data,
		void * (*callback)(
			void * user_data,
			void * parent,
			nomenklatura_t *ptr
			)
		);

extern void EXPORTDLL
prozubi_nomenklatura_free(nomenklatura_t *c);

#endif // NOMENKLATURA_H	
