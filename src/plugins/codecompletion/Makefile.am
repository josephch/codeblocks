SUBDIRS = resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libcodecompletion.la

libcodecompletion_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libcodecompletion_la_LIBADD = ../../sdk/libcodeblocks.la\
			$(WX_LIBS)

if ! CODEBLOCKS_NT
libcodecompletion_la_LIBADD += $(CB_TINYXML_LIBS)
endif

libcodecompletion_la_SOURCES = ccoptionsdlg.cpp \
			ccoptionsprjdlg.cpp \
			cctreectrl.cpp \
			classbrowser.cpp \
			classbrowserbuilderthread.cpp \
			codecompletion.cpp \
			coderefactoring.cpp \
			doxygen_parser.cpp \
			gotofunctiondlg.cpp \
			insertclassmethoddlg.cpp \
			parsemanager.cpp \
			parsemanager_base.cpp \
			selectincludefile.cpp \
			systemheadersthread.cpp \
			parser/ccdebuginfo.cpp \
			parser/cclogger.cpp \
			parser/parser.cpp \
			parser/parser_base.cpp \
			parser/parserthread.cpp \
			parser/parserthreadedtask.cpp \
			parser/profiletimer.cpp \
			parser/token.cpp \
			parser/tokenizer.cpp \
			parser/tokentree.cpp \
			parser/searchtree.cpp \
			parser/expression.cpp

noinst_HEADERS = ccoptionsdlg.h \
			ccoptionsprjdlg.h \
			cctreectrl.h \
			classbrowser.h \
			classbrowserbuilderthread.h \
			codecompletion.h \
			coderefactoring.h \
			doxygen_parser.h \
			gotofunctiondlg.h \
			insertclassmethoddlg.h \
			parsemanager.h \
			parsemanager_base.h \
			selectincludefile.h \
			systemheadersthread.h \
			parser/ccdebuginfo.h \
			parser/cclogger.h \
			parser/parser.h \
			parser/parser_base.h \
			parser/parserthread.h \
			parser/parserthreadedtask.h \
			parser/profiletimer.h \
			parser/token.h \
			parser/tokenizer.h \
			parser/tokentree.h \
			parser/searchtree.h \
			parser/expression.h

EXTRA_DIST = $(srcdir)/*.cbp \
		cctest \
		testing \
		wxsmith
