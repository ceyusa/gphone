CXXFLAGS := -O0 -ggdb -Wall -Wextra -Wno-unused-parameter
CFLAGS := $(CXXFLAGS)

override CFLAGS += -Wmissing-prototypes -ansi -std=gnu99 -D_GNU_SOURCE

OPAL_CFLAGS := $(shell pkg-config --cflags opal)
OPAL_LIBS := $(shell pkg-config --libs opal)

G_CFLAGS := $(shell pkg-config --cflags gobject-2.0)
G_LIBS := $(shell pkg-config --libs gobject-2.0)

all:

libgopal_sources := gopalmanager.h gopalmanager.cpp gopal.h gopal.cpp \
	gopalsipep.h gopalsipep.cpp

libgopal.so: gopalmanager.o gopal.o gopalsipep.o
libgopal.so: override CXXFLAGS += $(OPAL_CFLAGS) $(G_CFLAGS)
libgopal.so: override CFLAGS += $(G_CFLAGS)
libgopal.so: override LIBS += $(OPAL_LIBS) $(G_LIBS)
targets += libgopal.so

phone: main.o
phone: override CFLAGS += $(G_CFLAGS)
phone: override LIBS += $(G_LIBS) -lgopal -L ./
bins += phone

all: $(targets) $(bins) gopal.vapi

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
	$(QUIET_CLEAN)$(RM) -v $(targets) $(bins) *.o *.d *.gir *.typelib *.vapi

-include *.d

-include gir.make
