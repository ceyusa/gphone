# pretty print
ifndef V
QUIET_VALAC = @echo '   VALAC      '$^;
QUIET_VALAC_OPTS = --quiet
endif

VALAC ?= $(shell which valac)

VALA_SOURCES = phone.vala
VALAC_FLAGS = --vapidir=. --pkg=gopal --pkg=posix

targets += vala.stamp

vala.stamp: $(VALA_SOURCES)
	$(QUIET_VALAC)$(VALAC) $(QUIET_VALAC_OPTS) --ccode $(VALAC_FLAGS) $^
