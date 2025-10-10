/**
 * File              : fm.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.09.2021
 * Last Modified Date: 09.10.2025
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "../include/fm.h"

#ifdef _WIN32
#include <io.h>
#define F_OK 0
#define access _access
#define _SLASH_ "\\"
#ifndef INVALID_FILE_ATTRIBUTES  
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1) 
#endif
#else /* _WIN32 */
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h> /* mkdir, stat */
#define _SLASH_ "/"
#endif

#ifdef _MSC_VER
const char * basename(const char *path) {
	const char *slash = strrchr(path, '\\');
	if (!slash || slash == path)
		return path;
	slash += 1;
	return slash;
}
#endif

/* fexists
 * true if file exists and writable
 * %path - file path */
bool fexists(const char *path) {
  if (access(path, F_OK) == 0)
    return true;
  return false;
}

/* fsize
 * return file size
 * %path - file path */
off_t fsize(const char *path) {
#if defined _WIN32
  WIN32_FIND_DATA findData;
  HANDLE hFind = INVALID_HANDLE_VALUE;
	off_t size = 0;

  hFind = FindFirstFile(path, &findData);
  if (hFind != INVALID_HANDLE_VALUE) 
    size = (findData.nFileSizeHigh * MAXDWORD) + 
			findData.nFileSizeLow;
	return size;
  
#else
  struct stat st;
  if (stat(path, &st) == 0)
    return st.st_size;
  
#endif
  return 0;
}

/* homedir
 * return allocated string with path to home directory */
char *homedir(void)
{
	char homedir[BUFSIZ];
#ifdef _WIN32
	sprintf(homedir, "%s%s", getenv("HOMEDRIVE"), 
			getenv("HOMEPATH"));
#else
	snprintf(homedir, BUFSIZ,
		 	"%s", getenv("HOME"));
#endif
	return strdup(homedir);																								        
}

/* parentdir
 * modify argument string - remove last path component
 * from path string
 * %path - name or path of file */
char * parentdir(char *path) {
	const char *slash = strrchr(path, _SLASH_[0]);
	if (slash && slash != path)
		path[slash-path] = 0;
#ifndef _WIN32
	else
		strcpy(path, "/");
#endif
	return path;
}

/* isdir
 * true if directory at path exists
 * and is accesable
 * %path - directory path */
bool isdir(const char *path) {
#if defined _WIN32
  WIN32_FIND_DATA findData;
  HANDLE hFind = INVALID_HANDLE_VALUE;
  char fullpath[MAX_PATH];
  sprintf(fullpath, "%s\\*", path);

  hFind = FindFirstFile(fullpath, &findData);
  if (hFind != INVALID_HANDLE_VALUE)
    return true;
#else
  struct dirent *entry;
  DIR *dp = opendir(path);
  if (dp != NULL) {
    closedir(dp);
    return true;
  }
#endif
  return false;
}

/* islink
 * true if file or directory is link
 * %path - file/directory path */
bool islink(const char *path) {
#ifdef _WIN32
	DWORD attr = GetFileAttributes(path);
	if (attr != INVALID_FILE_ATTRIBUTES && 
			(attr & FILE_ATTRIBUTE_REPARSE_POINT) == FILE_ATTRIBUTE_REPARSE_POINT)
		return true;
#else
  struct stat buf;
	if (lstat(path, &buf) == 0 &&
			(buf.st_mode & S_IFMT) == S_IFLNK)
		return true;
#endif
  
	return false;
}

/* slink
 * create symbolic link
 * %path - file/directory path
 * %linkname */
int slink(const char *path, const char *linkname)
{
#ifdef _WIN32
	// change path components
	int ret;
	char *s = strdup(path);
	if (s){
		char *p = strrchr(s, '/');
		while(p){
			s[p-s] = '\\';
			p = strrchr(s, '/');
		}
		ret = CreateSymbolicLink(linkname, s, 0);
		free(s);
		if (ret == 1)
			return 0;
	}
	return -1;
#else
	return symlink(path, linkname);
#endif
}

/* hlink
 * create hard link
 * %path - file/directory path
 * %linkname */
int hlink(const char *path, const char *linkname)
{
#ifdef _WIN32
	// change path components
	int ret;
	char *s = strdup(path);
	if (s){
		char *p = strrchr(s, '/');
		while(p){
			s[p-s] = '\\';
			p = strrchr(s, '/');
		}
		ret = CreateHardLink(linkname, s, NULL);
		free(s);
		if (ret == 1)
			return 0;
	}
	return -1;
#else
	return link(path, linkname);
#endif
}

/* fext
 * return file extension or NULL on error 
 * %filename - name or path of file */
const char * fext(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename)
		return "";
	dot += 1;
	return dot;
}

/* fname
 * return allocated string with file name without 
 * extension and path
 * %path - name or path of file */
char * fname(char *filename)
{
	char *p = strdup(basename(filename));
	char *s = strrchr(p, '.');
	if (s)
		*s = 0;
	return p;
}

/* dname
 * return allocated string with name of 
 * directory path (like POSIX dirname())
 * %path - path of file */
char * dname(const char *path) {
	char *p = strdup(path);
	char *s = strrchr(p, _SLASH_[0]);
	if (s)
		*s = 0;
	else 
		strcpy(p, ".");
	return p;
}

/* fcopy 
 * copy and overwrite file 
 * return 0 on success
 * %from - filepath source file
 * %to   - filepath dastination file */
int fcopy (const char *from, const char *to) {
	FILE *src, *dst;
	char buf[BUFSIZ];
	int err = 0;

	/* open source file */
	if ((src = fopen(from, "rb")) == NULL)
		return -1;

	/*open destination file*/
	if ((dst = fopen(to, "wb"))   == NULL){
		fclose(src);
		return -1;
	}
			
	/*do copy*/
	while (feof(src) == 0){
		fread (buf, sizeof(buf), 1, src);
		fwrite(buf, sizeof(buf), 1, dst);
	}

	/*check errors - to handle error call errno()*/
	if ((err = ferror(src)) || 
			(err = ferror(dst))){}

	/*close*/
	fclose(src);
	fclose(dst);

	return err;
}

/* dcopy 
 * copy directory recursive
 * return 0 on success
 * %from - filepath source file
 * %to   - filepath dastination file 
 * %overwrite - overwrite destination file if true */
int dcopy(const char *from, const char *to, bool overwrite)
{
	/*create `to` directory*/
	if (!fexists(to))
		newdir(to, 0755);

	do {
		dir_foreach(from, file)
		{
			char src[BUFSIZ], dst[BUFSIZ];
			/*skip error*/
			if (strcmp(file->d_name, "") == 0)
				continue;

			/*skip system dir names*/
			if (strcmp(file->d_name, ".") == 0 ||
					strcmp(file->d_name, "..") == 0)
				continue;
			
			sprintf(src, "%s" _SLASH_ "%s", from, file->d_name);
			sprintf(dst, "%s" _SLASH_ "%s", to,   file->d_name);

			/*handle links*/
			if (islink(src)){
				if (!fexists(dst) || overwrite)
					fcopy(src, dst);
				continue;
			}

			/*parse directory*/
			if (isdir(src)){
				dcopy(src, dst, overwrite);
				continue;
			}
				
			/*copy file*/
			if (!fexists(dst) || overwrite)
				fcopy(src, dst);
		}
	} while (0);

	return 0;
}

/* newdir
 * create new directory
 * link to mkdir function with universal
 * for unix/windows args
 * %path - directory path with name
 * %mode - access mode (not used in windows) */
int newdir(const char *path, int mode)
{
#ifdef _WIN32
	return CreateDirectory(path, NULL);
#else
	umask(0);
	return mkdir(path, mode);
#endif
}

/* dcopyf
 * copy directory content with comma-separated filters 
 * applied
 * return 0 on success
 * %from - filepath source file
 * %to   - filepath dastination file 
 * %overwrite - overwrite destination file if true
 * %filters - coma-separated filters like '*' or '*.ext' */
int dcopyf(
		const char *from, const char *to, bool overwrite, 
		char *filters) 
{
	int err = 0;

	do {
		dir_foreach(from, e)
		{	
			char src[BUFSIZ], dst[BUFSIZ], *f, *t;
			/*drop errors */
			if (e->d_name[0] == 0)
				continue;
			
			/*drop '.' and '..'*/
			if (strcmp(e->d_name, ".")  == 0 ||
					strcmp(e->d_name, "..") == 0 )
				continue;

			/*copy names*/
			sprintf(src, "%s" _SLASH_ "%s", from, e->d_name);
			sprintf(dst, "%s" _SLASH_ "%s", to, e->d_name);

			/*cycle through filters*/
			f = strdup(filters);
			for (t=strtok(f, ", ");
					 t; 
					 t=strtok(NULL, ", "))
			{
				/*check is dir			*/
				if (isdir(src)){
					char *dn = dname(t);
					/*copy if directory name matches*/
					if (dn && strcmp(e->d_name, dn) == 0){
						char *error = NULL;
						if ((err = dcopy(src, dst, overwrite)))
							return err;
					}
					free(dn);
				} else {
					/*this is file*/
					/*if file name is "*" - copy if extension matches*/
					/*oterwise copy if filename and extension matches */
					const char *ext = fext(t);
					char *name = fname(t);
					if (name[0] == '*'){
						/*copy file if extension matches*/
						if (strcmp(ext, 
									fext(e->d_name)) == 0)
							if ((err = fcopy(src, dst)))
								return err;
					} else {
						/*copy if name and extension matches*/
						if (strcmp(t, e->d_name) == 0)
							if ((err = fcopy(src, dst)))
								return err;
					}
					free(name);
				}
			}
			if (f) free(f);
		}
	} while (0);

	return 0;
}

#ifdef _WIN32
/* scandir support */
int win_find_data_to_dirent(
		WIN32_FIND_DATA *findData,
		struct dirent *entry)
{
	strncpy(entry->d_name, (const char *)findData->cFileName, 
			sizeof(entry->d_name) - 1);
	entry->d_name[sizeof(entry->d_name)-1] = 0;

	return 1;
}

#define ISDIGIT(a) isdigit(a)

/* states: S_N: normal, S_I: comparing integral part, S_F: comparing
           fractional parts, S_Z: idem but with leading Zeroes only */
#define  S_N    0x0
#define  S_I    0x4
#define  S_F    0x8
#define  S_Z    0xC

/* result_type: CMP: return diff; LEN: compare using len_diff/diff */
#define  CMP    2
#define  LEN    3

/* Compare S1 and S2 as strings holding indices/version numbers,
   returning less than, equal to or greater than zero if S1 is less than,
   equal to or greater than S2 (for more info, see the Glibc texinfo doc).  */

static int
strverscmp (const char *s1, const char *s2)
{
  const unsigned char *p1 = (const unsigned char *) s1;
  const unsigned char *p2 = (const unsigned char *) s2;
  unsigned char c1, c2;
  int state;
  int diff;

  /* Symbol(s)    0       [1-9]   others  (padding)
     Transition   (10) 0  (01) d  (00) x  (11) -   */
  static const unsigned int next_state[] =
    {
      /* state    x    d    0    - */
      /* S_N */  S_N, S_I, S_Z, S_N,
      /* S_I */  S_N, S_I, S_I, S_I,
      /* S_F */  S_N, S_F, S_F, S_F,
      /* S_Z */  S_N, S_F, S_Z, S_Z
    };

  static const int result_type[] =
    {
      /* state   x/x  x/d  x/0  x/-  d/x  d/d  d/0  d/-
                 0/x  0/d  0/0  0/-  -/x  -/d  -/0  -/- */

      /* S_N */  CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
                 CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
      /* S_I */  CMP, -1,  -1,  CMP, +1,  LEN, LEN, CMP,
                 +1,  LEN, LEN, CMP, CMP, CMP, CMP, CMP,
      /* S_F */  CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
                 CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
      /* S_Z */  CMP, +1,  +1,  CMP, -1,  CMP, CMP, CMP,
                 -1,  CMP, CMP, CMP
    };

  if (p1 == p2)
    return 0;

  c1 = *p1++;
  c2 = *p2++;
  /* Hint: '0' is a digit too.  */
  state = S_N | ((c1 == '0') + (ISDIGIT (c1) != 0));

  while ((diff = c1 - c2) == 0 && c1 != '\0')
    {
      state = next_state[state];
      c1 = *p1++;
      c2 = *p2++;
      state |= (c1 == '0') + (ISDIGIT (c1) != 0);
    }

  state = result_type[state << 2 | (((c2 == '0') + (ISDIGIT (c2) != 0)))];

  switch (state)
    {
    case CMP:
      return diff;

    case LEN:
      while (ISDIGIT (*p1++))
        if (!ISDIGIT (*p2++))
          return 1;

      return ISDIGIT (*p2) ? -1 : diff;

    default:
      return state;
    }
}

int alphasort(
		const struct dirent **a, const struct dirent **b)
{
	return strcoll((*a)->d_name, (*b)->d_name);
}

int versionsort(
	const struct dirent **a, const struct dirent **b)
{
	return strverscmp((*a)->d_name, (*b)->d_name);
}

int 
scandir(
		 const char * dirp,
		 struct dirent *** namelist,
		 int (*filter)(const struct dirent *),
		 int (*compar)(
					const void *, 
					const void *))
{
	struct dirent **array = NULL;
	int len = 0;

	do {
		dir_foreach(dirp, file)
		{
			struct dirent *entry = 
				(struct dirent *)malloc(sizeof(struct dirent));
			if (!entry)
				return -1;
			
			*entry = *file;
			
			if (filter && filter(entry) == 0){
				free(entry);
			} else {
				// add to array
				array = 
					(struct dirent **)realloc(array, 
							sizeof(array) * len + sizeof(array));
				if (!array)
					return -1;

				array[len++] = entry; 
			}
		}
	} while(0);

	// sort array
	if (compar)
		qsort(array, len, sizeof(struct dirent *), compar);

	if (namelist)
		*namelist = array;
	else{
		int i;
		for (i = 0; i < len; ++i)
			free(array[i]);
		free(array);
	}

	return len;
}
#endif 

/* get bundle */
char *execdir(const char *path) {
  if (!path)
    return NULL;
#ifdef _WIN32
  return dirname((char *)path);
#else /* _WIN32 */
#ifdef __APPLE__
  return dirname((char *)path);
#else /* __APPLE__ */
  char selfpath[128];
  if (readlink("/proc/self/exe", selfpath, sizeof(selfpath) - 1) < 0) {
    return NULL;
  }
  return dirname(selfpath);
#endif /* __APPLE__ */
#endif /* _WIN32 */
  return NULL;
}
