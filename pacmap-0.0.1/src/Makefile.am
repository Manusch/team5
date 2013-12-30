
bin_PROGRAMS = pacmap

BUILT_SOURCES=main.x

# libconfigfiles_la_SOURCES = grammar.yy  lexer.ll  parser.h  token.h lexer.h config_creator.h configfile.h configfile.cpp
pacmap_SOURCES = 	main.cpp \
					osm.cpp osm.h \
					parse.cpp parse.h \
					cmdline.cpp cmdline.h \
					rendering.cpp rendering.h \
					objloader.cpp objloader.h \
					drawelement.cpp drawelement.h \
					wall-timer.h

# for bison/flex
# AM_CPPFLAGS = -Isrc/
# AM_YFLAGS = -d -p__yacc_prefix__
# AM_LFLAGS = -P__lex_prefix__ -o lex.yy.c

#library_includedir = $(includedir)/$(PACKAGE)-$(VERSION)
#library_include_HEADERS = osmtest.h

# setting c++ flags:
# AM_CXXFLAGS = -Wall -ansi -pedantic -ggdb -pg

snarf_opts = $(AM_CPPFLAGS) $(CGL_CFLAGS) $(CCGL_CFLAGS) $(CPPFLAGS)
SUFFIXES: .x
.cpp.x:
	guile-snarf -o $@ $< $(snarf_opts)

# setting c preprocessor flags for main project
pacmap_CPPFLAGS = -D PACKAGE=\"$(PACKAGE)\" -D VERSION=\"$(VERSION)\" $(CGL_CFLAGS) $(CCGL_CFLAGS)
pacmap_LDADD = $(CGL_LIBS) $(CCGL_LIBS)

