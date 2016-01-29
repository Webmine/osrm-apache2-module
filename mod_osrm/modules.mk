mod_osrm.la: mod_osrm.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_osrm.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_osrm.la
