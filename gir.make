# pretty print
ifndef V
QUIET_GIR_SCANNER  = @echo '   GISCAN     '$@;
QUIET_GIR_OPTS     = --quiet
QUIET_GIR_COMPILER = @echo '   GICOMP     '$@;
QUIET_VAPIGEN      = @echo '   VAPIGEN    '$@;
QUIET_VAPIGEN_OPTS = --quiet
endif

GIR_SCANNER ?= $(shell which g-ir-scanner)
GIR_COMPILER ?= $(shell which g-ir-compiler)
VAPIGEN ?= $(shell which vapigen)

Gopal-0.1.gir: override CFLAGS=
Gopal-0.1.gir: override LIBS=
Gopal-0.1.gir: libgopal.so $(libgopal_sources)
	$(QUIET_GIR_SCANNER)$(GIR_SCANNER) $(QUIET_GIR_OPTS) \
	--warn-all 		\
	--add-include-path=.	\
	--include=Gio-2.0	\
	--namespace=Gopal 	\
	--nsversion=0.1		\
	--library=gopal		\
	--c-include "gopal.h"	\
	$^			\
	--output $@

%.typelib: %.gir
	$(QUIET_GIR_COMPILER)$(GIR_COMPILER) \
	--includedir=. $< -o $@

gopal.vapi: Gopal-0.1.gir Gopal-0.1.metadata
	$(QUIET_VAPIGEN)$(VAPIGEN) $(QUIET_VAPIGEN_OPTS) \
	--vapidir=.		\
	--metadatadir=.		\
	--pkg gio-2.0		\
	--library gopal 	\
	$<
