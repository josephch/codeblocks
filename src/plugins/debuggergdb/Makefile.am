SUBDIRS = resources

AM_CPPFLAGS = $(WX_CXXFLAGS) \
		$(CB_SQUIRREL_CFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libdebugger.la

libdebugger_la_LDFLAGS = @MODULE_SHARED_LDFLAGS@ -version-info 0:1:0 -no-undefined -avoid-version

libdebugger_la_LIBADD = ../../sdk/libcodeblocks.la \
			$(WX_LIBS) \
			$(WX_GTK_LIBS)

if ! CODEBLOCKS_NT
libdebugger_la_LIBADD += $(CB_TINYXML_LIBS)
endif

libdebugger_la_SOURCES = cdb_driver.cpp \
			databreakpointdlg.cpp \
			debugger_defs.cpp \
			debuggerdriver.cpp \
			debuggergdb.cpp \
			debuggeroptionsdlg.cpp \
			debuggeroptionsprjdlg.cpp \
			debuggerstate.cpp \
			editbreakpointdlg.cpp \
			editwatchdlg.cpp \
			gdb_driver.cpp \
			parsewatchvalue.cpp

noinst_HEADERS = cdb_commands.h \
			cdb_driver.h \
			databreakpointdlg.h \
			debugger_defs.h \
			debuggerdriver.h \
			debuggergdb.h \
			debuggeroptionsdlg.h \
			debuggeroptionsprjdlg.h \
			debuggerstate.h \
			editbreakpointdlg.h \
			editwatchdlg.h \
			gdb_commands.h \
			gdb_driver.h \
			parsewatchvalue.h \
			remotedebugging.h

EXTRA_DIST = debuggergdb_test_defs.cpp \
		debuggergdb_test_backtrace.cpp \
		debuggergdb_test_common.h \
		debuggergdb_test_main.cpp \
		debuggergdb_test_parser.cpp \
		debuggergdb_test_parser_cdb.cpp \
		$(srcdir)/*.cbp

