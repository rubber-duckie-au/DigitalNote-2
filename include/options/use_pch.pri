contains(USE_PCH, 1) {
	CONFIG += precompile_header

	## Use Precompiled headers (PCH)
	PRECOMPILED_HEADER += src/all_headers.h

	precompile_header:!isEmpty(PRECOMPILED_HEADER) {
		DEFINES += USING_PCH
	}
}
