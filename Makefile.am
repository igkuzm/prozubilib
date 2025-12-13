#config to create static prozubi lib with automake
#ACLOCAL_AMFLAGS="-I m4"
AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = kdata2

lib_LTLIBRARIES = libprozubilib.la
libprozubilib_la_CFLAGS =
libprozubilib_la_LDFLAGS =

libprozubilib_la_SOURCES = \
	src/bill.c \
	src/cases.c \
	src/diagnisis.c \
	src/doctors.c \
	src/documents.c \
	src/images.c \
	src/mkb.c \
	src/nomenklatura.c \
	src/passport.c \
	src/planlecheniya.c \
	src/prices.c \
	src/template.c \
	src/rtf.c \
	src/prozubilib.c \
	src/rtfutf8tocp1251.c \
	src/fm.c \
	src/text_on_bitmap.c text_on_bitmap.h

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
