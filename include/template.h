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

extern void	
prozubi_templates_table_init(struct kdata2_table **templates); 

/* allocate and init new template */
extern struct template_t EXPORTDLL *
prozubi_template_new(
		kdata2_t *kdata,
#define TEMPLATES_COLUMN_TEXT(member, number, title) const char * member, 
	TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT
		const char *id
		);

/* callback all templates for template name; set template name to NULL to list all templates*/
extern void EXPORTDLL
prozubi_template_foreach(
		kdata2_t   *kdata,
		const char *templatename,
		const char *predicate,
		void       *user_data,
		int        (*callback)(void *user_data, struct template_t *p)
		);

#define TEMPLATES_COLUMN_TEXT(member, number, title)\
extern int EXPORTDLL prozubi_template_set_##number (kdata2_t *p, struct template_t *t,\
		const char *text, bool update);

		TEMPLATES_COLUMNS
#undef TEMPLATES_COLUMN_TEXT			

extern int EXPORTDLL prozubi_template_set(
		TEMPLATES key, kdata2_t *p, struct template_t *t, const char *text, bool update);


extern void EXPORTDLL
prozubi_template_free(struct template_t *d);

extern int EXPORTDLL prozubi_template_remove(
		kdata2_t *p, struct template_t *t
		);



#endif /* ifndef TEMPLATES_H */
