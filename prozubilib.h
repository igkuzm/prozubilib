/**
 * File              : prozubilib.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 17.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PROZUBILIB_H
#define PROZUBILIB_H

#include <time.h>
#include <stdint.h>

#include "kdata_conf.h"

typedef char uuid4_t[37];


static kdata2_t *
prozubilib_init(
		const char *filepath,
		const char *token
		);


/* ptient struct */
struct patient {
	time_t dateofbirth;
	char address[256];
	char comment[256];
	char document[512];
	char email[128];
	char familiya[128];
	char imia[128];
	char otchestvo[128];
	uuid4_t patientid;
	uint64_t tel;
};

static
void prozubilib_get_patients(
		kdata2_t *kdata,
		void *user_data,
		int (*callback)(struct patient * patient, void *userdata, char *error)
		);

/* ptient struct */
struct doctor {
	uuid4_t doctorid;
	char calendar[256];
	char fio[128];
	uint64_t tel;
};

static
void prozubilib_get_doctors(
		kdata2_t *kdata,
		void *user_data,
		int (*callback)(struct doctor * doctor, void *userdata, char *error)
		);


typedef struct case_t {
	time_t date;
	time_t dateOfNext;

	char   z11[4];
	char   z12[4];
	char   z13[4];
	char   z14[4];
	char   z15[4];
	char   z16[4];
	char   z17[4];
	char   z18[4];

	char   z21[4];
	char   z22[4];
	char   z23[4];
	char   z24[4];
	char   z25[4];
	char   z26[4];
	char   z27[4];
	char   z28[4];

	char   z31[4];
	char   z32[4];
	char   z33[4];
	char   z34[4];
	char   z35[4];
	char   z36[4];
	char   z37[4];
	char   z38[4];
	
	char   z41[4];
	char   z42[4];
	char   z43[4];
	char   z44[4];
	char   z45[4];
	char   z46[4];
	char   z47[4];
	char   z48[4];
	

} prozubilib_case_t;

#endif /* ifndef PROZUBILIB_H */
