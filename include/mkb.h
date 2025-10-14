/**
 * File              : mkb.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 11.05.2023
 * Last Modified Date: 13.09.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PROZUBILIB_MKB_H
#define PROZUBILIB_MKB_H

#include "prozubilib_conf.h"

#define MKB_BASE "MKBCodes.sqlite"
#define MKB_TABL "kodiMKB"

#define MKB_COLUMNS \
	MKB_COLUMN_INT(iD) \
	MKB_COLUMN_INT(parent) \
	MKB_COLUMN_TEX(kod) \
	MKB_COLUMN_TEX(name) \

typedef struct MKB_t {
	#define MKB_COLUMN_INT(title) int    title;
	#define MKB_COLUMN_TEX(title) char * title;
	MKB_COLUMNS
	#undef MKB_COLUMN_INT
	#undef MKB_COLUMN_TEX
} mkb_t;

BEGIN_ENUM(MKB)
	#define MKB_COLUMN_INT(title) MKB_##title,
	#define MKB_COLUMN_TEX(title) MKB_##title,
	MKB_COLUMNS
	#undef MKB_COLUMN_INT
	#undef MKB_COLUMN_TEX
	MKB_COLS_NUM
END_ENUM(MKB)

BEGIN_ENUM_STRING(MKB)
	#define MKB_COLUMN_INT(title) "MKB_" #title,
	#define MKB_COLUMN_TEX(title) "MKB_" #title,
	MKB_COLUMNS
	#undef MKB_COLUMN_INT
	#undef MKB_COLUMN_TEX
END_ENUM_STRING(MKB)

extern void EXPORTDLL
prozubi_mkb_foreach(
		prozubi_t *p,
		const char *predicate,
		void * user_data,
		void * (*callback)(
			void * user_data,
			void * parent,
			mkb_t *ptr
			)
		);


extern void EXPORTDLL
prozubi_mkb_free(mkb_t *c);

#endif // PROZUBILIB_MKB_H	
