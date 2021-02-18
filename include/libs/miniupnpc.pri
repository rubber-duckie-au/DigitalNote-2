# use: qmake "USE_UPNP=1" ( enabled by default; default)
#  or: qmake "USE_UPNP=0" (disabled by default)
#  or: qmake "USE_UPNP=-" (not supported)
# miniupnpc (http://miniupnp.free.fr/files/) must be installed for support

contains(USE_UPNP, -) {
	message(Building without UPNP support)
} else {
	DEFINES += MINIUPNP_STATICLIB
	DEFINES += USE_UPNP
	LIBS += -lminiupnpc
	
	win32 {
		LIB_PATH = $${DIGITALNOTE_LIB_DIR}/$${DIGITALNOTE_LIB_MINIUPNP_NAME}
		
		exists($${LIB_PATH}/libminiupnpc.a) {
			message("found MiniUPNP lib")
		} else {
			message("You need to compile lib MiniUPNP yourself.")
			message("Also you need to configure the following variables:")
			message("	DIGITALNOTE_LIB_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs")
			message("	DIGITALNOTE_LIB_MINIUPNP_NAME = miniupnpc")
		}
		
		QMAKE_LIBDIR += $${LIB_PATH}
		INCLUDEPATH += $${LIB_PATH}/../
		LIBS += -liphlpapi
	}
}