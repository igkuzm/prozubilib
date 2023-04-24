/**
 * File              : alloc.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 22.02.2022
 * Last Modified Date: 25.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef k_lib_alloc_h__
#define k_lib_alloc_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#define NEW(T) ((T *)malloc(sizeof(T)))

#ifdef __cplusplus
}
#endif

#endif // k_lib_alloc_h__
