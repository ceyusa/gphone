# pretty print
ifndef V
QUIET_VALAC = @echo '   VALAC      '$^;
QUIET_VALAC_OPTS = --quiet
else
QUIET_VALAC_OPTS = -v
endif

VALAC ?= $(shell which valac)

PKGS=--pkg=gopal \
	--pkg=posix \
	--pkg=gtk+-3.0 \
	--pkg=gstreamer-1.0 \
	--pkg=libnotify \
	--pkg=sqlite3 \
	--pkg=gio-2.0 \
	--pkg=libcanberra

VALAC_FLAGS = --vapidir=. $(PKGS)

ifdef OLDVALA
VALAC_FLAGS += --vapidir=vapi
endif

$(patsubst %.vala, %.c, $(gphone_sources)): gopal.vapi .stamp

.stamp: $(gphone_sources)
	$(QUIET_VALAC)$(VALAC) $(QUIET_VALAC_OPTS) --ccode $(VALAC_FLAGS) $^
	@touch $@
