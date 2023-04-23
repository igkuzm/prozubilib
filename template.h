/**
 * File              : template.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 23.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef TEMPLATES_H
#define TEMPLATES_H

#include "prozubilib_conf.h"

#include <string.h>
#include <stdlib.h>

#define TEMPLATES_TABLENAME "ZTEMPLATES"

/*
 * TEMPLATES_COLUMN_TEXT(struct member, enum number, SQLite column title, size)
 * TEMPLATES_COLUMN_TEXT_P(struct member, enum number, SQLite column title)
 */
#define TEMPLATES_COLUMNS \
	TEMPLATES_COLUMN_TEXT(name,   TEMPLATENAME,  "ZTEMPLATENAME", 256)\
	TEMPLATES_COLUMN_TEXT(title,  TEMPLATETITLE, "ZTITLE",        128)\
	TEMPLATES_COLUMN_TEXT_P(text,   TEMPLATETEXT,  "ZTEXT")

struct template_t {
	uuid4_str id;         /* uuid of the template */

#define TEMPLATES_COLUMN_TEXT(member, number, title, size) char member[size]; 
#define TEMPLATES_COLUMN_TEXT_P(member, number, title) char * member; size_t len_##member; 	
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT
#undef TEMPLATES_COLUMN_TEXT_P
};


enum template_column_number {
#define TEMPLATES_COLUMN_TEXT(member, number, title, size) number, 
#define TEMPLATES_COLUMN_TEXT_P(member, number, title) number, 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT
#undef TEMPLATES_COLUMN_TEXT_P

	TEMPLATES_COLS_NUM,
};

static void	
prozubi_templates_table_init(struct kdata2_table **templates){
	kdata2_table_init(templates, TEMPLATES_TABLENAME,

#define TEMPLATES_COLUMN_TEXT(member, number, title, size) KDATA2_TYPE_TEXT, title, 
#define TEMPLATES_COLUMN_TEXT_P(member, number, title) KDATA2_TYPE_TEXT, title, 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT
#undef TEMPLATES_COLUMN_TEXT_P
			
			NULL); 
} 

/* allocate and init new template */
static struct template_t *
prozubi_template_new(
		kdata2_t *kdata,
#define TEMPLATES_COLUMN_TEXT(member, number, title, size) const char * member, 
#define TEMPLATES_COLUMN_TEXT_P(member, number, title) const char * member, 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT
#undef TEMPLATES_COLUMN_TEXT_P
		const char *id
		)
{
	/* allocate case_t */
	struct template_t *t = malloc(sizeof(struct template_t));
	if (!t){
		ERR("%s", "can't allocate struct template_t");
		return NULL;
	}

	if (!id){
		/* create new uuid */
		UUID4_STATE_T state; UUID4_T identifier;
		uuid4_seed(&state);
		uuid4_gen(&state, &identifier);
		if (!uuid4_to_s(identifier, t->id, 37)){
			ERR("%s", "can't generate uuid");
			return NULL;
		}
	} else
		strcpy(t->id, id);

	/* set values */
#define TEMPLATES_COLUMN_TEXT(member, number, title, size)\
   	if (member)\
		kdata2_set_text_for_uuid(kdata, TEMPLATES_TABLENAME, title, member, t->id);	
#define TEMPLATES_COLUMN_TEXT_P(member, number, title)\
   	if (member)\
		kdata2_set_text_for_uuid(kdata, TEMPLATES_TABLENAME, title, member, t->id);	
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT
#undef TEMPLATES_COLUMN_TEXT_P
	
	return t;
}

/* callback all templates for template name; set template name to NULL to list all templates*/
static void 
prozubi_template_foreach(
		kdata2_t   *kdata,
		const char *templatename,
		void       *user_data,
		int        (*callback)(void *user_data, struct template_t *p)
		)
{
	/* check kdata */
	if (!kdata){
		ERR("%s", "kdata is NULL");
		return;
	}
	if (!kdata->db){
		ERR("%s", "kdata->db is NULL");
		return;
	}

	struct template_t t;
	
	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define TEMPLATES_COLUMN_TEXT(member, number, title, size) strcat(SQL, title); strcat(SQL, ", "); 
#define TEMPLATES_COLUMN_TEXT_P(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT			
#undef TEMPLATES_COLUMN_TEXT_P			
	
	strcat(SQL, "ZRECORDNAME FROM ");
	strcat(SQL, TEMPLATES_TABLENAME);
	if (templatename)
		strcat(SQL, STR(" WHERE ZTEMPLATENAME = '%s'", templatename));

	/* start SQLite request */
	int res;
	sqlite3_stmt *stmt;
	
	res = sqlite3_prepare_v2(kdata->db, SQL, -1, &stmt, NULL);
	if (res != SQLITE_OK) {
		ERR("sqlite3_prepare_v2: %s: %s", SQL, sqlite3_errmsg(kdata->db));	
		return;
	}	

	while (sqlite3_step(stmt) != SQLITE_DONE) {
		/* iterate columns */
		int i;
		for (i = 0; i < TEMPLATES_COLS_NUM; ++i) {
			/* handle values */
			switch (i) {

#define TEMPLATES_COLUMN_TEXT(member, number, title, size) \
				case number:\
				{\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					strncpy(t.member, (const char *)value, size - 1);\
					t.member[size - 1] = 0;\
					break;\
				}; 
#define TEMPLATES_COLUMN_TEXT_P(member, number, title) \
				case number:\
				{\
					size_t len = sqlite3_column_bytes(stmt, i);\
					const unsigned char *value = sqlite3_column_text(stmt, i);\
					t.member = (char *)value;\
					t.member[len - 1] = 0;\
					t.len_##member = len;\
					break;\
				};				

			TEMPLATES_COLUMNS

#undef TEMPLATES_COLUMN_TEXT			
#undef TEMPLATES_COLUMN_TEXT_P			

				default:
					break;					
			}
		}		

		/* handle template id */
		const unsigned char *value = sqlite3_column_text(stmt, i);				
		strncpy(t.id, (const char *)value, sizeof(t.id) - 1);
		t.id[sizeof(t.id) - 1] = 0;		

		/* callback */
		if (callback)
			if (callback(user_data, &t))
				break;		
	}	

	sqlite3_finalize(stmt);
}

#endif /* ifndef TEMPLATES_H */
