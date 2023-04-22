/**
 * File              : prozubilib_conf.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 17.04.2023
 * Last Modified Date: 21.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef UUIDCOLUMN
#define UUIDCOLUMN "ZRECORDNAME"
#endif /* ifndef UUIDCOLUMN */

#include "log.h"
#include "kdata2/kdata2.h"
#include "kdata2/sqlite3.h"
#include "kdata2/cYandexDisk/cJSON.h"
#include "kdata2/cYandexDisk/uuid4.h"

#ifndef UUID4_STR
#define UUID4_STR
typedef char uuid4_str[37];
#endif /* ifndef UUID4_STR */

#ifndef NSTimeIntervalSince1970
#define NSTimeIntervalSince1970  978307200.0
#endif /* ifndef NSTimeIntervalSince1970 */

