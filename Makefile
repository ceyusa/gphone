CXXFLAGS := -O0 -ggdb -Wall -Wextra -Wno-unused-parameter
CFLAGS := $(CXXFLAGS)

override CFLAGS += -Wmissing-prototypes -ansi -std=gnu99 -D_GNU_SOURCE

OPAL_CFLAGS := $(shell pkg-config --cflags opal)
OPAL_LIBS := $(shell pkg-config --libs opal)

G_CFLAGS := $(shell pkg-config --cflags gobject-2.0)
G_LIBS := $(shell pkg-config --libs gobject-2.0)

GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS := $(shell pkg-config --libs gtk+-3.0)

GST_CFLAGS := $(shell pkg-config --cflags gstreamer-1.0 gstreamer-app-1.0)
GST_LIBS := $(shell pkg-config --libs gstreamer-1.0 gstreamer-app-1.0)

all:

libgopal_sources := gopalmanager.h gopalmanager.cpp gopal.h gopal.cpp \
	gopalsipep.h gopalsipep.cpp gopalpcssep.h gopalpcssep.cpp

phone_sources := model.vala view.vala account.vala main.vala

libgopal.so: $(patsubst %.cpp, %.o, $(filter %.cpp, $(libgopal_sources)))
libgopal.so: override CXXFLAGS += $(OPAL_CFLAGS) $(G_CFLAGS)
libgopal.so: override LIBS += $(OPAL_LIBS) $(G_LIBS)
targets += libgopal.so

phone: $(patsubst %.vala, %.o, $(phone_sources))
phone: override CFLAGS += $(GTK_CFLAGS) $(GST_CFLAGS) -I. -DG_LOG_DOMAIN=\"GPhone\"
phone: override LIBS += $(GTK_LIBS) $(GST_LIBS) -lgopal -L.
bins += phone

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
endif

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
	$(QUIET_CLEAN)$(RM) $(targets) $(bins) *.o *.d *.gir *.typelib *.stamp

-include *.d

