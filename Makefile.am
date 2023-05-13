#config to create static prozubi lib with automake

SUBDIRS = kdata2

noinst_LIBRARIES = libprozubilib.a

if WINDOWS
if X86_64
		CURL_LIBDIR = -Lkdata2/cYandexDisk/curl/windows/win64/libs
endif
if I386
		CURL_LIBDIR = -Lkdata2/cYandexDisk/curl/windows/win32/libs
endif
else
	SQLITE3_LINK = -lsqlite3
endif


libprozubilib_a_SOURCES = \
	prozubilib.c \
	date-time/strftime.c\
	date-time/strptime.c

libprozubilib_a_LIBADD = kdata2/*.o kdata2/cYandexDisk/*.o $(SQLITE3_LINK) -lcurl $(CURL_LIBDIR)
