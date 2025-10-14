/**
 * File              : documents.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 25.07.2023
 * Last Modified Date: 05.10.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef DOCUMENTS_H
#define DOCUMENTS_H

#include "images.h"
#include "passport.h"
#include "planlecheniya.h"
#include "cases.h"
#include "bill.h"
#include "images.h"
#include "rtf.h"

extern const char EXPORTDLL * 
documents_get_plan_lecheniya(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c
		);

extern const char EXPORTDLL * 
documents_get_case(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c
		);

extern const char EXPORTDLL * 
documents_get_akt(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c
		);

extern const char EXPORTDLL * 
documents_get_dogovor(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient
		);

#endif /* ifndef DOCUMENTS_H */
