# pretty print
ifndef V
QUIET_VALAC = @echo '   VALAC      '$^;
QUIET_VALAC_OPTS = --quiet
endif

VALAC ?= $(shell which valac)

VALA_SOURCES = test.vala
VALAC_FLAGS = --vapidir=. --pkg=gopal

targets += test.stamp

test.stamp: $(VALA_SOURCES)
	$(QUIET_VALAC)$(VALAC) $(QUIET_VALAC_OPTS) --ccode $(VALAC_FLAGS) $^
