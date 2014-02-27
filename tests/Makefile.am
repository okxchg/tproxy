TESTS = run_tests
check_PROGRAMS = run_tests
run_tests_SOURCES = run_tests.c check_uri.c check_http.c check_log.c common.c common.h \
					$(top_builddir)/src/http.h $(top_builddir)/src/http.c \
					$(top_builddir)/src/uri.h $(top_builddir)/src/uri.c \
					$(top_builddir)/src/xmalloc.c $(top_builddir)/src/xmalloc.h \
					$(top_builddir)/src/pio.c $(top_builddir)/src/pio.h \
					${top_builddir}/src/log.c ${top_builddir}/src/log.h
run_tests_CFLAGS = @CHECK_CFLAGS@ -ggdb -Wall
run_tests_LDADD = @CHECK_LIBS@ -lrt
