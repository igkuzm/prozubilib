/**
 * File              : documents.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 25.07.2023
 * Last Modified Date: 05.10.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef DOCUMENTS_H
#define DOCUMENTS_H


#include <stdint.h>
#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "images.h"
#include "kdata2/cYandexDisk/cJSON.h"
#include "passport.h"
#include "planlecheniya.h"
#include "cases.h"
#include "prozubilib.h"
#include "bill.h"
#include "images.h"
#include "str.h"
#include "rtf.h"

#define OUTFILE "out.rtf"

struct pl_table_str{
	struct str str;
	char *summa;
	char *sroki;
};

static void *
pl_table_cb(void *d, void *p, struct planlecheniya_t *t){
	struct pl_table_str *s = (struct pl_table_str *)d;
	switch (t->type) {
		case PLANLECHENIYA_TYPE_STAGE:
			{
				str_appendf(&s->str, 
							"\\pard\\ql \\b %s \\b0 \\ \\par\n", t->title);
				str_appendf(&s->str, 
					"\\pard\\par\\trowd\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx400\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx7254\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx8254\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx9254\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx10254\n"
					"\\intbl \\b № \\b0 \\cell\n"
					"\\intbl \\b Наименование работы (услуги) \\b0 \\cell\n"
					"\\intbl \\b Количество \\b0 \\cell\n"
					"\\intbl \\b Цена \\b0 \\cell\n"
					"\\intbl \\b Сумма \\b0 \\cell\n"
					"\\row\n");
				break;
			}
		case PLANLECHENIYA_TYPE_ITEM:
			{
				str_appendf(&s->str, 
						"\\trowd\n"
						"\\intbl %d \\cell\n"
						"\\intbl %s \\cell\n"
						"\\intbl %s \\cell\n"
						"\\intbl %s \\cell\n"
						"\\intbl %s \\cell\n"
						"\\row\n"
						, t->itemIndex + 1
						, t->title
						, t->count
						, t->price
						, t->total);
				break;
			}
		case PLANLECHENIYA_TYPE_STAGE_PRICE:
			{
				str_appendf(&s->str, 
						"\\trowd\n"
						"\\intbl  \\cell\n"
						"\\intbl \\b %s \\b0 \\cell\n"
						"\\intbl \\cell\n"
						"\\intbl \\cell\n"
						"\\intbl \\b %s руб. \\b0 \\cell\n"
						"\\row\n"
						, t->title
						, t->total
				);
				break;
			}
		case PLANLECHENIYA_TYPE_STAGE_DURATION:
			{
				str_appendf(&s->str, 
						"\\trowd\n"
						"\\intbl  \\cell\n"
						"\\intbl \\b %s \\b0 \\cell\n"
						"\\intbl \\cell\n"
						"\\intbl \\cell\n"
						"\\intbl \\b %s мес. \\b0 \\cell\n"
						"\\row\\lastrow\\par\n"
						, t->title
						, t->count
				);
				break;
			}
		case PLANLECHENIYA_TYPE_TOTAL_PRICE:
			{
				s->summa = (char *)malloc(strlen(t->total) + 1);
				if (!s->summa){
					perror("realloc");
					exit(EXIT_FAILURE);	
				}
				strcpy(s->summa, t->total);
				break;
			}
		case PLANLECHENIYA_TYPE_TOTAL_DURATION:
			{
				s->sroki = (char *)malloc(strlen(t->count) + strlen(t->title) + 1);
				if (!s->sroki){
					perror("realloc");
					exit(EXIT_FAILURE);	
				}
				sprintf(s->sroki, "%s  %s", t->title, t->count);		
				break;
			}
		default:
			break;
	}
}

static void pl_table(prozubi_t *p, cJSON *pl, char **table, char **summa, char **sroki){
	struct pl_table_str s;
	if (str_init(&s.str))
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	} 

	// create RTF table
	prozubi_planlecheniya_foreach(p, pl, &s, pl_table_cb);

	if (table)
		*table = s.str.str;
	if (summa)
		*summa = s.summa;
	if (sroki)
		*sroki = s.sroki;
}

#define PL_TEETH_UP\
	PL_TOOTH_UP(Z18)\
	PL_TOOTH_UP(Z17)\
	PL_TOOTH_UP(Z16)\
	PL_TOOTH_UP(Z15)\
	PL_TOOTH_UP(Z14)\
	PL_TOOTH_UP(Z13)\
	PL_TOOTH_UP(Z12)\
	PL_TOOTH_UP(Z11)\
	PL_TOOTH_UP(Z21)\
	PL_TOOTH_UP(Z22)\
	PL_TOOTH_UP(Z23)\
	PL_TOOTH_UP(Z24)\
	PL_TOOTH_UP(Z25)\
	PL_TOOTH_UP(Z26)\
	PL_TOOTH_UP(Z27)\
	PL_TOOTH_UP(Z28)\

#define PL_TEETH_DOWN\
	PL_TOOTH_DOWN(Z48)\
	PL_TOOTH_DOWN(Z47)\
	PL_TOOTH_DOWN(Z46)\
	PL_TOOTH_DOWN(Z45)\
	PL_TOOTH_DOWN(Z44)\
	PL_TOOTH_DOWN(Z43)\
	PL_TOOTH_DOWN(Z42)\
	PL_TOOTH_DOWN(Z41)\
	PL_TOOTH_DOWN(Z31)\
	PL_TOOTH_DOWN(Z32)\
	PL_TOOTH_DOWN(Z33)\
	PL_TOOTH_DOWN(Z34)\
	PL_TOOTH_DOWN(Z35)\
	PL_TOOTH_DOWN(Z36)\
	PL_TOOTH_DOWN(Z37)\
	PL_TOOTH_DOWN(Z38)\
	
static void pl_zformula(prozubi_t *p, struct case_t *c, char **zformula){
	const char title[] = "\\pard\\par\\qc \\b Зубная формула \\b0 \\ \n";
	const char *tbl = 
					"\\pard\\par\\trowd\\qc\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx640\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx1280\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx1921\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx2562\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx3203\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx3844\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx4485\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx5126\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx5767\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx6408\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx7049\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx7690\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx8331\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx8972\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx9613\n"
					"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
					"\\cellx10254\n"
					;
					
	char up[BUFSIZ];
	strcpy(up, "");
#define PL_TOOTH_UP(n)\
	const char *v_##n = (char *)prozubi_case_get_##n(c);\
	if (!v_##n) v_##n = "";\
	strcat(up, "\\intbl ");\
	strcat(up, v_##n);\
	strcat(up, " \\cell\n");\

	PL_TEETH_UP
#undef PL_TEETH_UP
	strcat(up,
					"\\row\n"
					"\\intbl 1.8 \\cell\n"
					"\\intbl 1.7 \\cell\n"
					"\\intbl 1.6 \\cell\n"
					"\\intbl 1.5 \\cell\n"
					"\\intbl 1.4 \\cell\n"
					"\\intbl 1.3 \\cell\n"
					"\\intbl 1.2 \\cell\n"
					"\\intbl 1.1 \\cell\n"
					"\\intbl 2.1 \\cell\n"
					"\\intbl 2.2 \\cell\n"
					"\\intbl 2.3 \\cell\n"
					"\\intbl 2.4 \\cell\n"
					"\\intbl 2.5 \\cell\n"
					"\\intbl 2.6 \\cell\n"
					"\\intbl 2.7 \\cell\n"
					"\\intbl 2.8 \\cell\n"
					"\\row\n"
					"\\intbl 4.8 \\cell\n"
					"\\intbl 4.7 \\cell\n"
					"\\intbl 4.6 \\cell\n"
					"\\intbl 4.5 \\cell\n"
					"\\intbl 4.4 \\cell\n"
					"\\intbl 4.3 \\cell\n"
					"\\intbl 4.2 \\cell\n"
					"\\intbl 4.1 \\cell\n"
					"\\intbl 3.1 \\cell\n"
					"\\intbl 3.2 \\cell\n"
					"\\intbl 3.3 \\cell\n"
					"\\intbl 3.4 \\cell\n"
					"\\intbl 3.5 \\cell\n"
					"\\intbl 3.6 \\cell\n"
					"\\intbl 3.7 \\cell\n"
					"\\intbl 3.8 \\cell\n"
					"\\row\n"
					);
	
	char down[BUFSIZ];
	strcpy(down, "");
#define PL_TOOTH_DOWN(n)\
	const char *v_##n = (char *)prozubi_case_get_##n(c);\
	if (!v_##n) v_##n = "";\
	strcat(down, "\\intbl ");\
	strcat(down, v_##n);\
	strcat(down, " \\cell\n");\

	PL_TEETH_DOWN
#undef PL_TEETH_DOWN
	strcat(down, "\\row\\lastrow\n");

		*zformula = (char *)malloc(
				strlen(title) + 
				strlen(tbl)  +
				strlen(up)  +
				strlen(down) + 1);
		if (!*zformula){
			perror("realloc");
			exit(EXIT_FAILURE);	
		}
		strcpy(*zformula, title);
		strcat(*zformula, tbl);
		strcat(*zformula, up);
		strcat(*zformula, down);
}

struct pl_images{
	struct str str;
	prozubi_t *p;
};

static int
pl_images_cb(void *d, struct image_t *image){
	struct pl_images *img = (struct pl_images *)d;	
	if (!image){
		if (img->p->on_error)
			img->p->on_error(img->p->on_error_data,
					STR("can't load image: NULL"));
		return 0;
	}	
	
	// try to load image
	int x, y, c;
	if (!stbi_info_from_memory(
				(const stbi_uc *)(image->data), 
				image->len_data, &x, &y, &c))
	{
		if (img->p->on_error)
			img->p->on_error(img->p->on_error_data,
					STR("can't load image: %s", image->id));
		return 0;
	}

	int i;
	str_appendf(&img->str,
		"{\\pict\\picw0\\pich0\\picwgoal10254\\pichgoal6000\\jpegblip\n");

	for (i = 0; i < image->len_data; i++) {
		char bit = ((char *)image->data)[i];
		str_appendf(&img->str, "%02x", bit);
	}
	str_appendf(&img->str, "}\n");

	return 0;
}

static void 
pl_images_get(
		prozubi_t *p, struct case_t *c, struct pl_images *img)
{
	prozubi_image_foreach(p, c->id, NULL, 
			img, pl_images_cb);
}

#define PL_REPS\
	PL_REP("$date", c->date, PL_DATE)\
	PL_REP("$familiya", patient->familiya, PL_TEXT)\
	PL_REP("$imia", patient->imia, PL_TEXT)\
	PL_REP("$otchestvo", patient->otchestvo, PL_TEXT)\
	PL_REP("$diagnozis", c->diagnozis, PL_TEXT)\
	PL_REP("$image", image, PL_NONE)\
	PL_REP("$zformula", zformula, PL_TEXT)\
	PL_REP("$tables", table, PL_TEXT)\
	PL_REP("$summa", summa, PL_NONE)\
	PL_REP("$srokiLecheniya", sroki, PL_TEXT)\
	PL_REP("$doctor", "Семенцов И.В.", PL_TEXT)\
	PL_REP("$patientID", patient->id, PL_TEXT)\
	PL_REP("$document", patient->document, PL_TEXT)\
	PL_REP("$address", patient->address, PL_TEXT)\
	PL_REP("$tel", patient->tel, PL_TEXT)\
	PL_REP("$email", patient->email, PL_TEXT)\
	PL_REP("$zhalobi", c->zhalobi, PL_TEXT)\
	PL_REP("$anamnezMorbi", c->anamnezmorbi, PL_TEXT)\
	PL_REP("$sostoyaniye", c->sostoyanie, PL_TEXT)\
	PL_REP("$soznaniye", c->soznaniye, PL_TEXT)\
	PL_REP("$polozheniye", c->polozheniye, PL_TEXT)\
	PL_REP("$soStoroniVnutrennihOrganov", c->sostoyanievnutrennihorganov, PL_TEXT)\
	PL_REP("$mestno", c->mestno, PL_TEXT)\
	PL_REP("$instrumentAndRentgen", c->instrumentandrentgen, PL_TEXT)\
	PL_REP("$diagnozis", c->diagnozis, PL_TEXT)\
	PL_REP("$lecheniye", c->lecheniye, PL_TEXT)\
	PL_REP("$recomendacii", c->recomendacii, PL_TEXT)\
	PL_REP("$z11", c->z11, PL_TEXT)\
	PL_REP("$z12", c->z12, PL_TEXT)\
	PL_REP("$z13", c->z13, PL_TEXT)\
	PL_REP("$z14", c->z14, PL_TEXT)\
	PL_REP("$z15", c->z15, PL_TEXT)\
	PL_REP("$z16", c->z16, PL_TEXT)\
	PL_REP("$z17", c->z17, PL_TEXT)\
	PL_REP("$z18", c->z18, PL_TEXT)\
	PL_REP("$z21", c->z21, PL_TEXT)\
	PL_REP("$z22", c->z22, PL_TEXT)\
	PL_REP("$z23", c->z23, PL_TEXT)\
	PL_REP("$z24", c->z24, PL_TEXT)\
	PL_REP("$z25", c->z25, PL_TEXT)\
	PL_REP("$z26", c->z26, PL_TEXT)\
	PL_REP("$z27", c->z27, PL_TEXT)\
	PL_REP("$z28", c->z28, PL_TEXT)\
	PL_REP("$z31", c->z31, PL_TEXT)\
	PL_REP("$z32", c->z32, PL_TEXT)\
	PL_REP("$z33", c->z33, PL_TEXT)\
	PL_REP("$z34", c->z34, PL_TEXT)\
	PL_REP("$z35", c->z35, PL_TEXT)\
	PL_REP("$z36", c->z36, PL_TEXT)\
	PL_REP("$z37", c->z37, PL_TEXT)\
	PL_REP("$z38", c->z38, PL_TEXT)\
	PL_REP("$z41", c->z41, PL_TEXT)\
	PL_REP("$z42", c->z42, PL_TEXT)\
	PL_REP("$z43", c->z43, PL_TEXT)\
	PL_REP("$z44", c->z44, PL_TEXT)\
	PL_REP("$z45", c->z45, PL_TEXT)\
	PL_REP("$z46", c->z46, PL_TEXT)\
	PL_REP("$z47", c->z47, PL_TEXT)\
	PL_REP("$z48", c->z48, PL_TEXT)\

enum {
	PL_NONE,
	PL_DATE,
	PL_TEXT,
};

static const char *pl_needle_array[] = 
{
	#define PL_REP(needle, ...) needle,
PL_REPS
	#undef PL_REP
	NULL
};

static char *pl_replace(
		int index,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c,
		char *table,
		char *summa,
		char *sroki,
		char *zformula,
		char *image
		)
{
	int num = 0;
#define PL_REP(needle, rep, type)\
	if (index == num){\
		if (type == PL_TEXT){\
			char *ccc = rtf_from_utf8((char *)rep);\
			return ccc;\
		}\
		else if (type == PL_NONE){\
			return strdup((char *)rep);\
		}\
		else if (type == PL_DATE){\
			time_t time = (time_t)rep;\
			struct tm *tm = localtime(&time);\
			char *date = (char *)malloc(11);\
			if (!date) return NULL;\
			strftime(date, 11, "%d.%m.%Y", tm);\
			return date;\
		}\
	}\
	num++;
PL_REPS
	#undef PL_REP
	return NULL;
}

static const char * 
documents_get_plan_lecheniya(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c
		)
{
	int i;

	// open template
	FILE *in = fopen(template_file_path, "r");
	if (!in){
		if (p->on_error)
			p->on_error(p->on_error_data,			
				STR_ERR("can't read file: %s", 
				template_file_path));
		return NULL;	
	}

	// remove temp file
	remove(OUTFILE);
	// open out file
	FILE *out = fopen(OUTFILE, "w+");
	if (!in){
		if (p->on_error)
			p->on_error(p->on_error_data,			
				STR_ERR("can't wtite file: %s", OUTFILE));
		fclose(in);
		return NULL;	
	}

	// get table
	char *table, *summa, *sroki;
	pl_table(p, c->planlecheniya, &table,
		 	&summa, &sroki);
	
	// get zformula
	char *zformula;
	void *zdata; size_t zlen;
	if (_prozubi_zformula_image(p, c,
			 	"zubFormula.png",
			 	&zdata, &zlen))
	{
		pl_zformula(p, c, &zformula);
	} else {
		zformula = 
			rtf_from_image("png", zdata, zlen,
				 	10250, 6000);
	}

	// load images
	struct pl_images img;
	if (str_init(&img.str)){
		perror("malloc");
		return NULL;
	}
	pl_images_get(p, c, &img);

	// parse RTF
	int ch;
	while ((ch = fgetc(in)) != EOF) {
		// check if word starts with '$'
		if (ch == '$'){
			// get word
			char buf[256];
			i=0;
			while (
					ch != ' ' && ch != '\\' && 
					ch != '.' && ch != '\t' && 
					ch != '{' && ch != '}' && 
					ch != '\n' && ch != '\r' && 
					ch != ',' && ch != EOF
					)
			{
				buf[i++] = ch;
				ch = fgetc(in);
			}
			
			// terminate buffer
			buf[i] = 0;

			// convert buf to replace
			i = 0;
			char **needles = (char **)pl_needle_array;
			while (needles[i]) {
				char *needle = needles[i];	
				if (strcmp(buf, needle) == 0)
				{
					char *replace = pl_replace(
							i, p, patient, c, 
							table, summa, sroki, 
							zformula, img.str.str);
					// put replace to out stream
					if (replace){
						fputs(replace, out);
						free(replace);
					}
					break;
				}
				i++;
			}
			
		
		} else
			fputc(ch, out);
	}

	free(table);
	free(summa);
	free(sroki);
	
	free(zformula);

	fclose(in);
	fclose(out);

	return OUTFILE;
}

static const char * 
documents_get_case(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c
		)
{
	int i;

	// open template
	FILE *in = fopen(template_file_path, "r");
	if (!in){
		if (p->on_error)
			p->on_error(p->on_error_data,			
				STR_ERR("can't read file: %s", 
				template_file_path));
		return NULL;	
	}

	// remove temp file
	remove(OUTFILE);
	// open out file
	FILE *out = fopen(OUTFILE, "w+");
	if (!in){
		if (p->on_error)
			p->on_error(p->on_error_data,			
				STR_ERR("can't wtite file: %s", OUTFILE));
		fclose(in);
		return NULL;	
	}

	// parse RTF
	int ch;
	while ((ch = fgetc(in)) != EOF) {
		// check if word starts with '$'
		if (ch == '$'){
			// get word
			char buf[256];
			i=0;
			while (
					ch != ' ' && ch != '\\' && 
					ch != '.' && ch != '\t' && 
					ch != '{' && ch != '}' && 
					ch != '\n' && ch != '\r' && 
					ch != ',' && ch != EOF
					)
			{
				buf[i++] = ch;
				ch = fgetc(in);
			}
			
			// terminate buffer
			buf[i] = 0;

			// convert buf to replace
			i = 0;
			char **needles = (char **)pl_needle_array;
			while (needles[i]) {
				char *needle = needles[i];	
				if (strcmp(buf, needle) == 0)
				{
					char *replace = pl_replace(
							i, p, patient, c, 
							NULL, NULL, NULL, 
							NULL, NULL);
					// put replace to out stream
					if (replace){
						fputs(replace, out);
						free(replace);
					}
					break;
				}
				i++;
			}
			
		
		} else
			fputc(ch, out);
	}

	fclose(in);
	fclose(out);

	return OUTFILE;
}

static void 
bill_table_cb(void *d, struct bill_t *t){
	struct pl_table_str *s = (struct pl_table_str *)d;
	switch (t->type) {
		case BILL_TYPE_ITEM:
			{
				str_appendf(&s->str, 
						"\\trowd\n"
						"\\intbl %d \\cell\n"
						"\\intbl %s \\cell\n"
						"\\intbl %s \\cell\n"
						"\\intbl %s \\cell\n"
						"\\intbl %s \\cell\n"
						"\\row\n"
						, t->itemIndex + 1
						, t->title
						, t->count
						, t->price
						, t->total);
				break;
			}
		
		case BILL_TYPE_TOTAL_PRICE:
			{
				s->summa = (char *)malloc(strlen(t->total) + 1);
				if (!s->summa){
					perror("realloc");
					exit(EXIT_FAILURE);	
				}
				strcpy(s->summa, t->total);
				break;
			}
		default:
			break;
	}
}

static void bill_table(
		prozubi_t *p, cJSON *bill, char **table)
{
	struct pl_table_str s;
	if (str_init(&s.str))
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	} 
		str_appendf(&s.str, 
			"\\pard\\par\\trowd\n"
			"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
			"\\cellx400\n"
			"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
			"\\cellx7254\n"
			"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
			"\\cellx8254\n"
			"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
			"\\cellx9254\n"
			"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
			"\\cellx10254\n"
			"\\intbl \\b № \\b0 \\cell\n"
			"\\intbl \\b Наименование работы (услуги) \\b0 \\cell\n"
			"\\intbl \\b Количество \\b0 \\cell\n"
			"\\intbl \\b Цена \\b0 \\cell\n"
			"\\intbl \\b Сумма \\b0 \\cell\n"
			"\\row\n");

	// create RTF table
	prozubi_bill_foreach(p, bill, &s, bill_table_cb);

	if (table)
		*table = s.str.str;
}


static const char * 
documents_get_akt(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c
		)
{
	int i;

	// open template
	FILE *in = fopen(template_file_path, "r");
	if (!in){
		if (p->on_error)
			p->on_error(p->on_error_data,			
				STR_ERR("can't read file: %s", 
				template_file_path));
		return NULL;	
	}

	// remove temp file
	remove(OUTFILE);
	// open out file
	FILE *out = fopen(OUTFILE, "w+");
	if (!in){
		if (p->on_error)
			p->on_error(p->on_error_data,			
				STR_ERR("can't wtite file: %s", OUTFILE));
		fclose(in);
		return NULL;	
	}

	// get table
	char *bill = NULL;
	bill_table(p, c->bill, &bill);
	//prozubi_bill_to_rtf(p, c->bill, &bill);
	
	// parse RTF
	int ch;
	while ((ch = fgetc(in)) != EOF) {
		// check if word starts with '$'
		if (ch == '$'){
			// get word
			char buf[256];
			i=0;
			while (
					ch != ' ' && ch != '\\' && 
					ch != '.' && ch != '\t' && 
					ch != '{' && ch != '}' && 
					ch != '\n' && ch != '\r' && 
					ch != ',' && ch != EOF
					)
			{
				buf[i++] = ch;
				ch = fgetc(in);
			}
			
			// terminate buffer
			buf[i] = 0;

			// convert buf to replace
			i = 0;
			char **needles = (char **)pl_needle_array;
			while (needles[i]) {
				char *needle = needles[i];	
				if (strcmp(buf, needle) == 0)
				{
					char *replace = pl_replace(
							i, p, patient, c, 
							bill, NULL, NULL, 
							NULL, NULL);
					// put replace to out stream
					if (replace){
						fputs(replace, out);
						free(replace);
					}
					break;
				}
				i++;
			}
			
		
		} else
			fputc(ch, out);
	}

	if (bill)
		free(bill);

	fclose(in);
	fclose(out);

	return OUTFILE;
}


static const char * 
documents_get_dogovor(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient
		)
{
	int i;

	// open template
	FILE *in = fopen(template_file_path, "r");
	if (!in){
		if (p->on_error)
			p->on_error(p->on_error_data,			
				STR_ERR("can't read file: %s", 
				template_file_path));
		return NULL;	
	}

	// remove temp file
	remove(OUTFILE);
	// open out file
	FILE *out = fopen(OUTFILE, "w+");
	if (!in){
		if (p->on_error)
			p->on_error(p->on_error_data,			
				STR_ERR("can't wtite file: %s", OUTFILE));
		fclose(in);
		return NULL;	
	}

	struct case_t c;
	c.date = time(NULL);

	// parse RTF
	int ch;
	while ((ch = fgetc(in)) != EOF) {
		// check if word starts with '$'
		if (ch == '$'){
			// get word
			char buf[256];
			i=0;
			while (
					ch != ' ' && ch != '\\' && 
					ch != '.' && ch != '\t' && 
					ch != '{' && ch != '}' && 
					ch != '\n' && ch != '\r' && 
					ch != ',' && ch != EOF
					)
			{
				buf[i++] = ch;
				ch = fgetc(in);
			}
			
			// terminate buffer
			buf[i] = 0;

			// convert buf to replace
			i = 0;
			char **needles = (char **)pl_needle_array;
			while (needles[i]) {
				char *needle = needles[i];	
				if (strcmp(buf, needle) == 0)
				{
					char *replace = pl_replace(
							i, p, patient, &c, 
							NULL, NULL, NULL, 
							NULL, NULL);
					// put replace to out stream
					if (replace){
						fputs(replace, out);
						free(replace);
					}
					break;
				}
				i++;
			}
			
		
		} else
			fputc(ch, out);
	}

	fclose(in);
	fclose(out);

	return OUTFILE;
}


#endif /* ifndef DOCUMENTS_H */
