# use: qmake "USE_UPNP=1" ( enabled by default; default)
#  or: qmake "USE_UPNP=0" (disabled by default)
#  or: qmake "USE_UPNP=-" (not supported)
# miniupnpc (http://miniupnp.free.fr/files/) must be installed for support

contains(USE_UPNP, -) {
	message(Building without UPNP support)
} else {
	win32 {
		exists($${DIGITALNOTE_LIB_MINIUPNP_DIR}/libminiupnpc.a) {
			message("found MiniUPNP lib")
		} else {
			message("You need to compile lib MiniUPNP yourself.")
			message("Also you need to configure the following variables:")
			message("	DIGITALNOTE_LIB_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs")
			message("	DIGITALNOTE_LIB_MINIUPNP_NAME = miniupnpc-2.1")
		}
		
		QMAKE_LIBDIR += $${DIGITALNOTE_LIB_MINIUPNP_DIR}
		
		INCLUDEPATH += $${DIGITALNOTE_LIB_MINIUPNP_DIR}/../
		DEPENDPATH += $${DIGITALNOTE_LIB_MINIUPNP_DIR}/../
		
		LIBS += -liphlpapi
	}
	
	macx {
		QMAKE_LIBDIR += $${DIGITALNOTE_LIB_MINIUPNP_DIR}/lib
		INCLUDEPATH += $${DIGITALNOTE_LIB_MINIUPNP_DIR}/include
		DEPENDPATH += $${DIGITALNOTE_LIB_MINIUPNP_DIR}/include
	}
	
	LIBS += -lminiupnpc
	
	DEFINES += MINIUPNP_STATICLIB
	DEFINES += USE_UPNP
}