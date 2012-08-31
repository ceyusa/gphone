Gopal-0.1.gir: libgopal.so $(libgopal_sources)
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./ \
	g-ir-scanner -v --warn-all 	\
	--add-include-path=./	\
	--include=GObject-2.0	\
	--namespace=Gopal 	\
	--nsversion=0.1		\
	--library=gopal		\
	$^			\
	--output $@
