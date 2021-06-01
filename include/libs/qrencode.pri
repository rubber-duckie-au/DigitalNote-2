# use: qmake "USE_QRCODE=1"
contains(USE_QRCODE, 1) {
    message(Building with QRCode support)
    
	win32 {
		exists($${DIGITALNOTE_LIB_QRENCODE_DIR}/.libs/libqrencode.a) {
			message("found QREncode lib.")
		} else {
			message("You need to compile lib QREncode yourself with msys2.")
			message("Also you need to configure the following variables:")
			message("	DIGITALNOTE_LIB_QRENCODE_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs/qrencode-4.1.1")
		}
		
		QMAKE_LIBDIR += $${DIGITALNOTE_LIB_QRENCODE_DIR}/.libs
		INCLUDEPATH += $${DIGITALNOTE_LIB_QRENCODE_DIR}
		DEPENDPATH += $${DIGITALNOTE_LIB_QRENCODE_DIR}
	}
	
	macx {
		QMAKE_LIBDIR += $${DIGITALNOTE_LIB_QRENCODE_DIR}/lib
		INCLUDEPATH += $${DIGITALNOTE_LIB_QRENCODE_DIR}/include
		DEPENDPATH += $${DIGITALNOTE_LIB_QRENCODE_DIR}/include
	}
	
	DEFINES += USE_QRCODE
    
	LIBS += -lqrencode
	
	HEADERS += src/qt/qrcodedialog.h
	SOURCES += src/qt/qrcodedialog.cpp
	FORMS += src/qt/forms/qrcodedialog.ui
}