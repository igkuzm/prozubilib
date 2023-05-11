/**
 * File              : nomenklatura.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.05.2023
 * Last Modified Date: 11.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef NOMENKLATURA_H
#define NOMENKLATURA_H

#include "enum.h"
#include "kdata2/sqlite3.h"


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
	NOMENKLATURA_N
END_ENUM(NOMENKLATURA)

BEGIN_ENUM_STRING(NOMENKLATURA)
	#define NOMENKLATURA_COLUMN_INT(title) "NOMENKLATURA_" #title,
	#define NOMENKLATURA_COLUMN_TEX(title) "NOMENKLATURA_" #title,
	NOMENKLATURA_COLUMNS
	#undef NOMENKLATURA_COLUMN_INT
	#undef NOMENKLATURA_COLUMN_TEX
END_ENUM_STRING(NOMENKLATURA)

static void
prozubi_nomenklatura_foreach(
		void * user_data,
		int (*callback)(
			void * user_data,
			nomenklatura_t *p
			)
		)
{

}

#endif // NOMENKLATURA_H	
