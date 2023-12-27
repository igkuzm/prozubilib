/**
 * File              : documents.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 25.07.2023
 * Last Modified Date: 27.12.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef DOCUMENTS_H
#define DOCUMENTS_H


#include <cstdlib>
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
#include "rtf.h"

#define DOCUMENTS_TABLENAME "ZDOCUMENTS"
#define OUTFILE "out.rtf"

typedef enum DOCUMENTS_VALUE_TYPE {
	DOCUMENTS_VALUE_TYPE_TEXT,
	DOCUMENTS_VALUE_TYPE_IMAGE,
	DOCUMENTS_VALUE_TYPE_PLAN_LECHENIYA, // set SQL value to
																			 // caseid
	DOCUMENTS_VALUE_TYPE_BILL,           // set SQL value to
																			 // caseid
	DOCUMENTS_VALUE_TYPE_ZFORMULA,       // set SQL value to
																			 // caseid
} DOCUMENTS_VALUE_TYPE;

struct documents_key_t {
	/* data */
	DOCUMENTS_VALUE_TYPE type; // type of value
	char key[128];             // key without mark '$'
	bool multiple;             // true if SQL request has
														 // multiple lines (vaue will
														 // append for each line)
};

/*
 * DOCUMENTS_COLUMN_TEXT(
 * struct member, enum number, SQLite column title, size)
 */
#define DOCUMENTS_COLUMNS \
	DOCUMENTS_COLUMN_TEXT(name,   DOCUMENTSNAME,    "ZNAME")\
	DOCUMENTS_COLUMN_TEXT(data,   DOCUMENTSDATA,    "ZTEMPL")\
	DOCUMENTS_COLUMN_KARRAY(keys, DOCUMENTSKEYS,    "ZKEYS")\
	DOCUMENTS_COLUMN_SARRAY(vars, DOCUMENTSVARS,    "ZVARS")\
	DOCUMENTS_COLUMN_TEXT(sql,    DOCUMENTSQL,      "ZSQL")\

struct document_t {
	uuid4_str id;         /* uuid of the document */

#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
 	char * member; size_t len_##member; 

#define DOCUMENTS_COLUMN_KARRAY(member, number, title)\
 	struct documents_key_t* member; size_t len_##member; 

#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
 	char ** member; size_t len_##member; 

	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_KARRAY
#undef DOCUMENTS_COLUMN_SARRAY
};

BEGIN_ENUM(DOCUMENTS)
#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
	 	DECL_ENUM_ELEMENT(number), 

#define DOCUMENTS_COLUMN_KARRAY(member, number, title)\
	 	DECL_ENUM_ELEMENT(number), 
	
#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
	 	DECL_ENUM_ELEMENT(number), 
	
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_KARRAY
#undef DOCUMENTS_COLUMN_SARRAY

	DOCUMENTS_COLS_NUM,
END_ENUM(DOCUMENTS)

BEGIN_ENUM_STRING(DOCUMENTS)
#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
	 	DECL_ENUM_STRING_ELEMENT(number), 

#define DOCUMENTS_COLUMN_KARRAY(member, number, title)\
	 	DECL_ENUM_STRING_ELEMENT(number), 
	
#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
	 	DECL_ENUM_STRING_ELEMENT(number), 
	
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_KARRAY
#undef DOCUMENTS_COLUMN_SARRAY
END_ENUM_STRING(DOCUMENTS)	

/* init documents table */
static void	
prozubi_documents_table_init(
		struct kdata2_table **documents)
{
	kdata2_table_init(
			documents, DOCUMENTS_TABLENAME,

#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
	KDATA2_TYPE_TEXT, title, 

#define DOCUMENTS_COLUMN_KARRAY(member, number, title)\
	KDATA2_TYPE_TEXT, title, 
	
#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
	KDATA2_TYPE_TEXT, title, 
	
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_KARRAY
#undef DOCUMENTS_COLUMN_SARRAY
			
			NULL); 
} 

static struct document_t *
_prozubi_documents_new(prozubi_t *p, const char *id)
{
	/* allocate document_t */
	struct document_t *t = 
		NEW(struct document_t, 
			if (p->on_error)
				p->on_error(p->on_error_data,			
				STR_ERR("%s", 
					"can't allocate struct document_t")), 
			return NULL);

	if (!id){
		/* create new uuid */
		UUID4_STATE_T state; UUID4_T identifier;
		uuid4_seed(&state);
		uuid4_gen(&state, &identifier);
		if (!uuid4_to_s(
					identifier, t->id, 37))
		{
			if (p->on_error)
				p->on_error(p->on_error_data,			
				STR_ERR("%s", "can't generate uuid"));
			return NULL;
		}
	} else
		strcpy(t->id, id);

	/* set default values */
#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
	 	t->member = NULL; t->len_##member = 0; 

#define DOCUMENTS_COLUMN_KARRAY(member, number, title)\
	 	t->member = NULL; t->len_##member = 0; 

#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
	 	t->member = NULL; t->len_##member = 0; 

DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_KARRAY
#undef DOCUMENTS_COLUMN_SARRAY

	return t;
}

#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
static int prozubi_documents_set_##number(\
		prozubi_t *p, struct document_t *t,\
		const char *text, bool update)\
{\
	if (update)\
		if (!kdata2_set_text_for_uuid(\
					p, DOCUMENTS_TABLENAME, title, text, t->id))\
			return -1;\
	if(t->member)\
		free(t->member);\
	t->member = strdup(text);\
	t->len_##member = strlen(text);\
	return 0;\
}

#define DOCUMENTS_COLUMN_KARRAY(member, number, title)\
static int prozubi_documents_set_##number(\
		prozubi_t *p, struct document_t *t,\
		const struct documents_key_t *keys,\
			int len, bool update)\
{\
	int i;\
	if (keys && len > 0){\
		if (t->member)\
			free(t->member);\
		t->member =\
			(struct documents_key_t *) malloc(\
					sizeof(struct documents_key_t)*len);\
		for (i = 0; i < len; ++i)\
			t->member[i] = keys[i];\
		t->len_##member = len;\
		if (t->member && update){\
			cJSON *json = cJSON_CreateArray();\
			if (json){\
				for (i = 0; i < len; ++i) {\
					cJSON *object = cJSON_CreateObject();\
					if (object){\
						cJSON_AddItemToObject(\
								object, "type",\
								cJSON_CreateNumber(member[i].type));\
						cJSON_AddItemToObject(\
								object, "key",\
								cJSON_CreateString(member[i].key));\
						cJSON_AddItemToObject(\
								object, "multiple",\
								cJSON_CreateBool(member[i].multiple));\
						cJSON_AddItemToArray(\
								json, object);\
					}\
				}\
				if (!kdata2_set_text_for_uuid(\
						p,\
						DOCUMENTS_TABLENAME,\
						title,\
						cJSON_Print(json),\
						t->id))\
					return -1;\
				cJSON_free(json);\
			}\
		}\
	}\
	return 0;\
}\
	
#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
static int prozubi_documents_set_##number(\
		prozubi_t *p, struct document_t *t,\
		const char **array, int len, bool update)\
{\
	char *str = strjoin(array, len, ", ");\
	if (str && len > 0){\
		if (update)\
			if (!kdata2_set_text_for_uuid(\
						p, DOCUMENTS_TABLENAME, title, str, t->id))\
				return -1;\
		if (t->member)\
			free(t->member);\
		t->member = (char**)malloc(len * sizeof(char*));\
		if (t->member){\
			int i;\
			for (i=0;i<len;++i)\
				t->member[i] = strdup(array[i]);\
		}\
		free(str);\
		return 0;\
	}\
	return -1;\
}
		
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_KARRAY			
#undef DOCUMENTS_COLUMN_SARRAY			


/* allocate and init new document */
static struct document_t *
prozubi_documents_new(
		prozubi_t *p,
#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
	 	const char * member, 

#define DOCUMENTS_COLUMN_KARRAY(member, number, title)\
	 	struct documents_key_t * member, int len_##member, 
	
#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
	 	const char ** member, int len_##member, 
	
	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_KARRAY
#undef DOCUMENTS_COLUMN_SARRAY
		const char *id)
{
	if (!p)
		return NULL;

	struct document_t *t =
		_prozubi_documents_new(p, id);
	if (!t)
		return NULL;

		/* set values */
#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
	prozubi_documents_set_##number(p, t, member, true);

#define DOCUMENTS_COLUMN_KARRAY(member, number, title)\
	prozubi_documents_set_##number(\
			p, t, member, len_##member, true);

#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
	prozubi_documents_set_##number(\
			p, t, member, len_##member, true);

	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT
#undef DOCUMENTS_COLUMN_KARRAY
#undef DOCUMENTS_COLUMN_SARRAY
	
	return t;
}

static struct document_t *
prozubi_documents_from_sql(
		prozubi_t *p,
		sqlite3_stmt *stmt)
{
	struct document_t *t = 
		_prozubi_documents_new(p, "");
	if (!t)
		return NULL;

	/* iterate columns */
	int i;
	for (i = 0; i < PRICES_COLS_NUM; ++i) {
		/* handle values */
		switch (i) {

#define DOCUMENTS_COLUMN_TEXT(member, number, title) \
			case number:\
			{\
				size_t len = sqlite3_column_bytes(stmt, i);\
				const unsigned char *value = \
					sqlite3_column_text(stmt, i);\
				if (value){\
					t->member = strndup((char*)value, len);\
					t->len_##member = len;\
				}\
				break;\
			}; 

#define DOCUMENTS_COLUMN_KARRAY(member, number, title) \
			case number:\
			{\
				size_t len = sqlite3_column_bytes(stmt, i);\
				const unsigned char *value =\
					sqlite3_column_text(stmt, i);\
				if (value){\
					cJSON *json = cJSON_ParseWithLength(\
							(const char*)value, len);\
					if (!json)\
						break;\
					t->len_##member = cJSON_GetArraySize(json);\
					t->member = (struct documents_key_t *)\
						malloc(sizeof(struct documents_key_t)*len);\
					if(!t->member)\
						break;\
					int i=0; cJSON *object;\
					cJSON_ArrayForEach(object, json){\
						cJSON *type = \
							cJSON_GetObjectItem(object, "type");\
						cJSON *key = \
							cJSON_GetObjectItem(object, "key");\
						cJSON *multiple = \
							cJSON_GetObjectItem(object, "multiple");\
						t->member[i].type = (DOCUMENTS_VALUE_TYPE)\
							cJSON_GetNumberValue(type);\
						strncpy(t->member[i].key, \
								cJSON_GetStringValue(key),\
								sizeof(t->member[i].key)-1);\
						t->member[i].key[sizeof(t->member[i].key)-1]=0;\
						t->member[i].multiple = \
							cJSON_GetNumberValue(multiple);\
						cJSON_free(json);\
						i++;\
					}\
				}\
				break;\
			};

#define DOCUMENTS_COLUMN_SARRAY(member, number, title) \
			case number:\
			{\
				size_t len = sqlite3_column_bytes(stmt, i);\
				const unsigned char *value =\
					sqlite3_column_text(stmt, i);\
				if (value){\
					t->len_##member = \
						strsplit((char*)value, ", ", &t->member);\
				}\
				break;\
			}; 

		DOCUMENTS_COLUMNS

#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_KARRAY			
#undef DOCUMENTS_COLUMN_SARRAY			

			default:
				break;					
		}
	}		

	/* handle document id */
	const unsigned char *value = 
		sqlite3_column_text(stmt, i);				
	strncpy(
			t->id, 
			(const char *)value,
		 	sizeof(t->id) - 1);
	t->id[sizeof(t->id) - 1] = 0;		

	return t;
}

/* callback all documents */
static void 
prozubi_documents_foreach(
		prozubi_t  *p,
		const char *predicate,
		void       *user_data,
		int        (*callback)
							 (void *user_data, struct document_t *t)
		)
{
	/* check kdata */
	if (!p){
		return;
	}
	if (!p->db){
		if (p->on_error)
			p->on_error(p->on_error_data,		
			STR_ERR("%s", "p->db is NULL"));
		return;
	}

	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
 	strcat(SQL, title); strcat(SQL, ", "); 

#define DOCUMENTS_COLUMN_KARRAY(member, number, title)\
 	strcat(SQL, title); strcat(SQL, ", "); 

#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
 	strcat(SQL, title); strcat(SQL, ", "); 

	DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_KARRAY			
#undef DOCUMENTS_COLUMN_SARRAY			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, DOCUMENTS_TABLENAME);
	strcat(SQL, " ");
	if (predicate)
		strcat(SQL, predicate);

	/* start SQLite request */
	int res;
	sqlite3_stmt *stmt;
	
	res = sqlite3_prepare_v2(
			p->db, SQL,
		 	-1, &stmt, NULL);
	if (res != SQLITE_OK) {
		if (p->on_error)
			p->on_error(p->on_error_data,		
			STR_ERR("sqlite3_prepare_v2: %s: %s", 
				SQL, sqlite3_errmsg(p->db)));	
		return;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
		struct document_t *t =
			prozubi_documents_from_sql(p, stmt);	
		/* callback */
		if (t)
			if (callback)
				if (callback(user_data, t))
					break;		
	}	

	sqlite3_finalize(stmt);
}

static int prozubi_documents_set_text(
		DOCUMENTS key, prozubi_t *p, struct document_t *t,
		const char *text, bool update)
{
	switch (key) {
#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
		case number:\
		return prozubi_documents_set_##number(\
				p, t, text, update);
#define DOCUMENTS_COLUMN_KARRAY(member, number, title)
#define DOCUMENTS_COLUMN_SARRAY(member, number, title)
		
		DOCUMENTS_COLUMNS

#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_KARRAY			
#undef DOCUMENTS_COLUMN_SARRAY			
		
		default:
			break;
	}
	return -1;
}

static int prozubi_documents_set_keys(
		DOCUMENTS key, prozubi_t *p, struct document_t *t,
		const struct documents_key_t *keys, int len, 
		bool update)
{
	switch (key) {
#define DOCUMENTS_COLUMN_TEXT(member, number, title)
#define DOCUMENTS_COLUMN_KARRAY(member, number, title)\
		case number:\
		return prozubi_documents_set_##number(\
				p, t, keys, len, update);
#define DOCUMENTS_COLUMN_SARRAY(member, number, title)
		
		DOCUMENTS_COLUMNS

#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_KARRAY			
#undef DOCUMENTS_COLUMN_SARRAY			
		
		default:
			break;
	}
	return -1;
}

static int prozubi_documents_set_vars(
		DOCUMENTS key, kdata2_t *p, struct document_t *t,
		const char **array, int len, bool update)
{
	switch (key) {
#define DOCUMENTS_COLUMN_TEXT(member, number, title)
#define DOCUMENTS_COLUMN_KARRAY(member, number, title)
#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
		case number:\
		return prozubi_documents_set_##number(\
				p, t, array, len, update);
		
		DOCUMENTS_COLUMNS

#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_KARRAY			
#undef DOCUMENTS_COLUMN_SARRAY			
		
		default:
			break;
	}
	return -1;
}

static void
prozubi_documents_free(struct document_t *t){
	if (t){
		int i;

#define DOCUMENTS_COLUMN_TEXT(member, number, title)\
		if(t->member) free(t->member); 

#define DOCUMENTS_COLUMN_KARRAY(member, number, title)\
		if(t->member) free(t->member);\
		t->len_##member = 0;

#define DOCUMENTS_COLUMN_SARRAY(member, number, title)\
		if (t->member){\
			for (i = 0; i < t->len_##member; ++i)\
				if (t->member[i])\
					free(t->member[i]);\
			free(t->member);\
		}\
		t->len_##member = 0;
		
		DOCUMENTS_COLUMNS
#undef DOCUMENTS_COLUMN_TEXT			
#undef DOCUMENTS_COLUMN_KARRAY			
#undef DOCUMENTS_COLUMN_SARRAY			
		
		free(t);
	}
}

static int prozubi_documents_remove(
		prozubi_t *p, struct document_t *t)
{
	return 
		kdata2_remove_for_uuid(p,
				DOCUMENTS_TABLENAME, t->id);
}

static int prozubi_documents_get_document_cb(
		void *userdata, struct document_t *p)
{
	struct document_t **t = 
		(struct document_t **)userdata;
	if (p)
		*t = p;
	return 0;
}

static struct document_t *prozubi_documents_get(
		prozubi_t *p, const char *uuid)
{
	char predicate[BUFSIZ];
	sprintf(predicate,
			"WHERE ZRECORDNAME = '%s'", uuid);

	struct document_t *t = NULL;
	prozubi_documents_foreach(p, predicate, &t, 
			prozubi_documents_get_document_cb);
	if (!t){
		if (p->on_error)
			p->on_error(p->on_error_data, 
					STR("no document with uuid: %s", uuid));
	}
	return t;
}

static struct document_t *prozubi_documents_get_with_name(
		prozubi_t *p, const char *name)
{
	char predicate[BUFSIZ];
	sprintf(predicate, "WHERE ZNAME = '%s'", name);

	struct document_t *t = NULL;
	prozubi_documents_foreach(p, predicate, &t, 
			prozubi_documents_get_document_cb);
	if (!t){
		if (p->on_error)
			p->on_error(p->on_error_data, 
					STR("no document with name: %s", name));
	}
	return t;
}

struct prozubi_documents_keys_and_values{
	documents_key_t *keys;
	char **values;
	size_t len;
};

static int _prozubi_documents_make_rtf_sfind_cb(
		char **found, void *data)
{
	struct prozubi_documents_keys_and_values *d =
		(struct prozubi_documents_keys_and_values *)data;

	int i;
	for (i = 0; i < d->len; ++i)
		if (strcmp(d->keys[i].key, *found) == 0)
			*found = d->values[i];

	return 0;
}

/* generate rtf from document - set string values of 
 * document markers as arguments */
static int _prozubi_documents_make_rtf(
	prozubi_t *p, const char *outfile, const char *name, ...)
{
	struct document_t *t = 
		prozubi_documents_get_with_name(p, name);
	if (!t)
		return -1;

	if (t->len_keys < 1) {
		if (p->on_error)
			p->on_error(p->on_error_data, 
					STR(
						"no keys set for document: %s", name));
		return -1;
	}
	// allocate values array
	char **values = 
		(char**)malloc(t->len_keys * sizeof(char*));
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
	for (i = 0; i < t->len_keys; ++i) {
		char *marker = t->keys[i].key;
		char *value = va_arg(args, char *);
		if (!value){
			if (p->on_error)
				p->on_error(p->on_error_data, 
						STR("no value set for key: %s", 
							marker));
			return -1;
		}
		values[i] = value;
	}
	va_end(args);
	
	struct prozubi_documents_keys_and_values d =
		{t->keys, values, t->len_keys};
	
	// find keys in text and rename them with values
	char *out = NULL;
	int len = 
		sfind(t->data, &out, t->len_data,
			(char*)"$", &d, 
			_prozubi_documents_make_rtf_sfind_cb);

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

struct _prozubi_documents_sql_replace_args {
	/* data */
	int i; // iter
	struct document_t *t;
	prozubi_t *p;
};

static int _prozubi_documents_sql_replace_args_cb(
		char **found, void *data)
{
	struct _prozubi_documents_sql_replace_args *d =
		(struct _prozubi_documents_sql_replace_args *)data;

	// check iterate
	if (d->i <= d->t->len_vars){
		if (d->p->on_error)
			d->p->on_error(d->p->on_error_data, 
					STR("%s", 
						"too many vars in SQL template"));
		return -1;
	}
		
	char var[8];
	sprintf(var, "var%d", d->i);	

	if (strcmp(var, *found) == 0)
		*found = d->t->vars[d->i];

	d->i++;

	return 0;
}

static char * _prozubi_documents_create_sql_request(
		prozubi_t *p, struct document_t *t)
{
	char *SQL = NULL;
	struct _prozubi_documents_sql_replace_args d = 
		{0, t, p};
	sfind(t->sql, &SQL, 0, "$",
			&d,
			_prozubi_documents_sql_replace_args_cb);	
	return SQL;
}

static int _prozubi_documents_get_values_from_sql(
	prozubi_t *p, struct document_t *t)
{
	int i, k, res;
	sqlite3_stmt *stmt;
	struct str values[t->len_keys];	// array to handle values

	// init values
	for (i = 0; i < t->len_keys; ++i)
		if (str_init(&values[i], BUFSIZ))
			return -1;
	
	// get SQL
	char *SQL = 
		_prozubi_documents_create_sql_request(p, t);
	if (!SQL)
		return -1;

	/* start SQLite request */
	
	res = sqlite3_prepare_v2(
			p->db, SQL,
		 	-1, &stmt, NULL);
	if (res != SQLITE_OK) {
		if (p->on_error)
			p->on_error(p->on_error_data,		
			STR_ERR("sqlite3_prepare_v2: %s: %s", 
				SQL, sqlite3_errmsg(p->db)));	
		return -1;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
		// handle with keys/values
		for (i=0; i<t->len_keys; ++i){
			// key is column title
			const char *key = sqlite3_column_name(stmt, i);
			
			// check key matches in document_t
			for (k = 0; k < t->len_keys; ++k) {
				if (strcmp(key, t->keys[k].key) == 0){
					// get value by it's type
					switch (t->keys[k].type) {
						case DOCUMENTS_VALUE_TYPE_TEXT:
							{
								if (values[k].len == 0 || 
										t->keys[k].multiple)
								{
									// get text
									const unsigned char *value = 
										sqlite3_column_text(stmt, i);
									char *rtf_string =
										rtf_from_utf8((char*)value);
									// append text to keys
									if (rtf_string){
										str_append(
												&values[k], 
												rtf_string,
												strlen(rtf_string));
										free(rtf_string);
									}
								}

								break;
							}
						
						case DOCUMENTS_VALUE_TYPE_IMAGE:
							{
								// get image
								if (values[k].len == 0 || 
										t->keys[k].multiple)
								{
									struct image_t *image = 
										prozubi_image_from_sql(
												p, stmt);
									if (image){
										char *rtf_string = NULL;
										int len = 
											prozubi_image_to_rtf(
													p, image, &rtf_string);
										if (rtf_string){
											str_append(
													&values[k], 
													rtf_string, len);
											free(rtf_string);
										}
										prozubi_image_free(image);
									}
								}
								break;
							}
					
						case DOCUMENTS_VALUE_TYPE_PLAN_LECHENIYA:
							{
								// get planlecheniya
								if (values[k].len == 0 || 
										t->keys[k].multiple)
								{
									char *caseid =
										(char*)sqlite3_column_text(
												stmt, i);
									if (caseid){
										struct case_t *c = 
											prozubi_case_get(p, caseid);
										if (!c){
											if (p->on_error)
												p->on_error(p->on_error_data,
														STR("no case with uuid: %s", 
															caseid));
											break;
										}
										char *rtf_string = NULL;
										size_t len = 
											prozubi_planlecheniya_to_rtf(
												p, c->planlecheniya,
											 	&rtf_string);
										if (rtf_string){
											str_append(
													&values[k], 
													rtf_string, len);
											free(rtf_string);
										}
										prozubi_case_free(c);
									}
								}
								break;
							}
					
						case DOCUMENTS_VALUE_TYPE_BILL:
							{
								// get bill
									char *caseid =
										(char*)sqlite3_column_text(
												stmt, i);
									if (caseid){
										struct case_t *c = 
											prozubi_case_get(p, caseid);
										if (!c){
											if (p->on_error)
												p->on_error(p->on_error_data,
														STR("no case with uuid: %s", 
															caseid));
											break;
										}
										char *rtf_string = NULL;
										size_t len = 
											prozubi_bill_to_rtf(
												p, c->bill,
											 	&rtf_string);
										if (rtf_string){
											str_append(
													&values[k], 
													rtf_string, len);
											free(rtf_string);
										}
										prozubi_case_free(c);
									}
								break;
							}
						
						case DOCUMENTS_VALUE_TYPE_ZFORMULA:
							{
									// get zformula
									char *caseid =
										(char*)sqlite3_column_text(
												stmt, i);
									if (caseid){
										struct case_t *c = 
											prozubi_case_get(p, caseid);
										if (!c){
											if (p->on_error)
												p->on_error(p->on_error_data,
														STR("no case with uuid: %s", 
															caseid));
											break;
										}
										char *rtf_string = NULL;
										size_t len = 
											prozubi_case_zubformula_to_rtf(
												p, c,
											 	&rtf_string);
										if (rtf_string){
											str_append(
													&values[k], 
													rtf_string, len);
											free(rtf_string);
										}
										prozubi_case_free(c);
									}
								break;
							}
					
						default:
							break;
					}
				}
			}
		}
	}	

	sqlite3_finalize(stmt);
	free(SQL);
	return 0;
}
#endif /* ifndef DOCUMENTS_H */
