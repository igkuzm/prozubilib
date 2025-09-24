#config to create static prozubi lib with automake
#ACLOCAL_AMFLAGS="-I m4"

SUBDIRS = kdata2

lib_LTLIBRARIES = libprozubilib.la
libprozubilib_la_CFLAGS =
libprozubilib_la_LDFLAGS =

libprozubilib_la_SOURCES = \
	prozubilib.c \
	text_on_bitmap.c text_on_bitmap.h

if WIN64
CURL_LIBDIR = -Lkdata2/cYandexDisk/curl/windows/win64/lib
endif

if WIN32
CURL_LIBDIR = -Lkdata2/cYandexDisk/curl/windows/win32/lib
libprozubilib_la_CFLAGS += \
	-Ikdata2/cYandexDisk/curl/windows/win32/include -Ikdata2/cYandexDisk/pthreads-win32/include
libprozubilib_la_LDFLAGS += -lstbimage -Lwin32
endif

if WINDOWS
#ADD_OBJECTS = kdata2/*.o kdata2/cYandexDisk/*.o
else
SQLITE3_LINK = -lsqlite3
libprozubilib_la_CFLAGS += \
	-Ikdata2/cYandexDisk/curl/windows/win64/include -Ikdata2/cYandexDisk/pthreads-win32/include
libprozubilib_la_SOURCES += stb_image.c
endif

libprozubilib_la_LIBADD =  $(SQLITE3_LINK) -lcurl $(CURL_LIBDIR) $(ADD_OBJECTS) -lm
