# use: qmake "USE_QRCODE=1"
# libqrencode (http://fukuchi.org/works/qrencode/index.en.html) must be installed for support

contains(USE_QRCODE, 1) {
    message(Building with QRCode support)
	
    DEFINES += USE_QRCODE
    LIBS += -lqrencode
	HEADERS += src/qt/qrcodedialog.h
	SOURCES += src/qt/qrcodedialog.cpp
	FORMS += src/qt/forms/qrcodedialog.ui
}
