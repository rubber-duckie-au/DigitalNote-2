# use: qmake "USE_UPNP=1" ( enabled by default; default)
#  or: qmake "USE_UPNP=0" (disabled by default)
#  or: qmake "USE_UPNP=-" (not supported)
# miniupnpc (http://miniupnp.free.fr/files/) must be installed for support

contains(USE_UPNP, -) {
	message(Building without UPNP support)
} else {
	message(Building with UPNP support)
	
	count(USE_UPNP, 0) {
		USE_UPNP=1
	}
	DEFINES += DMINIUPNP_STATICLIB
	INCLUDEPATH += $$MINIUPNPC_INCLUDE_PATH
	LIBS += $$join(MINIUPNPC_LIB_PATH,,-L,) -lminiupnpc
}
