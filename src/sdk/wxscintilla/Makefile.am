AM_CPPFLAGS = $(WX_CXXFLAGS) \
			$(SCINTILLA_GTK_CFLAGS) \
			-I$(srcdir)/include \
			-DSCI_LEXER \
			-DLINK_LEXERS \
			-DGTK \
			-D__WX__

if CODEBLOCKS_NT
AM_CPPFLAGS += -DWXMAKINGDLL_SCI
endif

noinst_LTLIBRARIES = libwxscintilla.la

libwxscintilla_la_SOURCES = src/wxscintilla.cpp

sdkdir=$(includedir)/codeblocks/wxscintilla/include/wx

sdk_HEADERS = include/wx/wxscintilla.h

EXTRA_DIST = $(srcdir)/Readme \
	$(srcdir)/build/*.txt \
	$(srcdir)/build/Makefile \
	$(srcdir)/build/wxscintilla.*

# add upper-case copy
CLEANFILES = README
README: $(srcdir)/Readme
	cp -p $< $@
