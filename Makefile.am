#config to create static prozubi lib with automake
#ACLOCAL_AMFLAGS="-I m4"

SUBDIRS = kdata2

lib_LTLIBRARIES = libprozubilib.la
libprozubilib_la_CFLAGS =

if WIN64
CURL_LIBDIR = -Lkdata2/cYandexDisk/curl/windows/win64/lib
endif

if WIN32
CURL_LIBDIR = -Lkdata2/cYandexDisk/curl/windows/win32/lib
libprozubilib_la_CFLAGS += \
	-IcYandexDisk/curl/windows/win32/include -IcYandexDisk/pthreads-win32/include
endif

if WINDOWS
#ADD_OBJECTS = kdata2/*.o kdata2/cYandexDisk/*.o
else
SQLITE3_LINK = -lsqlite3
libprozubilib_la_CFLAGS += \
	-IcYandexDisk/curl/windows/win64/include -IcYandexDisk/pthreads-win32/include
endif

libprozubilib_la_SOURCES = \
	prozubilib.c \
	text_on_bitmap.c text_on_bitmap.h \
	stb_image.c

libprozubilib_la_LIBADD =  $(SQLITE3_LINK) -lcurl $(CURL_LIBDIR) $(ADD_OBJECTS) -lm
