# pretty print
ifndef V
QUIET_GIR_SCANNER  = @echo '   GISCAN     '$@;
QUIET_GIR_OPTS     = --quiet
QUIET_GIR_COMPILER = @echo '   GICOMP     '$@;
QUIET_VAPIGEN      = @echo '   VAPIGEN    '$@;
QUIET_VAPIGEN_OPTS = --quiet
endif

GIR_SCANNER ?= LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:. $(shell which g-ir-scanner)
GIR_COMPILER ?= $(shell which g-ir-compiler)
VAPIGEN ?= $(shell which vapigen)

targets += gopal.vapi

Gopal-0.1.gir: libgopal.so $(libgopal_sources)
	$(QUIET_GIR_SCANNER)$(GIR_SCANNER) $(QUIET_GIR_OPTS) \
	--warn-all 		\
	--add-include-path=.	\
	--include=GObject-2.0	\
	--namespace=Gopal 	\
	--nsversion=0.1		\
	--library=gopal		\
	--c-include "gopal.h"	\
	$^			\
	--output $@

%.typelib: %.gir
	$(QUIET_GIR_COMPILER)$(GIR_COMPILER) \
	--includedir=. $< -o $@

gopal.vapi: Gopal-0.1.gir
	$(QUIET_VAPIGEN)$(VAPIGEN) $(QUIET_VAPIGEN_OPTS) \
	--library gopal $<
