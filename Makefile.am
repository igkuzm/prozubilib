#config to create static prozubi lib with automake

SUBDIRS = kdata2

lib_LTLIBRARIES = libprozubilib.la

if WIN64
CURL_LIBDIR = -Lkdata2/cYandexDisk/curl/windows/win64/lib
endif

if WIN32
CURL_LIBDIR = -Lkdata2/cYandexDisk/curl/windows/win32/lib
endif

if !WINDOWS
SQLITE3_LINK = -lsqlite3
endif


libprozubilib_la_SOURCES = \
	prozubilib.c \
	date-time/strftime.c\
	date-time/strptime.c

libprozubilib_la_LIBADD =  $(SQLITE3_LINK) -lcurl $(CURL_LIBDIR)
