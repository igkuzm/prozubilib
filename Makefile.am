#config to create static kdata2 lib with automake

SUBDIRS = kdata2

noinst_LIBRARIES = libprozubilib.a

libprozubilib_a_SOURCES = \
	prozubilib.c \
	date-time/strftime.c\
	date-time/strptime.c

libprozubilib_a_LIBADD = kdata2/libkdata2.a
