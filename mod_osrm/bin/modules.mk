mod_osrm.la: client.slo unix-client.slo mod_osrm.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_osrm.lo unix-client.lo client.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_osrm.la