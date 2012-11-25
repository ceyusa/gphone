# pretty print
ifndef V
QUIET_VALAC = @echo '   VALAC      '$^;
QUIET_VALAC_OPTS = --quiet --debug
endif

VALAC ?= $(shell which valac)

VALAC_FLAGS = --vapidir=. --pkg=gopal --pkg=posix --pkg=gtk+-3.0

$(patsubst %.vala, %.c, $(phone_sources)): gopal.vapi vala.stamp

vala.stamp: $(phone_sources)
	$(QUIET_VALAC)$(VALAC) $(QUIET_VALAC_OPTS) --ccode $(VALAC_FLAGS) $^
	@touch vala.stamp
