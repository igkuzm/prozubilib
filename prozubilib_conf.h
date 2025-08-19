/**
 * File              : prozubilib_conf.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 17.04.2023
 * Last Modified Date: 13.09.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PROZUBILIB_CONF_H
#define PROZUBILIB_CONF_H


#ifndef UUIDCOLUMN
#define UUIDCOLUMN "ZRECORDNAME"
#endif /* ifndef UUIDCOLUMN */

#include "kdata2/kdata2.h"
#include "kdata2/sqlite3.h"
#include "kdata2/cYandexDisk/cJSON.h"
#include "kdata2/cYandexDisk/uuid4.h"
#include "kdata2/cYandexDisk/log.h"
#include "kdata2/cYandexDisk/log.h"

typedef kdata2_t prozubi_t;

#ifndef UUID4_STR
#define UUID4_STR
typedef char uuid4_str[37];
#endif /* ifndef UUID4_STR */

#define ON_ERR(ptr, msg) \
	({if (ptr->on_error) ptr->on_error(ptr->on_error_data, msg);})
#define ON_LOG(ptr, msg) \
	({if (ptr->on_log) ptr->on_log(ptr->on_log_data, msg);})

#define STRCOPY(dst, src) \
	({strncpy(dst, src, sizeof(dst)-1); dst[sizeof(dst)-1]=0;})

#ifndef NSTimeIntervalSince1970
#define NSTimeIntervalSince1970  978307200.0
#endif /* ifndef NSTimeIntervalSince1970 */

#endif /* ifndef PROZUBILIB_CONF_H */
