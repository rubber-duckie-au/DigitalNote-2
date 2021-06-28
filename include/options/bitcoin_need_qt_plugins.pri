contains(BITCOIN_NEED_QT_PLUGINS, 1) {
    DEFINES += BITCOIN_NEED_QT_PLUGINS
	
    QTPLUGIN += qcncodecs
	QTPLUGIN += qjpcodecs
	QTPLUGIN += qtwcodecs
	QTPLUGIN += qkrcodecs
	QTPLUGIN += qtaccessiblewidgets
}
