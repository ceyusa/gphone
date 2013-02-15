CXXFLAGS := -O0 -ggdb -Wall -Wextra -Wno-unused-parameter
CFLAGS := $(CXXFLAGS)

override CFLAGS += -Wmissing-prototypes -ansi -std=gnu99 -D_GNU_SOURCE

GOPAL_CFLAGS := $(shell pkg-config --cflags opal gio-2.0)
GOPAL_LIBS := $(shell pkg-config --libs opal gio-2.0 gstreamer-app-1.0)

GST_CFLAGS := $(shell pkg-config --cflags gstreamer-app-1.0)

GPHONE_CFLAGS := $(shell pkg-config --cflags gtk+-3.0 gio-2.0 \
	gstreamer-1.0 libnotify sqlite3 libcanberra)
GPHONE_LIBS := $(shell pkg-config --libs gtk+-3.0 gio-2.0 \
	gstreamer-1.0 libnotify sqlite3 libcanberra)

all:

version := $(shell ./get-version)

libgopal_headers := gopalmanager.h gopal.h gopalsipep.h gopalpcssep.h
libgopal_sources := gopalmanager.cpp gopal.cpp gopalsipep.cpp	\
	gopalpcssep.cpp soundgst.cpp $(libgopal_headers)

libgopal_plugins := mmbackend.h mmbackend.c

libgopal.so: gopalenum.o \
	$(patsubst %.cpp, %.o, $(filter %.cpp, $(libgopal_sources))) \
	$(patsubst %.c, %.o, $(filter %.c, $(libgopal_plugins)))
libgopal.so: override CXXFLAGS += $(GOPAL_CFLAGS)
libgopal.so: override CFLAGS += $(GST_CFLAGS)
libgopal.so: override LIBS += $(GOPAL_LIBS)
targets += libgopal.so

gphone_sources := model.vala view.vala account.vala controller.vala main.vala \
	history.vala sounds.vala actions.vala widgets.vala config.vala

gphone_genfiles := $(patsubst %.vala, %.c, $(gphone_sources)) resources.c

gphone: $(patsubst %.c, %.o, $(gphone_genfiles))
gphone: override CFLAGS += $(GPHONE_CFLAGS) -I. -DG_LOG_DOMAIN=\"GPhone\" -DGETTEXT_PACKAGE=\"gphone\"
gphone: override LIBS += $(GPHONE_LIBS) -lgopal -L.
bins += gphone

-include gir.make
-include vala.make

all: $(targets) $(bins)

D = $(DESTDIR)

# pretty print
ifndef V
QUIET_CC    = @echo '   CC         '$@;
QUIET_CXX   = @echo '   CXX        '$@;
QUIET_LINK  = @echo '   LINK       '$@;
QUIET_CLEAN = @echo '   CLEAN      '$@;
QUIET_GEN   = @echo '   GEN        '$@;
endif

resources.c: gresource.xml resources/gphone-ui.xml
	$(QUIET_GEN)glib-compile-resources \
		--target=$@ \
		--sourcedir=./resources \
		--generate-source \
		--c-name gphone \
		gresource.xml

gopalenum.h: $(libgopal_headers)
	$(QUIET_GEN)glib-mkenums 				\
		--fhead "#ifndef __ENUM_TYPES_H__\n"		\
		--fhead "#define __ENUM_TYPES_H__\n\n"		\
		--fhead "#include <glib-object.h>\n\n"		\
		--fhead "G_BEGIN_DECLS\n\n"			\
		--ftail "G_END_DECLS\n\n"			\
		--ftail "#endif /* __ENUM_TYPES_H__ */\n"	\
		--fprod "\n/* --- @filename@ --- */"		\
		--eprod "#define GOPAL_TYPE_@ENUMSHORT@ @enum_name@_get_type()\n" \
		--eprod "GType @enum_name@_get_type (void) G_GNUC_CONST;\n"	\
		$^ > $@

gopalenum.c: $(libgopal_headers) gopalenum.h
	$(QUIET_GEN)glib-mkenums 					\
		--fhead "#include \"$*.h\"\n\n"				\
		--fprod "\n/* enumerations from \"@filename@\" */"	\
		--fprod "\n#include \"@filename@\"\n"			\
		--vhead "GType\n"					\
		--vhead "@enum_name@_get_type (void)\n"			\
		--vhead "{\n"						\
		--vhead "  static volatile gsize __type = 0;\n\n"	\
		--vhead "  if (g_once_init_enter (&__type)) {\n"	\
		--vhead "    static const G@Type@Value values[] = {"  \
		--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
		--vtail "      { 0, NULL, NULL }\n"			\
		--vtail "    };\n\n"					\
		--vtail "    GType type = g_@type@_register_static (g_intern_static_string (\"@EnumName@\"), values);\n"	\
		--vtail "    g_once_init_leave (&__type, type);\n"	\
		--vtail "  }\n\n" 					\
		--vtail "  return __type;\n" 				\
		--vtail "}\n\n" 					\
		$^ > $@

%.so: override CFLAGS += -fPIC
%.so: override CXXFLAGS += -fPIC

%.o:: %.c
	$(QUIET_CC)$(CC) $(CFLAGS) $(INCLUDES) -MMD -o $@ -c $<

%.o:: %.cpp
	$(QUIET_CXX)$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -o $@ -c $<

$(bins):
	$(QUIET_LINK)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.so::
	$(QUIET_LINK)$(CC) $(LDFLAGS) -shared $^ $(LIBS) -o $@

clean:
	$(QUIET_CLEAN)$(RM) $(targets) $(bins) *.o *.d *.gir *.typelib .stamp $(gphone_genfiles) gopalenum.* gopal.vapi

dist: base := gphone-$(version)
dist:
	git archive --format=tar --prefix=$(base)/ HEAD > /tmp/$(base).tar
	mkdir -p $(base)
	echo $(version) > $(base)/.version
	chmod 664 $(base)/.version
	tar --append -f /tmp/$(base).tar --owner root --group root $(base)/.version
	rm -r $(base)
	gzip /tmp/$(base).tar

-include *.d

