/**
 * File              : documents.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 25.07.2023
 * Last Modified Date: 12.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef DOCUMENTS_H
#define DOCUMENTS_H


#include <stdarg.h>
#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "images.h"
#include "kdata2/cYandexDisk/cJSON.h"
#include "log.h"
#include "passport.h"
#include "planlecheniya.h"
#include "cases.h"
#include "prozubilib.h"
#include "images.h"
#include "rtf.h"
#include "ffind.h"
#include "bill.h"
#include "str.h"
#include "enum.h"
#include "strsplit.h"
#include "strjoin.h"
#include "sfind.h"

#define DOCUMENTS_TABLENAME "ZDOCUMENTS"
#define OUTFILE "out.rtf"

/*
 * DOCUMENTS_COLUMN_TEXT(struct member, enum number, SQLite column title, size)
 */
#define DOCUMENTS_COLUMNS \
	DOCUMENTS_COLUMN_TEXT(name,      DOCUMENTSNAME,    "ZNAME")\
	DOCUMENTS_COLUMN_TEXT(data,      DOCUMENTSDATA,    "ZTEMPLATE")\
	DOCUMENTS_COLUMN_SARRAY(markers, DOCUMENTSMARKERS, "ZMARKERS")

struct document_t {
	uuid4_str id;         /* uuid of the document */

#define DOCUMENTS_COLUMN_TEXT(member, number, title) char * member; size_t len_##member; 
#define DOCUMENTS_COLUMN_SARRAY(member, number, title) char ** member; size_t len_##member; 
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_SARRAY
};


BEGIN_ENUM(DOCUMENTS)
#define DOCUMENTS_COLUMN_TEXT(member, number, title) DECL_ENUM_ELEMENT(number), 
#define DOCUMENTS_COLUMN_SARRAY(member, number, title) DECL_ENUM_ELEMENT(number), 
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_SARRAY

	DOCUMENTS_COLS_NUM,
END_ENUM(DOCUMENTS)

BEGIN_ENUM_STRING(DOCUMENTS)
#define DOCUMENTS_COLUMN_TEXT(member, number, title) DECL_ENUM_STRING_ELEMENT(number), 
#define DOCUMENTS_COLUMN_SARRAY(member, number, title) DECL_ENUM_STRING_ELEMENT(number), 
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_SARRAY
END_ENUM_STRING(DOCUMENTS)	

static void	
prozubi_documents_table_init(struct kdata2_table **documents){
	kdata2_table_init(documents, DOCUMENTS_TABLENAME,

#define DOCUMENTS_COLUMN_TEXT(member, number, title) KDATA2_TYPE_TEXT, title, 
#define DOCUMENTS_COLUMN_SARRAY(member, number, title) KDATA2_TYPE_TEXT, title, 
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_SARRAY
			
			NULL); 
} 

/* allocate and init new document */
static struct document_t *
prozubi_documents_new(
		kdata2_t *kdata,
#define DOCUMENTS_COLUMN_TEXT(member, number, title) const char * member, 
#define DOCUMENTS_COLUMN_SARRAY(member, number, title) const char ** member, int len_##member, 
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_SARRAY
		const char *id
		)
{
	if (!kdata)
		return NULL;

	/* allocate document_t */
	struct document_t *p = NEW(struct document_t, 
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,			
			STR_ERR("%s", "can't allocate struct document_t")), return NULL);

	if (!id){
		/* create new uuid */
		UUID4_STATE_T state; UUID4_T identifier;
		uuid4_seed(&state);
		uuid4_gen(&state, &identifier);
		if (!uuid4_to_s(identifier, p->id, 37)){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
				STR_ERR("%s", "can't generate uuid"));
			return NULL;
		}
	} else
		strcpy(p->id, id);

	/* set values */
#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
	if (member){\
		kdata2_set_text_for_uuid(kdata, DOCUMENTS_TABLENAME, title, member, p->id);\
		p->member = strdup(member);\
		p->len_##member = strlen(member);\
	}
#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
		if (member && len_##member > 0){\
			char *str = strjoin(member, len_##member, ", ");\
			if (str){\
				kdata2_set_text_for_uuid(kdata, DOCUMENTS_TABLENAME, title, str, p->id);\
				p->member = (char**)malloc(len_##member * sizeof(char*));\
				if (p->member){\
					int i;\
					for (i=0;i<len_##member;++i)\
						p->member[i] = strdup(member[i]);\
				}\
				free(str);\
			}\
		}
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_SARRAY
	
	return p;
}

/* callback all documents */
static void 
prozubi_documents_foreach(
		kdata2_t   *kdata,
		const char *predicate,
		void       *user_data,
		int        (*callback)(void *user_data, struct document_t *p)
		)
{
	/* check kdata */
	if (!kdata){
		return;
	}
	if (!kdata->db){
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("%s", "kdata->db is NULL"));
		return;
	}

	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define DOCUMENTS_COLUMN_TEXT(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
#define DOCUMENTS_COLUMN_SARRAY(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_SARRAY			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, DOCUMENTS_TABLENAME);
	strcat(SQL, " ");
	if (predicate)
		strcat(SQL, predicate);

	/* start SQLite request */
	int res;
	sqlite3_stmt *stmt;
	
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		if (kdata->on_error)
			kdata->on_error(kdata->on_error_data,		
			STR_ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db)));	
		return;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
	
		struct document_t *p = NEW(struct document_t, 
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,				
				STR_ERR("%s", "can't allocate struct document_t")), return);

		/* iterate columns */
		int i;
		for (i = 0; i < PRICES_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define DOCUMENTS_COLUMN_TEXT(member, number, title) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					if (value){\
						p->member = strndup((char*)value, len);\
						p->len_##member = len;\
					} else {\
						p->member = NULL;\
						p->len_##member = 0;\
					}\
					break;\
				}; 
#define DOCUMENTS_COLUMN_SARRAY(member, number, title) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					if (value){\
						p->len_##member = strsplit((char*)value, ", ", &p->member);\
					} else {\
						p->member = NULL;\
						p->len_##member = 0;\
					}\
					break;\
				}; 

			DOCUMENTS_COLUMNS

#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_SARRAY			

				default:
					break;					
			}
		}		

		/* handle document id */
		const unsigned char *value = sqlite3_column_text(stmt, i);				
		strncpy(p->id, (const char *)value, sizeof(p->id) - 1);
		p->id[sizeof(p->id) - 1] = 0;		

		/* callback */
		if (callback)
			if (callback(user_data, p))
				break;		
	}	

	sqlite3_finalize(stmt);
}

#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
static int prozubi_documents_set_##number (kdata2_t *p, struct document_t *c,\
		const char *text, bool update)\
{\
	if (update)\
		if (!kdata2_set_text_for_uuid(p, DOCUMENTS_TABLENAME, title, text, c->id))\
			return -1;\
	if(c->member)\
		free(c->member);\
	c->member = strdup(text);\
	c->len_##member = strlen(text);\
	return 0;\
}
#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
static int prozubi_documents_set_##number (kdata2_t *p, struct document_t *c,\
		const char **array, int len, bool update)\
{\
	char *str = strjoin(array, len, ", ");\
	if (str && len > 0){\
		if (update)\
			if (!kdata2_set_text_for_uuid(p, DOCUMENTS_TABLENAME, title, str, c->id))\
				return -1;\
		if (c->member)\
			free(c->member);\
		c->member = (char**)malloc(len * sizeof(char*));\
		if (c->member){\
			int i;\
			for (i=0;i<len;++i)\
				c->member[i] = strdup(array[i]);\
		}\
		free(str);\
		return 0;\
	}\
	return -1;\
}
		
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_SARRAY			

static int prozubi_documents_set_text(
		DOCUMENTS key, kdata2_t *p, struct document_t *c, const char *text, bool update)
{
	switch (key) {
#define DOCUMENTS_COLUMN_TEXT(member, number, title) case number:\
		return prozubi_documents_set_##number(p, c, text, update);
#define DOCUMENTS_COLUMN_SARRAY(member, number, title)
		
		DOCUMENTS_COLUMNS

#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_SARRAY			
		
		default:
			break;
	}
	return -1;
}

static int prozubi_documents_set_array(
		DOCUMENTS key, kdata2_t *p, struct document_t *c, const char **array, int len, bool update)
{
	switch (key) {
#define DOCUMENTS_COLUMN_SARRAY(member, number, title) case number:\
		return prozubi_documents_set_##number(p, c, array, len, update);
#define DOCUMENTS_COLUMN_TEXT(member, number, title)
		
		DOCUMENTS_COLUMNS

#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_SARRAY			
		
		default:
			break;
	}
	return -1;
}

static void
prozubi_documents_free(struct document_t *d){
	if (d){

#define DOCUMENTS_COLUMN_TEXT(member, number, title) if(d->member) free(d->member); 
#define DOCUMENTS_COLUMN_SARRAY(member, number, title) if(d->member) free(d->member); 
		DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_SARRAY			
		
		free(d);
		d = NULL;
	}
}

static int prozubi_documents_remove(
		kdata2_t *p, struct document_t *c
		)
{
	return kdata2_remove_for_uuid(p, DOCUMENTS_TABLENAME, c->id);
}

static int prozubi_documents_get_document_cb(
		void *userdata, struct document_t *p)
{
	struct document_t **c = 
		(struct document_t **)userdata;
	if (p)
		*c = p;
	return 0;
}

static struct document_t *prozubi_documents_get(
		kdata2_t *p, const char *uuid)
{
	char predicate[BUFSIZ];
	sprintf(predicate, "WHERE ZRECORDNAME = '%s'", uuid);

	struct document_t *c = NULL;
	prozubi_documents_foreach(p, predicate, &c, 
			prozubi_documents_get_document_cb);
	if (!c){
		if (p->on_error)
			p->on_error(p->on_error_data, 
					STR("no document with uuid: %s", uuid));
	}
	return c;
}

static struct document_t *prozubi_documents_get_with_name(
		kdata2_t *p, const char *name)
{
	char predicate[BUFSIZ];
	sprintf(predicate, "WHERE ZNAME = '%s'", name);

	struct document_t *c = NULL;
	prozubi_documents_foreach(p, predicate, &c, 
			prozubi_documents_get_document_cb);
	if (!c){
		if (p->on_error)
			p->on_error(p->on_error_data, 
					STR("no document with name: %s", name));
	}
	return c;
}

struct prozubi_documents_markers_and_values{
	char **markers;
	char **values;
	size_t len;
};

static int prozubi_documents_make_rtf_sfind_cb(
		char **found, void *data)
{
	struct prozubi_documents_markers_and_values *d =
		(struct prozubi_documents_markers_and_values *)data;

	int i;
	for (i = 0; i < d->len; ++i)
		if (strcmp(d->markers[i], *found) == 0)
			*found = d->values[i];

	return 0;
}

/* generate rtf from document - set string values of 
 * document markers as arguments */
static int prozubi_documents_make_rtf(
	kdata2_t *p, const char *outfile, const char *name, ...)
{
	struct document_t *c = 
		prozubi_documents_get_with_name(p, name);
	if (!c)
		return -1;

	if (c->len_markers < 1) {
		if (p->on_error)
			p->on_error(p->on_error_data, 
					STR("no markers set for document: %s", name));
		return -1;
	}
	// allocate values array
	char **values = 
		(char**)malloc(c->len_markers * sizeof(char*));
	if (!values){
		if (p->on_error)
			p->on_error(p->on_error_data, 
					STR("can't allocate memory"));
		return 1;
	}

	// check argements and fill values array
	va_list args;
	va_start(args, name);
	
	int i;
	for (i = 0; i < c->len_markers; ++i) {
		char *marker = c->markers[i];
		char *value = va_arg(args, char *);
		if (!value){
			if (p->on_error)
				p->on_error(p->on_error_data, 
						STR("no value argument set for marker: %s", 
							marker));
			return -1;
		}
		values[i] = value;
	}
	va_end(args);
	
	struct prozubi_documents_markers_and_values d =
		{c->markers, values, c->len_markers};
	
	// find markers in text and rename them with values
	char *out = NULL;
	int len = 
		sfind(c->data, &out, c->len_data, (char*)"$", &d, 
			prozubi_documents_make_rtf_sfind_cb);

	if (len < 0 || !out){
		if (p->on_error)
			p->on_error(p->on_error_data, 
					STR("error in sfind"));
		return -1;
	}

	// write rtf
	FILE *fp = fopen(outfile, "w");
	if (!fp){
		if (p->on_error)
			p->on_error(p->on_error_data, 
					STR("can't write to: %s", outfile));
		return -1;
	}

	fwrite(out, len, 1, fp);
	fclose(fp);

	free(values);
	free(out);

	return 0;
}

struct pl_table_data{
	struct str *s;
	char *summa;
	char *sroki;
};

static void *
pl_table_cb(void *d, void *p, struct planlecheniya_t *t){
	struct pl_table_data *data = (struct pl_table_data *)d;
	switch (t->type) {
		case PLANLECHENIYA_TYPE_STAGE:
			{
				char title[256];
					sprintf(title, 
							"\\pard\\par\\ql \\b %s \\b0 \\ \n", t->title);

				const char *titles[] =
				{
					"\\b № \\b0",
					"\\b Наименование работы (услуги) \\b0"
					"\\b Количество \\b0"
					"\\b Цена \\b0"
					"\\b Сумма \\b0"
				};
				int width[] = 
					{400, 6854, 1000, 1000, 1000};
				char *tbl = 
					rtf_table_header(5, titles, width);
				
				str_append(data->s, title);
				str_append(data->s, tbl);
				free(tbl);
				break;
			}
		case PLANLECHENIYA_TYPE_ITEM:
			{
				char row[256];
				sprintf(row, 
						"%d=%s=%s=%s=%s"
						, t->itemIndex + 1
						, t->title
						, t->count
						, t->price
						, t->total
				);
				char *tbl = 
					rtf_table_row_from_string(row, "=");
				str_append(data->s, tbl);
				free(tbl);
				break;
			}
		case PLANLECHENIYA_TYPE_STAGE_PRICE:
			{
				char row[256];
				sprintf(row, 
						"=\\b %s \\b0==\\b %s руб. \\b0="
						, t->title
						, t->total
				);
				char *tbl = 
					rtf_table_row_from_string(row, "=");
				str_append(data->s, tbl);
				free(tbl);
				break;
			}
		case PLANLECHENIYA_TYPE_STAGE_DURATION:
			{
				char row[256];
				sprintf(row, 
						"==\\b %s \\b0===\\b %s мес. \\b0"
						, t->title
						, t->count
				);
				char *tbl = 
					rtf_table_row_from_string(row, "=");
				str_append(data->s, tbl);
				free(tbl);
				break;
			}
		case PLANLECHENIYA_TYPE_TOTAL_PRICE:
			{
				data->summa = 
					(char *)malloc(strlen(t->total) + 1);
				if (!data->summa){
					perror("realloc");
					exit(EXIT_FAILURE);	
				}
				strcpy(data->summa, t->total);
				break;
			}
		case PLANLECHENIYA_TYPE_TOTAL_DURATION:
			{
				data->sroki = 
					(char *)malloc(
							strlen(t->count) + strlen(t->title) + 1);
				if (!data->sroki){
					perror("realloc");
					exit(EXIT_FAILURE);	
				}
				sprintf(data->sroki, 
						"%s  %s", t->title, t->count);		
				break;
			}
		default:
			break;
	}
}

static void pl_table(
		prozubi_t *p, cJSON *pl, char **table, 
		char **summa, char **sroki)
{
	struct pl_table_data data;
	struct str s;
	str_init(&s);
	data.s = &s;

	// create RTF table
	prozubi_planlecheniya_foreach(
			p, pl, &data, pl_table_cb);

	if (table)
		*table = data.s->str;
	if (summa)
		*summa = data.summa;
	if (sroki)
		*sroki = data.sroki;
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
	char *images;
	size_t size;
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
	if (!stbi_info_from_memory(image->data, 
				image->len_data, &x, &y, &c))
	{
		if (img->p->on_error)
			img->p->on_error(img->p->on_error_data,
					STR("can't load image: %s", image->id));
		return 0;
	}

	int i;
	const char *header = 
		"{\\pict\\picw0\\pich0\\picwgoal10254\\pichgoal6000\\jpegblip\n";

	img->size = (image->len_data * 3 + strlen(header));

	img->images = (char *)realloc(img->images, img->size);
	if (!img->images){
		perror("malloc");
		exit(EXIT_FAILURE);
	} 
	strcat(img->images, header);
	for (i = 0; i < image->len_data; i++) {
		char bit = ((char *)image->data)[i];
		char s[3];
		sprintf(s, "%02x", bit);
		strcat(img->images, s);
	}
	strcat(img->images, "}\n");

	return 0;
}

static void 
pl_images_get(prozubi_t *p, struct case_t *c, struct pl_images *img){
	prozubi_image_foreach(p, c->id, NULL, img, pl_images_cb);
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
	PL_REP("$summa", summa, PL_TEXT)\
	PL_REP("$srokiLecheniya", sroki, PL_TEXT)\
	PL_REP("$doctor", "Семенцов И.В.", PL_TEXT)\

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
	int num = 0, i;
#define PL_REP(needle, rep, type)\
	if (index == num){\
		if (type == PL_TEXT){\
			char *str = (char *)rep;\
			return rtf_from_utf8(str);\
		}\
		else if (type == PL_NONE){\
			return (char *)rep;\
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
	pl_table(p, c->planlecheniya, &table, &summa, &sroki);
	
	// get zformula
	char *zformula;
	pl_zformula(p, c, &zformula);

	// load images
	struct pl_images img;
	img.size = 1;
	img.images = (char *)malloc(img.size);
	if (!img.images){
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	strcpy(img.images, "");
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
			buf[i++] = 0;
			// return last symbol to out
			fputc(ch, out);

			// convert buf to replace
			i = 0;
			char **needles = (char **)pl_needle_array;
			while (needles[i]) {
				char *needle = needles[i++];	
				if (strstr(buf, needle)){
					char *replace = pl_replace(
							i-1, p, patient, c, 
							table, summa, sroki, 
							zformula, img.images);
					// put replace to out stream
					if (replace){
						fputs(replace, out);
						free(replace);
					}
					break;
				}
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

static void
akt_table_cb(void *d, struct bill_t *t)
{
	struct str *str = (struct str *)d;
	if (str->str[0] == 0)
	{
			const char *tbl = 
			"\\pard\\par\\trowd\n"	"\\clbrdrt\\brdrs\\clbrdrl\\brdrs\\clbrdrb\\brdrs\\clbrdrr\\brdrs\n"
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
			"\\row\n"
			;
		str_append(str, tbl);
	}
	
	if (t->type == BILL_TYPE_ITEM)
	{
		char row[256];
		sprintf(row, 
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
				, t->total
		);
		str_append(str, row);
	}
	else if (t->type == BILL_TYPE_TOTAL_PRICE)
	{
		char row[256];
		sprintf(row, 
				"\\trowd\n"
				"\\intbl  \\cell\n"
				"\\intbl \\b %s \\b0 \\cell\n"
				"\\intbl \\cell\n"
				"\\intbl \\cell\n"
				"\\intbl \\b %s руб. \\b0 \\cell\n"
				"\\row\\lastrow\n"
				, t->title
				, t->total
		);
		str_append(str, row);
	}
}

static void akt_table(
		prozubi_t *p, 
		cJSON *bill, 
		char **table)
{
	struct str str;
	str_init(&str);

	// create RTF table
	prozubi_bill_foreach(p, bill, &str, akt_table_cb);

	if (table)
		*table = str.str;
}

#define AKT_REPS\
	AKT_REP("$date", c->date, PL_DATE)\
	AKT_REP("$familiya", patient->familiya, PL_TEXT)\
	AKT_REP("$imia", patient->imia, PL_TEXT)\
	AKT_REP("$otchestvo", patient->otchestvo, PL_TEXT)\
	AKT_REP("$table", table, PL_TEXT)\
	AKT_REP("$doctor", "Семенцов И.В.", PL_TEXT)\

static const char *akt_needle_array[] = 
{
	#define AKT_REP(needle, ...) needle,
AKT_REPS
	#undef AKT_REP
	NULL
};

static char *akt_replace(
		int index,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c,
		char *table)
{
	int num = 0, i;
#define AKT_REP(needle, rep, type)\
	if (index == num){\
		if (type == PL_TEXT){\
			char *str = (char *)rep;\
			return rtf_from_utf8(str);\
		}\
		else if (type == PL_NONE){\
			return (char *)rep;\
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
AKT_REPS
	#undef AKT_REP
	return NULL;
}

static const char * 
documents_get_plan_atk_sdachi_priema(
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
	char *table;
	akt_table(p, c->bill, &table);
	
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
			buf[i++] = 0;
			// return last symbol to out
			fputc(ch, out);

			// convert buf to replace
			i = 0;
			char **needles = (char **)pl_needle_array;
			while (needles[i]) {
				char *needle = needles[i++];	
				if (strstr(buf, needle)){
					char *replace = akt_replace(
							i-1, p, patient, c, 
							table);
					// put replace to out stream
					if (replace){
						fputs(replace, out);
						free(replace);
					}
					break;
				}
			}
		} else
			fputc(ch, out);
	}

	free(table);
	fclose(in);
	fclose(out);

	return OUTFILE;
}


#endif /* ifndef DOCUMENTS_H */
