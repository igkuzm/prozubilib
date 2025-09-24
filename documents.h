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
#include "fm.h"

#define OUTDIR "out.rtfd"
#define OUTFILE OUTDIR "/TXT.rtf"
#define ZFORMULAIMG "zubFormula.png"

static void free_out_dir(){
	// remove temp file
	remove(OUTFILE);
	
	// remove outdir
	dir_foreach(OUTDIR, file){
		char path[BUFSIZ];
		sprintf(path, "%s/%s", OUTDIR, file->d_name);
		remove(path);
	}
	rmdir(OUTDIR);
	newdir(OUTDIR, 0755);
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
	
	// add image to rtfd for macos support
	char path[BUFSIZ];
	sprintf(path, OUTDIR "/%s.jpeg", image->id);
	FILE *fp = fopen(path, "w");
	if (fp){
		fwrite(image->data, image->len_data, 1, fp);
		fclose(fp);
	}
	
	int i;
	str_appendf(&img->str,
		"{\\pict\\picw0\\pich0\\picwgoal10254\\pichgoal6000\\jpegblip\n");

	for (i = 0; i < image->len_data; i++) {
		char bit = ((char *)image->data)[i];
		str_appendf(&img->str, "%02x", bit);
	}
	str_appendf(&img->str, "}\n");
	
	// add macos support
	str_appendf(&img->str, "\\f0\\fs24 \\cf0 {{\\*\\NeXTGraphic %s.jpeg \\width10250 \\height6000}}\n", image->id);

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

static int
_documents_template_prepare(const char *template_file_path, 
							prozubi_t *p,
							FILE **in, FILE **out)
{
	// open template
	*in = fopen(template_file_path, "r");
	if (*in == NULL){
		if (p->on_error)
			p->on_error(p->on_error_data,			
						STR_ERR("can't read file: %s", 
								template_file_path));
		return 1;	
	}
	
	// remove temp file
	free_out_dir();
	
	// open out file
	*out = fopen(OUTFILE, "w+");
	if (*out == NULL){
		if (p->on_error)
			p->on_error(p->on_error_data,			
						STR_ERR("can't wtite file: %s", OUTFILE));
		fclose(in);
		return 1;	
	}
	
	return 0;
}

static const char*
_documents_finish(prozubi_t *p, FILE *in, FILE *out)
{
	fclose(in);
	fclose(out);
	
#ifdef	__APPLE__
	return OUTDIR;
#else
	return OUTFILE;
#endif

}

static void
_documents_parse_rtf(prozubi_t *p, FILE *in, FILE *out,
					 struct passport_t *patient, 		
					 struct case_t *c,
					 char *table,
					 char *summa,
					 char *sroki,
					 char *zformula,
					 char *image)
{
	// parse RTF
	int ch, i;
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
											   zformula, image);
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
}

static const char * 
documents_get_plan_lecheniya(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c
		)
{
	// open template
	FILE *in, *out;
	if (_documents_template_prepare(template_file_path, p, &in, &out))
		return NULL;

	// get table
	char *table;
	prozubi_planlecheniya_to_rtf(p, c->planlecheniya, &table);
	int duration = prozubi_planlecheniya_get_duration_total(p, c->planlecheniya);
	char sroki[128];
	sprintf(sroki,"Общая продолжительность лечения (мес.): %d", duration);
	int total = prozubi_planlecheniya_get_price_total(p, c->planlecheniya);
	char summa[128];
	sprintf(summa,"%d ", total);

	// get zformula
	char *zformula;
	void *zdata; size_t zlen;
	if (_prozubi_zformula_image(p, c,
			 	ZFORMULAIMG,
			 	&zdata, &zlen))
	{
		// add table
		prozubi_case_zubformula_to_rtf(p, c, &zformula);

	} else {
		// add image to rtf
		struct str s;
		str_init(&s);
		char *img = rtf_from_image("png", zdata, zlen,
								   10250, 6000);
		
		str_append(&s, img, strlen(img));
		
		// MAC OS add image to rtfd
		char path[BUFSIZ];
		sprintf(path, OUTDIR "/" ZFORMULAIMG);
		//fcopy(ZFORMULAIMG, path);
		
		int x, y, c,
			to_x = 450, to_y = 225;
		stbi_uc *image = stbi_load_from_memory((unsigned char *)zdata, zlen, &x, &y, &c, 0);
		if (image){
			stbi_uc *resized = malloc(to_x*to_y*c);
			if (resized){
				if (stbir_resize_uint8(image, x, y, 0, resized, to_x, to_y, 0, c)){
					stbi_write_png(path, to_x, to_y, c, resized, 0);
				}
				str_appendf(&s, "\\f0\\fs24 \\cf0 {{\\*\\NeXTGraphic "ZFORMULAIMG" \\width%d \\height%d}}\n", to_x, to_y);
				stbi_image_free(resized);
			}
			stbi_image_free(image);
		}
		
		free(zdata);
		zformula = s.str;
	}

	// load images
	struct pl_images img;
	if (str_init(&img.str)){
		perror("malloc");
		return NULL;
	}
	pl_images_get(p, c, &img);

	// parse RTF
	_documents_parse_rtf(p, in, out, patient, c, table,
						 summa, sroki, zformula, img.str.str);	

	free(table);	
	free(zformula);

	return _documents_finish(p, in, out);
}

static const char * 
documents_get_case(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c
		)
{
	// open template
	FILE *in, *out;
	if (_documents_template_prepare(template_file_path, p, &in, &out))
		return NULL;
		
	// parse RTF
	_documents_parse_rtf(p, in, out, patient, c, NULL,
						 NULL, NULL, NULL, NULL);	

	return _documents_finish(p, in, out);
}

static const char * 
documents_get_akt(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c
		)
{
	// open template
	FILE *in, *out;
	if (_documents_template_prepare(template_file_path, p, &in, &out))
		return NULL;
		
	// get table
	char *bill = NULL;
	prozubi_bill_to_rtf(p, c->bill, &bill);
	
	if (bill){
		
		// parse RTF
		_documents_parse_rtf(p, in, out, patient, c, bill,
						 NULL, NULL, NULL, NULL);		
		free(bill);
	}
	
	return _documents_finish(p, in, out);
}


static const char * 
documents_get_dogovor(
		const char *template_file_path,
		prozubi_t *p,
		struct passport_t *patient
		)
{
	// open template
	FILE *in, *out;
	if (_documents_template_prepare(template_file_path, p, &in, &out))
		return NULL;
	
	struct case_t c;
	c.date = time(NULL);

	// parse RTF
	_documents_parse_rtf(p, in, out, patient, &c, NULL,
						 NULL, NULL, NULL, NULL);	
	
	return _documents_finish(p, in, out);
}


#endif /* ifndef DOCUMENTS_H */
