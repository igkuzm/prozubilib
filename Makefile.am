#config to create static prozubi lib with automake
#ACLOCAL_AMFLAGS="-I m4"

SUBDIRS = kdata2

lib_LTLIBRARIES = libprozubilib.la

if WIN64
CURL_LIBDIR = -L`pwd`/kdata2/cYandexDisk/curl/windows/win64/lib
endif

if WIN32
CURL_LIBDIR = -L`pwd`/kdata2/cYandexDisk/curl/windows/win32/lib
endif

if WINDOWS
#ADD_OBJECTS = kdata2/*.o kdata2/cYandexDisk/*.o
else
SQLITE3_LINK = -lsqlite3
endif

libprozubilib_la_SOURCES = \
	prozubilib.c \
	text_on_bitmap.c text_on_bitmap.h \
	stb_image.c

libprozubilib_la_LIBADD =  $(SQLITE3_LINK) -lcurl $(CURL_LIBDIR) $(ADD_OBJECTS) -lm
