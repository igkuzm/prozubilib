/**
 * File              : log.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 19.03.2023
 * Last Modified Date: 06.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/*
 * Debugging in Android/iOS and other platforms
 */

#ifndef k_lib_log_h
#define k_lib_log_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#define STR(...)     ({char s[BUFSIZ]; snprintf(s, BUFSIZ-1, __VA_ARGS__); s;}) 
#define STR_ERR(...) STR("E/_%s: %s: %s", __FILE__, __func__, STR(__VA_ARGS__))
#define STR_LOG(...) STR("I/_%s: %s: %s", __FILE__, __func__, STR(__VA_ARGS__))
	
#ifdef __cplusplus
}
#endif

#endif /* ifndef k_lib_log_h */
