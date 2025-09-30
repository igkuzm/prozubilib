/**
 * File              : template.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 19.09.2024
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef TEMPLATES_H
#define TEMPLATES_H

#include "prozubilib_conf.h"

#include "enum.h"

#include <string.h>
#include <stdlib.h>

#define TEMPLATES_TABLENAME "ZTEMPLATES"

/*
 * TEMPLATES_COLUMN_TEXT(struct member, enum number, SQLite column title, size)
 * TEMPLATES_COLUMN_TEXT_P(struct member, enum number, SQLite column title)
 */
#define TEMPLATES_COLUMNS \
	TEMPLATES_COLUMN_TEXT(name,     TEMPLATENAME,  "ZTEMPLATENAME")\
	TEMPLATES_COLUMN_TEXT(title,    TEMPLATETITLE, "ZTITLE")\
	TEMPLATES_COLUMN_TEXT(text,     TEMPLATETEXT,  "ZTEXT")

struct template_t {
	uuid4_str id;         /* uuid of the template */

#define TEMPLATES_COLUMN_TEXT(member, number, title) char * member; size_t len_##member; 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT
};


BEGIN_ENUM(TEMPLATES) 
#define TEMPLATES_COLUMN_TEXT(member, number, title) DECL_ENUM_ELEMENT(number), 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT

	TEMPLATES_COLS_NUM
END_ENUM(TEMPLATES)

BEGIN_ENUM_STRING(TEMPLATES) 
#define TEMPLATES_COLUMN_TEXT(member, number, title) DECL_ENUM_STRING_ELEMENT(number), 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT
END_ENUM_STRING(TEMPLATES)	

static void	
prozubi_templates_table_init(struct kdata2_table **templates){
	kdata2_table_init(templates, TEMPLATES_TABLENAME,

#define TEMPLATES_COLUMN_TEXT(member, number, title) KDATA2_TYPE_TEXT, title, 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT
			
			NULL); 
} 

/* allocate and init new template */
static struct template_t *
prozubi_template_new(
		kdata2_t *kdata,
#define TEMPLATES_COLUMN_TEXT(member, number, title) const char * member, 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT
		const char *id
		)
{
	if (!kdata)
		return NULL;

	/* allocate case_t */
	struct template_t *t = NEW(struct template_t);
	if (t == NULL){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,			
				STR_ERR("%s", "can't allocate struct template_t")); 
			return NULL;
	} 
	
	if (!id){
		/* create new uuid */
		uuid4_init();
		uuid4_generate(t->id);
	} else
		strcpy(t->id, id);

	/* set values */
#define TEMPLATES_COLUMN_TEXT(member, number, title)\
   	if (member)\
		kdata2_set_text_for_uuid(kdata, TEMPLATES_TABLENAME, title, member, t->id);	
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT
	
	return t;
}

/* callback all templates for template name; set template name to NULL to list all templates*/
static void 
prozubi_template_foreach(
		kdata2_t   *kdata,
		const char *templatename,
		const char *predicate,
		void       *user_data,
		int        (*callback)(void *user_data, struct template_t *p)
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

#define TEMPLATES_COLUMN_TEXT(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, TEMPLATES_TABLENAME);
	if (templatename)
		strcat(SQL, STR(" WHERE ZTEMPLATENAME = '%s'", templatename));
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
	
		struct template_t *t = NEW(struct template_t);
		if (t == NULL){
			if (kdata->on_error)
				kdata->on_error(kdata->on_error_data,				
				STR_ERR("%s", "can't allocate struct template_t")); 
			return;
		}
	
		/* iterate columns */
		int i;
		for (i = 0; i < TEMPLATES_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define TEMPLATES_COLUMN_TEXT(member, number, title) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					if (value){\
						t->member = strdup((char*)value);\
						t->len_##member = len;\
					} else {\
						t->member = NULL;\
					}\
					break;\
				};				

			TEMPLATES_COLUMNS

#undef TEMPLATES_COLUMN_TEXT			

				default:
					break;					
			}
		}		

		/* handle template id */
		const unsigned char *value = sqlite3_column_text(stmt, i);				
		strncpy(t->id, (const char *)value, sizeof(t->id) - 1);
		t->id[sizeof(t->id) - 1] = 0;		

		/* callback */
		if (callback)
			if (callback(user_data, t))
				break;		
	}	

	sqlite3_finalize(stmt);
}

#define TEMPLATES_COLUMN_TEXT(member, number, title)\
static int prozubi_template_set_##number (kdata2_t *p, struct template_t *t,\
		const char *text, bool update)\
{\
	if (update)\
		if (!kdata2_set_text_for_uuid(p, TEMPLATES_TABLENAME, title, text, t->id))\
			return -1;\
	if(t->member)\
		free(t->member);\
	t->member = strdup(text);\
	t->len_##member = strlen(text);\
	return 0;\
}
		TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT			

static int prozubi_template_set(
		TEMPLATES key, kdata2_t *p, struct template_t *t, const char *text, bool update)
{
	switch (key) {
#define TEMPLATES_COLUMN_TEXT(member, number, title) case number:\
		return prozubi_template_set_##number(p, t, text, update);\
		
		TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT			
		
		default:
			break;
	}
	return -1;
}

static void
prozubi_template_free(struct template_t *d){
	if (d){

#define TEMPLATES_COLUMN_TEXT(member, number, title) if(d->member) free(d->member); 
		TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT			
		
		free(d);
		d = NULL;
	}
}

static int prozubi_template_remove(
		kdata2_t *p, struct template_t *t
		)
{
	return kdata2_remove_for_uuid(p,
		 	TEMPLATES_TABLENAME, t->id);
}


#endif /* ifndef TEMPLATES_H */
