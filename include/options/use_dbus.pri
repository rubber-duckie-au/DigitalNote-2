contains(USE_DBUS, 1) {
    message(Building with DBUS (Freedesktop notifications) support)
    DEFINES += USE_DBUS
    QT += dbus
}
