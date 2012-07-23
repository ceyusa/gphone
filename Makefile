CXXFLAGS := -O0 -ggdb -Wall -Wextra -Wno-unused-parameter
CFLAGS := $(CXXFLAGS)

override CFLAGS += -Wmissing-prototypes -ansi -std=gnu99 -D_GNU_SOURCE

OPAL_CFLAGS := $(shell pkg-config --cflags opal)
OPAL_LIBS := $(shell pkg-config --libs opal)

G_CFLAGS := $(shell pkg-config --cflags gobject-2.0)
G_LIBS := $(shell pkg-config --libs gobject-2.0)

all:

phone: main.o gopalmanager.o gopal.o
phone: override CXXFLAGS += $(OPAL_CFLAGS) $(G_CFLAGS)
phone: override CFLAGS += $(G_CFLAGS)
phone: override LIBS += $(OPAL_LIBS) $(G_LIBS)
bins += phone

all: $(bins)

D = $(DESTDIR)

# pretty print
ifndef V
QUIET_CC    = @echo '   CC         '$@;
QUIET_CXX   = @echo '   CXX        '$@;
QUIET_LINK  = @echo '   LINK       '$@;
QUIET_CLEAN = @echo '   CLEAN      '$@;
endif

.PHONY: push

%.o:: %.c
	$(QUIET_CC)$(CC) $(CFLAGS) $(INCLUDES) -MMD -o $@ -c $<

%.o:: %.cpp
	$(QUIET_CXX)$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -o $@ -c $<

$(bins):
	$(QUIET_LINK)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	$(QUIET_CLEAN)$(RM) $(bins) $(gst_plugin) *.o *.d

-include *.d
