# pretty print
ifndef V
QUIET_VALAC = @echo '   VALAC      '$^;
QUIET_VALAC_OPTS = --quiet
endif

VALAC ?= $(shell which valac)

PKGS=--pkg=gopal \
	--pkg=posix \
	--pkg=gtk+-3.0 \
	--pkg=gstreamer-1.0 \
	--pkg=libnotify \
	--pkg=sqlite3

VALAC_FLAGS = --vapidir=. $(PKGS)

$(patsubst %.vala, %.c, $(gphone_sources)): gopal.vapi .stamp

.stamp: $(gphone_sources)
	$(QUIET_VALAC)$(VALAC) $(QUIET_VALAC_OPTS) --ccode $(VALAC_FLAGS) $^
	@touch .stamp
