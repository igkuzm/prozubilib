/**
 * File              : diagnosis.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 30.05.2023
 * Last Modified Date: 30.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef DIAGNOSIS_H
#define DIAGNOSIS_H
#include "alloc.h"
#include "prozubilib_conf.h"
#include "cases.h"

static int 
_prozubi_diagnosis_poteria_zhevatelnoy_effectivnosti(
		struct case_t *c
		)
{
	int pze=0;
	if (strcmp(c->z46, "0") == 0
		||  strcmp(c->z16, "0") == 0)
		pze +=12;			
	if (strcmp(c->z36, "0") == 0
		||  strcmp(c->z26, "0") == 0)
		pze +=12;			
	if (strcmp(c->z47, "0") == 0
		||  strcmp(c->z17, "0") == 0)
		pze +=10;			
	if (strcmp(c->z37, "0") == 0
		||  strcmp(c->z27, "0") == 0)
		pze +=10;			
	if (strcmp(c->z45, "0") == 0
		||  strcmp(c->z15, "0") == 0)
		pze +=8;			
	if (strcmp(c->z35, "0") == 0
		||  strcmp(c->z25, "0") == 0)
		pze +=8;			
	if (strcmp(c->z44, "0") == 0
		||  strcmp(c->z14, "0") == 0)
		pze +=8;			
	if (strcmp(c->z34, "0") == 0
		||  strcmp(c->z24, "0") == 0)
		pze +=8;			
	if (strcmp(c->z43, "0") == 0
		||  strcmp(c->z13, "0") == 0)
		pze +=6;			
	if (strcmp(c->z33, "0") == 0
		||  strcmp(c->z23, "0") == 0)
		pze +=6;			
	if (strcmp(c->z41, "0") == 0
		||  strcmp(c->z11, "0") == 0)
		pze +=4;			
	if (strcmp(c->z31, "0") == 0
		||  strcmp(c->z21, "0") == 0)
		pze +=4;			
	if (strcmp(c->z42, "0") == 0
		||  strcmp(c->z12, "0") == 0)
		pze +=2;			
	if (strcmp(c->z32, "0") == 0
		||  strcmp(c->z22, "0") == 0)
		pze +=2;					
	return pze;
}

static char *
prozubi_diagnosis_get(
			prozubi_t *p,
			struct case_t *c
		)
{
	char *str = MALLOC(BUFSIZ, 
			if (p->on_error)
				p->on_error(p->on_error_data, "can't allocate memory"), 
			return NULL);
	str[0] = 0;
	int i;

#define DIAGNOSIS_TEETH\
	DIAGNOSIS_TOOTH(z11, "1.1")\
	DIAGNOSIS_TOOTH(z12, "1.2")\
	DIAGNOSIS_TOOTH(z13, "1.3")\
	DIAGNOSIS_TOOTH(z14, "1.4")\
	DIAGNOSIS_TOOTH(z15, "1.5")\
	DIAGNOSIS_TOOTH(z16, "1.6")\
	DIAGNOSIS_TOOTH(z17, "1.7")\
	DIAGNOSIS_TOOTH(z18, "1.8")\
	DIAGNOSIS_TOOTH(z21, "2.1")\
	DIAGNOSIS_TOOTH(z22, "2.2")\
	DIAGNOSIS_TOOTH(z23, "2.3")\
	DIAGNOSIS_TOOTH(z24, "2.4")\
	DIAGNOSIS_TOOTH(z25, "2.5")\
	DIAGNOSIS_TOOTH(z26, "2.6")\
	DIAGNOSIS_TOOTH(z27, "2.7")\
	DIAGNOSIS_TOOTH(z28, "2.8")\
	DIAGNOSIS_TOOTH(z31, "3.1")\
	DIAGNOSIS_TOOTH(z32, "3.2")\
	DIAGNOSIS_TOOTH(z33, "3.3")\
	DIAGNOSIS_TOOTH(z34, "3.4")\
	DIAGNOSIS_TOOTH(z35, "3.5")\
	DIAGNOSIS_TOOTH(z36, "3.6")\
	DIAGNOSIS_TOOTH(z37, "3.7")\
	DIAGNOSIS_TOOTH(z38, "3.8")\
	DIAGNOSIS_TOOTH(z41, "4.1")\
	DIAGNOSIS_TOOTH(z42, "4.2")\
	DIAGNOSIS_TOOTH(z43, "4.3")\
	DIAGNOSIS_TOOTH(z44, "4.4")\
	DIAGNOSIS_TOOTH(z45, "4.5")\
	DIAGNOSIS_TOOTH(z46, "4.6")\
	DIAGNOSIS_TOOTH(z47, "4.7")\
	DIAGNOSIS_TOOTH(z48, "4.8")
	
	char *pt[] = {"R", "Pt"};		
	
	#define DIAGNOSIS_TOOTH(n, t)\
		for (i = 0; i < 2; ++i) {\
			if (strcmp(c->n, pt[i]) == 0){\
				char s[256];\
				sprintf(s, "K04.5. Хронический апикальный периодонтит, зуб %s. ", t);\
				strcat(str, s);\
				break;\
			}\
		}
	DIAGNOSIS_TEETH
	#undef DIAGNOSIS_TOOTH

	#define DIAGNOSIS_TOOTH(n, t)\
		if (strcmp(c->n, "P") == 0){\
			char s[256];\
			sprintf(s, "K04.0. Пульпит, зуб %s. ", t);\
			strcat(str, s);\
		}
	DIAGNOSIS_TEETH
	#undef DIAGNOSIS_TOOTH

	#define DIAGNOSIS_TOOTH(n, t)\
		if (strcmp(c->n, "C") == 0){\
			char s[256];\
			sprintf(s, "K02.2. Кариес цемента, зуб %s. ", t);\
			strcat(str, s);\
		}
	DIAGNOSIS_TEETH
	#undef DIAGNOSIS_TOOTH

	int pze = _prozubi_diagnosis_poteria_zhevatelnoy_effectivnosti(c);
	if (pze){
			char s[256];
			sprintf(s, "K08.1 Потеря зубов. Потеря жевательной эффективности на %d%%.", pze);
			strcat(str, s);
	}
	return str;
}
#endif /* ifndef DIAGNOSIS_H */
