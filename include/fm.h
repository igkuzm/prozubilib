/**
 * File              : fm.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.09.2021
 * Last Modified Date: 09.10.2025
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/**
 * fm.h
 * Copyright (c) 2021 Igor V. Sementsov <ig.kuzm@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * FileManager
 * Functions to manage files and directories
 */

#ifndef k_lib_fm_h__
#define k_lib_fm_h__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <windows.h>
#ifndef bool
#define bool char
#define true 1
#define false 0
#endif /* bool */
#else /* _WIN32 */
#include <stdbool.h>
#endif /* _WIN32 */

#ifdef _MSC_VER
#else
#include <dirent.h>
#include <libgen.h>   /* basename, basedir */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

/* fexists
 * true if file exists and writable
 * %path - file path */
static bool fexists(const char *path);

/* fsize
 * return file size
 * %path - file path */
static off_t fsize(const char *path);

/* homedir
 * return allocated string with path to home directory */
static char * homedir();

/* parentdir
 * modify argument string - remove last path component
 * from path string
 * %path - name or path of file */
static char * parentdir(char *path);

/* isdir
 * true if directory at path exists
 * and is accesable
 * %path - directory path */
static bool isdir(const char *path);

/* islink
 * true if file or directory is link
 * %path - file/directory path */
static bool islink(const char *path);

/* slink
 * create symbolic link
 * %path - file/directory path
 * %linkname */
static int slink(const char *path, const char *linkname);

/* hlink
 * create hard link
 * %path - file/directory path
 * %linkname */
static int hlink(const char *path, const char *linkname);

/* fext
 * return file extension or NULL on error 
 * %filename - name or path of file */
static const char * fext(const char *filename);

/* fname
 * return allocated string with file name without 
 * extension and path
 * %path - name or path of file */
static char * fname(char *path);

/* dname
 * return allocated string with name of 
 * directory path (like POSIX dirname())
 * %path - path of file */
static char * dname(const char *path);

/* fcopy 
 * copy and overwrite file 
 * return 0 on success
 * %from - filepath source file
 * %to   - filepath dastination file */ 
static int fcopy(const char *from, const char *to);

/* dcopy 
 * copy directory recursive
 * return 0 on success
 * %from - filepath source file
 * %to   - filepath dastination file 
 * %overwrite - overwrite destination file if true */ 
static int dcopy(
		const char *from, const char *to, bool overwrite);

/* dcopyf
 * copy directory content with comma-separated filters 
 * applied
 * return 0 on success
 * %from - filepath source file
 * %to   - filepath dastination file 
 * %overwrite - overwrite destination file if true
 * %filters - coma-separated filters like '*' or '*.ext' */
static int dcopyf(
		const char *from, const char *to, bool overwrite, 
		char *filters);

/* newdir
 * create new directory
 * link to mkdir function with universal
 * for unix/windows args
 * %path - directory path with name
 * %mode - access mode (not used in windows) */
static int newdir(const char *path, int mode);

/* dir_foreach
 * scans directory and handle each file
 * %path - directory path with name
 * %file - pointer to dirent entry 
 * dir_foreach(path, file) */

#ifdef _MSC_VER
typedef struct dirent {
    char        d_name[MAX_PATH];
} dirent_t;

#endif

#ifdef _WIN32
/* scandir - implimation for Windows
 * scans the directory dirp, calling filter()
 * on each directory entry.
 * Entries for which filter() returns
 * nonzero are stored in strings allocated 
 * via malloc, sorted using qsort with the 
 * comparison function compar(), and
 * collected in array namelist which is 
 * allocated via malloc. If filter is NULL, all 
 * entries are selected.
 * The alphasort() and versionsort() functions can be 
 * used as the comparison function compar(). The 
 * former sorts directory entries using strcoll, the 
 * latter using strverscmp on the strings
 * (*a)->d_name and (*b)->d_name.
 * %dirp - directory path
 * %namelist - allocated namelist array of dirents
 * %filter - filter funcion
 * %compar - sort function 
 * int (*compar)(const struct dirent**, const struct dirent**) */
static int 
scandir(
		 const char * dirp,
		 struct dirent *** namelist,
		 int (*filter)(const struct dirent *),
		 int (*compar)(const void *, const void *));
#endif

#ifdef _WIN32
static int win_find_data_to_dirent(
		WIN32_FIND_DATA *findData,
		struct dirent *entry)
{
	strncpy(entry->d_name, (const char *)findData->cFileName, 
			sizeof(entry->d_name) - 1);
	entry->d_name[sizeof(entry->d_name)-1] = 0;

	return 1;
}

#define dir_foreach(path, file) \
char _fullpath[MAX_PATH];\
WIN32_FIND_DATA _findData;\
struct dirent file[1];\
HANDLE _hFind;\
int _r = 1;\
sprintf(_fullpath, "%s\\*", path);\
for (_hFind = FindFirstFile(_fullpath, &_findData);\
		 _hFind != INVALID_HANDLE_VALUE && win_find_data_to_dirent(&_findData, file) && _r != 0 || !FindClose(_hFind);\
		 _r = FindNextFile(_hFind, &_findData))

#else

#define dir_foreach(path, file)\
	DIR *_dir = opendir(path);\
	struct dirent *file = NULL;\
	if (_dir)\
		for(file = readdir(_dir);\
				file || ({if(_dir) closedir(_dir); 0;});\
				file = readdir(_dir))
#endif


#ifdef __cplusplus
}
#endif

#endif // k_lib_fm_h__
