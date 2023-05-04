/**
 * File              : template.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 20.04.2023
 * Last Modified Date: 04.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef TEMPLATES_H
#define TEMPLATES_H

#include "prozubilib_conf.h"

#include "enum.h"
#include "alloc.h"

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
	/* allocate case_t */
	struct template_t *t = NEW(struct template_t, 
			ERR("%s", "can't allocate struct template_t"), return NULL);
	
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

	/* create SQL string */
	char SQL[BUFSIZ] = "SELECT ";

#define TEMPLATES_COLUMN_TEXT(member, number, title) strcat(SQL, title); strcat(SQL, ", "); 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT			
	
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
	
		struct template_t *t = NEW(struct template_t, 
				ERR("%s", "can't allocate struct template_t"), return);
	
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
						char *str = MALLOC(len + 1,\
							ERR("can't allocate string with len: %ld", len+1), break);\
						strncpy(str, (const char *)value, len);\
						str[len] = 0;\
						t->member = str;\
						t->len_##member = len;\
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

#endif /* ifndef TEMPLATES_H */
