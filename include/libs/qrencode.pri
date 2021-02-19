# use: qmake "USE_QRCODE=1"
contains(USE_QRCODE, 1) {
    message(Building with QRCode support)
    
	win32 {
		LIB_PATH = $${DIGITALNOTE_LIB_DIR}/$${DIGITALNOTE_LIB_QRENCODE_NAME}
		
		exists($${LIB_PATH}/.libs/libqrencode.a) {
			message("found QREncode lib.")
		} else {
			message("You need to compile lib QREncode yourself with msys2.")
			message("Also you need to configure the following variables:")
			message("	DIGITALNOTE_LIB_DIR = $${DOLLAR}$${DOLLAR}DIGITALNOTE_PATH/../libs")
			message("	DIGITALNOTE_LIB_QRENCODE_NAME = qrencode-4.1.1")
		}
		
		QMAKE_LIBDIR += $${LIB_PATH}/.libs
	}
	
	DEFINES += USE_QRCODE
	INCLUDEPATH += $${LIB_PATH}
    LIBS += -lqrencode
	HEADERS += src/qt/qrcodedialog.h
	SOURCES += src/qt/qrcodedialog.cpp
	FORMS += src/qt/forms/qrcodedialog.ui
}