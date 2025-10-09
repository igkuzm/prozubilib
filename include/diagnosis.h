/**
 * File              : diagnosis.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 30.05.2023
 * Last Modified Date: 24.08.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef DIAGNOSIS_H
#define DIAGNOSIS_H
#include "prozubilib_conf.h"
#include "cases.h"

extern int 
_prozubi_diagnosis_poteria_zhevatelnoy_effectivnosti(
		struct case_t *c
		);

extern char *
prozubi_diagnosis_get(
			prozubi_t *p,
			struct case_t *c
		);

#endif /* ifndef DIAGNOSIS_H */
