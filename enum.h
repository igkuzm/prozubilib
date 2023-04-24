/**
 * File              : enum.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 24.04.2023
 * Last Modified Date: 24.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/*
 * macros for emun
 */

#ifndef ENUM_H
#define ENUM_H

    #define BEGIN_ENUM( ENUM_NAME ) typedef enum tag##ENUM_NAME
    #define DECL_ENUM_ELEMENT( element ) element
    #define END_ENUM( ENUM_NAME ) ENUM_NAME; 

    #define BEGIN_ENUM_STRING( ENUM_NAME ) static char* gs_##ENUM_NAME [] =
	#define DECL_ENUM_STRING_ELEMENT( element ) #element
    #define END_ENUM_STRING( ENUM_NAME ) ; static char* getString##ENUM_NAME(enum \
            tag##ENUM_NAME index){ return gs_##ENUM_NAME [index]; }

#endif /* ifndef ENUM_H */
