SOURCES = ceh.cpp comm.cpp join.cpp main.cpp

lib_LTLIBRARIES = libnxcc.la

libnxcc_la_SOURCES = $(SOURCES)
libnxcc_la_CPPFLAGS=-I@top_srcdir@/include -DLIBNXCC_EXPORTS
libnxcc_la_LDFLAGS = -release $(NETXMS_LIBRARY_VERSION)
libnxcc_la_LIBADD = @top_srcdir@/src/libnetxms/libnetxms.la

EXTRA_DIST = \
	libnxcc.vcxproj libnxcc.vcxproj.filters \
	libnxcc.h
