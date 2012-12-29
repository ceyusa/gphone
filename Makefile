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

libgopal_sources := gopalmanager.h gopalmanager.cpp gopal.h gopal.cpp \
	gopalsipep.h gopalsipep.cpp gopalpcssep.h gopalpcssep.cpp \
	soundgst.cpp

libgopal_plugins := mmbackend.h mmbackend.c

libgopal.so: $(patsubst %.cpp, %.o, $(filter %.cpp, $(libgopal_sources))) \
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
	$(QUIET_CLEAN)$(RM) $(targets) $(bins) *.o *.d *.gir *.typelib .stamp $(gphone_genfiles)

-include *.d

