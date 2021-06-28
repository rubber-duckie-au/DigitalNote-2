!equals(UNITY_BUILD_DISABLE, "1") {
	UNITY_SCRIPTS_DIR = $$clean_path($${PWD})
	UNITY_DIR = $$OUT_PWD/unity/
	
	# If you have many projects, you might want to put unity and other tmp files outside of the hierarchy. (out of source builds)
	# To do this, uncomment the following lines
	#
	# PROJECTS_ROOT is the path to the "src" directory
	# PROJECTS_ROOT = $$clean_path($${PWD}/../)
	# PROJECT_NAME = $$relative_path($$_PRO_FILE_PWD_, $$PROJECTS_ROOT)
	# OUTPUT_BASE_PATH = $$PROJECTS_ROOT/tmp/Build_Qt/$$PROJECT_NAME
	#
	# CONFIG( debug, debug|release ) {
	#     OUTPUT_BASE_PATH = $$OUTPUT_BASE_PATH/debug
	# } else {
	#     OUTPUT_BASE_PATH = $$OUTPUT_BASE_PATH/release
	# }
	# UNITY_DIR = $$OUTPUT_BASE_PATH/unity/
	#
	# To also put the object files outside, uncomment the following lines
	#
	# OBJECTS_DIR = $$OUTPUT_BASE_PATH/obj/
	# MOC_DIR = $$OUTPUT_BASE_PATH/moc/
	# UI_DIR = $$OUTPUT_BASE_PATH/ui/
	# RCC_DIR = $$OUTPUT_BASE_PATH/rcc/
	# PRECOMPILED_DIR = $$OUTPUT_BASE_PATH/pch/

	!defined(UNITY_MOC_MODE, var) {
		UNITY_MOC_MODE = MOC_LVL_2
	}

	!defined(UNITY_STRATEGY, var) {
		UNITY_STRATEGY = incremental
	}

	defined(UNITY_BUILD, var) {
		!count(SOURCES, 1) {
			mkpath($$UNITY_DIR)
			UNITY_SOURCES_FILE_LOCALVAR = $${UNITY_DIR}unitySources.txt
			write_file($$UNITY_SOURCES_FILE_LOCALVAR, SOURCES)
			system(cd $$_PRO_FILE_PWD_ & python3 $$UNITY_SCRIPTS_DIR/qmake_unity.py --mode update --strategy $$UNITY_STRATEGY --mocMode $$UNITY_MOC_MODE --tmpDir "$$UNITY_DIR" --sourceListPath "$$UNITY_SOURCES_FILE_LOCALVAR")
			UNITY_GENERATED_PRI_LINES = $$cat($${UNITY_DIR}unity.pri, lines)
			for(UNITY_GENERATED_PRI_LINE, UNITY_GENERATED_PRI_LINES) {
				eval($$UNITY_GENERATED_PRI_LINE)
				message($$UNITY_GENERATED_PRI_LINE)
			}
		}

		equals(UNITY_MOC_MODE, MOC_LVL_2) {
			!count(HEADERS, 1) {
				mkpath($$UNITY_DIR)
				UNITY_HEADERS_FILE_LOCALVAR = $${UNITY_DIR}unityHeaders.txt
				write_file($$UNITY_HEADERS_FILE_LOCALVAR, HEADERS)
				system(cd $$_PRO_FILE_PWD_ & python $$UNITY_SCRIPTS_DIR/unity_moc_headers.py --mode generate_groups --headerListPath "$$UNITY_HEADERS_FILE_LOCALVAR")
				UNITY_GENERATED_PRI_LINES = $$cat($${UNITY_DIR}unity_headers.pri, lines)
				for(UNITY_GENERATED_PRI_LINE, UNITY_GENERATED_PRI_LINES) {
					eval($$UNITY_GENERATED_PRI_LINE)
					message($$UNITY_GENERATED_PRI_LINE)
				}
			}
		}
	}
}
else {
    !build_pass:message("UNITY_BUILD_DISABLE is set.")
}
