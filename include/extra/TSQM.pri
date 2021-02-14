# automatically build translations, so they can be included in resource file
extra_TSQM.name = lrelease ${QMAKE_FILE_IN}
extra_TSQM.input = TRANSLATIONS
extra_TSQM.output = $$PROJECT_PWD/build/qt/locale/${QMAKE_FILE_BASE}.qm
extra_TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
extra_TSQM.CONFIG = no_link

QMAKE_EXTRA_COMPILERS += extra_TSQM
