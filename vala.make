# pretty print
ifndef V
QUIET_VALAC = @echo '   VALAC      '$^;
QUIET_VALAC_OPTS = --quiet
endif

VALAC ?= $(shell which valac)

PKGS=--pkg=gopal --pkg=posix --pkg=gtk+-3.0 --pkg=gstreamer-1.0 --pkg=libnotify
VALAC_FLAGS = --vapidir=. $(PKGS)

$(patsubst %.vala, %.c, $(phone_sources)): gopal.vapi vala.stamp

vala.stamp: $(phone_sources)
	$(QUIET_VALAC)$(VALAC) $(QUIET_VALAC_OPTS) --ccode $(VALAC_FLAGS) $^
	@touch vala.stamp
