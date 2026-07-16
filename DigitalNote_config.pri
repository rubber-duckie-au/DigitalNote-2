## Version
DIGITALNOTE_VERSION_MAJOR = 2
DIGITALNOTE_VERSION_MINOR = 0
DIGITALNOTE_VERSION_REVISION = 0
DIGITALNOTE_VERSION_BUILD = 8

## MSYS2 Install Path
MINGW64_PREFIX 						  = $$system(cygpath -m /mingw64)

## Leveldb library
DIGITALNOTE_LEVELDB_PATH              = $${DIGITALNOTE_PATH}/src/leveldb
DIGITALNOTE_LEVELDB_INCLUDE_PATH      = $${DIGITALNOTE_PATH}/src/leveldb/include
DIGITALNOTE_LEVELDB_HELPERS_PATH      = $${DIGITALNOTE_PATH}/src/leveldb/helpers
DIGITALNOTE_LEVELDB_LIB_PATH          = $${DIGITALNOTE_PATH}/src/leveldb

## Secp256K1 library
DIGITALNOTE_SECP256K1_PATH            = $${DIGITALNOTE_PATH}/src/secp256k1
DIGITALNOTE_SECP256K1_INCLUDE_PATH    = $${DIGITALNOTE_PATH}/src/secp256k1/include
DIGITALNOTE_SECP256K1_LIB_PATH        = $${DIGITALNOTE_PATH}/src/secp256k1/.libs

win32 {
	## Boost
	DIGITALNOTE_BOOST_INCLUDE_PATH    = $${DIGITALNOTE_PATH}/../libs/boost_1_91_0/include/boost-1_91
	DIGITALNOTE_BOOST_LIB_PATH        = $${DIGITALNOTE_PATH}/../libs/boost_1_91_0/lib
	## Boost b2 stamps the toolset major version into the static lib filename
	## (versioned-layout): libboost_system-mgw<MAJOR>-mt-s-x64-1_91.a
	## We auto-detect MAJOR from g++ at qmake time so MSYS2 GCC bumps (15->16
	## happened during v2.0.0.7 testing; future bumps will too) don't require
	## editing this file. Falls back to mgw16 if detection fails.
	DIGITALNOTE_GCC_MAJOR             = $$system(g++ -dumpversion 2>NUL)
	DIGITALNOTE_GCC_MAJOR             = $$section(DIGITALNOTE_GCC_MAJOR, ., 0, 0)
	isEmpty(DIGITALNOTE_GCC_MAJOR): DIGITALNOTE_GCC_MAJOR = 16
	DIGITALNOTE_BOOST_SUFFIX          = -mgw$${DIGITALNOTE_GCC_MAJOR}-mt-s-x64-1_91
	message(Boost suffix: $${DIGITALNOTE_BOOST_SUFFIX})
	
	## OpenSSL library
	DIGITALNOTE_OPENSSL_INCLUDE_PATH  = $${DIGITALNOTE_PATH}/../libs/openssl-3.5.7/include
	DIGITALNOTE_OPENSSL_LIB_PATH      = $${DIGITALNOTE_PATH}/../libs/openssl-3.5.7/lib

	## v2.0.9: OpenSSL 3.x may install its static libs to lib64/ instead of lib/
	## (depends on platform/config; 3.5.x on this MinGW toolchain uses lib64).
	## Resolve HERE, at the single source of truth, so BOTH the explicit .a paths
	## in include/libs.pri AND the search path / existence check in
	## include/libs/openssl.pri pick up the correct directory automatically.
	## config.pri is included before libs.pri (see the .pro files), so this
	## reassignment propagates everywhere downstream.
	exists($${DIGITALNOTE_OPENSSL_LIB_PATH}64/libcrypto.a) {
		DIGITALNOTE_OPENSSL_LIB_PATH  = $${DIGITALNOTE_OPENSSL_LIB_PATH}64
	}

	## v2.0.9: build against OpenSSL 3.5.7 (bumped from 1.1.1w).  The wallet still
	## calls raw-EC/BN/ECDSA/RIPEMD APIs (ecwrapper.cpp [SMSG], stealth.cpp,
	## smsg.cpp, cbignum*, hash.cpp) that OpenSSL 3.x marks DEPRECATED but still
	## ships.  OPENSSL_SUPPRESS_DEPRECATED silences the deprecation diagnostics so
	## they don't break -Werror builds; the functions themselves remain available
	## because the library is NOT configured with no-deprecated (see the builder's
	## compile/openssl.sh).  Migrating these call sites to the EVP/OSSL_PARAM 3.x
	## API is deferred future work (see v209-TODO Section 6b / Track B).
	DEFINES += OPENSSL_SUPPRESS_DEPRECATED
	
	## Berkeley db library
	DIGITALNOTE_BDB_INCLUDE_PATH      = $${DIGITALNOTE_PATH}/../libs/db-6.2.32.NC/include
	DIGITALNOTE_BDB_LIB_PATH          = $${DIGITALNOTE_PATH}/../libs/db-6.2.32.NC/lib
	
	## Event library
	DIGITALNOTE_EVENT_INCLUDE_PATH    = $${DIGITALNOTE_PATH}/../libs/libevent-2.1.12-stable/include
	DIGITALNOTE_EVENT_LIB_PATH        = $${DIGITALNOTE_PATH}/../libs/libevent-2.1.12-stable/lib
	
	## GMP library
	DIGITALNOTE_GMP_INCLUDE_PATH      = $${MINGW64_PREFIX}/include
	DIGITALNOTE_GMP_LIB_PATH          = $${MINGW64_PREFIX}/lib
	
	## Miniupnp library
	DIGITALNOTE_MINIUPNP_INCLUDE_PATH = $${DIGITALNOTE_PATH}/../libs/miniupnpc-2.2.8/include
	DIGITALNOTE_MINIUPNP_LIB_PATH     = $${DIGITALNOTE_PATH}/../libs/miniupnpc-2.2.8/lib
	## miniupnpc 2.2.8 = API version 18 — set explicitly since
	## Makefile.mingw does not define MINIUPNPC_API_VERSION correctly on Linux
	DEFINES += MINIUPNPC_API_VERSION=18
	
	## QREncode library
	DIGITALNOTE_QRENCODE_INCLUDE_PATH = $${DIGITALNOTE_PATH}/../libs/qrencode-4.1.1/include
	DIGITALNOTE_QRENCODE_LIB_PATH     = $${DIGITALNOTE_PATH}/../libs/qrencode-4.1.1/lib
	
	## BIP39 (sources compiled directly — no external lib needed)
	DIGITALNOTE_BIP39_INCLUDE_PATH = $${DIGITALNOTE_PATH}/src/bip39/include
	DIGITALNOTE_BIP39_SRC_PATH     = $${DIGITALNOTE_PATH}/src/bip39/src
}

macx {
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.00

	## libc++ on Xcode 15+ / macOS SDK 14+ removed std::unary_function and
	## related C++17-deprecated symbols that Boost 1.83 still references
	## (boost/container_hash/hash.hpp uses std::unary_function). Apple's
	## libc++ provides feature-test macros to keep the deprecated symbols
	## available; we set the broad one which covers unary_function,
	## binary_function, random_shuffle, auto_ptr, etc. Until Boost is
	## upgraded to 1.81+ (which dropped the dependency) this is the
	## supported workaround.
	DEFINES += _LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION
	DEFINES += _LIBCPP_ENABLE_CXX17_REMOVED_FEATURES

	## NB: Clang 14+/15+/16+ also produces fatal warnings inside Boost 1.83
	## headers (-Wenum-constexpr-conversion, -Wdeprecated-builtins,
	## -Wdeprecated-declarations, -Wunused-but-set-variable). Those are
	## suppressed in include/compiler_settings.pri's macx scope so they live
	## next to the other QMAKE_CXXFLAGS_WARN_ON entries — see that file.

	## Boost — built from source by CI's libs job into Builder/macos/<arch>/libs/
	## (formerly Homebrew at /usr/local/Cellar — that path doesn't exist on the
	## arm64 runner, and boost@1.83 is no longer in Homebrew. The libs symlink
	## ${{ github.workspace }}/../libs -> Builder/macos/<arch>/libs makes
	## $${DIGITALNOTE_PATH}/../libs/ resolve correctly here.)
	DIGITALNOTE_BOOST_INCLUDE_PATH    = $${DIGITALNOTE_PATH}/../libs/boost_1_91_0/include
	DIGITALNOTE_BOOST_LIB_PATH        = $${DIGITALNOTE_PATH}/../libs/boost_1_91_0/lib
	DIGITALNOTE_BOOST_SUFFIX          =
	
	## OpenSSL library
	DIGITALNOTE_OPENSSL_INCLUDE_PATH  = $${DIGITALNOTE_PATH}/../libs/openssl-3.5.7/include
	DIGITALNOTE_OPENSSL_LIB_PATH      = $${DIGITALNOTE_PATH}/../libs/openssl-3.5.7/lib

	## v2.0.9: OpenSSL 3.x may install its static libs to lib64/ instead of lib/
	## (depends on platform/config; 3.5.x on this MinGW toolchain uses lib64).
	## Resolve HERE, at the single source of truth, so BOTH the explicit .a paths
	## in include/libs.pri AND the search path / existence check in
	## include/libs/openssl.pri pick up the correct directory automatically.
	## config.pri is included before libs.pri (see the .pro files), so this
	## reassignment propagates everywhere downstream.
	exists($${DIGITALNOTE_OPENSSL_LIB_PATH}64/libcrypto.a) {
		DIGITALNOTE_OPENSSL_LIB_PATH  = $${DIGITALNOTE_OPENSSL_LIB_PATH}64
	}

	## v2.0.9: build against OpenSSL 3.5.7 (bumped from 1.1.1w).  The wallet still
	## calls raw-EC/BN/ECDSA/RIPEMD APIs (ecwrapper.cpp [SMSG], stealth.cpp,
	## smsg.cpp, cbignum*, hash.cpp) that OpenSSL 3.x marks DEPRECATED but still
	## ships.  OPENSSL_SUPPRESS_DEPRECATED silences the deprecation diagnostics so
	## they don't break -Werror builds; the functions themselves remain available
	## because the library is NOT configured with no-deprecated (see the builder's
	## compile/openssl.sh).  Migrating these call sites to the EVP/OSSL_PARAM 3.x
	## API is deferred future work (see v209-TODO Section 6b / Track B).
	DEFINES += OPENSSL_SUPPRESS_DEPRECATED
	
	## Berkeley db library
	DIGITALNOTE_BDB_INCLUDE_PATH      = $${DIGITALNOTE_PATH}/../libs/db-6.2.32.NC/include
	DIGITALNOTE_BDB_LIB_PATH          = $${DIGITALNOTE_PATH}/../libs/db-6.2.32.NC/lib
	DIGITALNOTE_LIB_BDB_SUFFIX        = 
	
	## Event library
	DIGITALNOTE_EVENT_INCLUDE_PATH    = $${DIGITALNOTE_PATH}/../libs/libevent-2.1.12-stable/include
	DIGITALNOTE_EVENT_LIB_PATH        = $${DIGITALNOTE_PATH}/../libs/libevent-2.1.12-stable/lib
	
	## GMP library — built from source by CI's libs job into
	## Builder/macos/<arch>/libs/gmp-6.3.0/ (matches compile/gmp.sh).
	## Manual macOS builders should run compile/gmp.sh in their flow as
	## well; otherwise this path will be empty and the static link fails.
	DIGITALNOTE_GMP_INCLUDE_PATH      = $${DIGITALNOTE_PATH}/../libs/gmp-6.3.0/include
	DIGITALNOTE_GMP_LIB_PATH          = $${DIGITALNOTE_PATH}/../libs/gmp-6.3.0/lib
	
	## Miniupnp library
	DIGITALNOTE_MINIUPNP_INCLUDE_PATH = $${DIGITALNOTE_PATH}/../libs/miniupnpc-2.2.8/include
	DIGITALNOTE_MINIUPNP_LIB_PATH     = $${DIGITALNOTE_PATH}/../libs/miniupnpc-2.2.8/lib
	## miniupnpc 2.2.8 = API version 18 — set explicitly since
	## Makefile.mingw does not define MINIUPNPC_API_VERSION correctly on Linux
	DEFINES += MINIUPNPC_API_VERSION=18
	
	## QREncode library
	DIGITALNOTE_QRENCODE_INCLUDE_PATH = $${DIGITALNOTE_PATH}/../libs/qrencode-4.1.1/include
	DIGITALNOTE_QRENCODE_LIB_PATH     = $${DIGITALNOTE_PATH}/../libs/qrencode-4.1.1/lib
	
	## BIP39 (sources compiled directly — no external lib needed)
	DIGITALNOTE_BIP39_INCLUDE_PATH = $${DIGITALNOTE_PATH}/src/bip39/include
	DIGITALNOTE_BIP39_SRC_PATH     = $${DIGITALNOTE_PATH}/src/bip39/src
}

##
## Linux — libs compiled from source by CI (see ci-linux-x64.yml)
##
linux:!macx {
	## Boost
	DIGITALNOTE_BOOST_INCLUDE_PATH    = $${DIGITALNOTE_PATH}/../libs/boost_1_91_0/include
	DIGITALNOTE_BOOST_LIB_PATH        = $${DIGITALNOTE_PATH}/../libs/boost_1_91_0/lib
	DIGITALNOTE_BOOST_SUFFIX          = 
	
	## OpenSSL library
	DIGITALNOTE_OPENSSL_INCLUDE_PATH  = $${DIGITALNOTE_PATH}/../libs/openssl-3.5.7/include
	DIGITALNOTE_OPENSSL_LIB_PATH      = $${DIGITALNOTE_PATH}/../libs/openssl-3.5.7/lib

	## v2.0.9: OpenSSL 3.x may install its static libs to lib64/ instead of lib/
	## (depends on platform/config; 3.5.x on this MinGW toolchain uses lib64).
	## Resolve HERE, at the single source of truth, so BOTH the explicit .a paths
	## in include/libs.pri AND the search path / existence check in
	## include/libs/openssl.pri pick up the correct directory automatically.
	## config.pri is included before libs.pri (see the .pro files), so this
	## reassignment propagates everywhere downstream.
	exists($${DIGITALNOTE_OPENSSL_LIB_PATH}64/libcrypto.a) {
		DIGITALNOTE_OPENSSL_LIB_PATH  = $${DIGITALNOTE_OPENSSL_LIB_PATH}64
	}

	## v2.0.9: build against OpenSSL 3.5.7 (bumped from 1.1.1w).  The wallet still
	## calls raw-EC/BN/ECDSA/RIPEMD APIs (ecwrapper.cpp [SMSG], stealth.cpp,
	## smsg.cpp, cbignum*, hash.cpp) that OpenSSL 3.x marks DEPRECATED but still
	## ships.  OPENSSL_SUPPRESS_DEPRECATED silences the deprecation diagnostics so
	## they don't break -Werror builds; the functions themselves remain available
	## because the library is NOT configured with no-deprecated (see the builder's
	## compile/openssl.sh).  Migrating these call sites to the EVP/OSSL_PARAM 3.x
	## API is deferred future work (see v209-TODO Section 6b / Track B).
	DEFINES += OPENSSL_SUPPRESS_DEPRECATED
#	
	## Berkeley db library
	DIGITALNOTE_BDB_INCLUDE_PATH      = $${DIGITALNOTE_PATH}/../libs/db-6.2.32.NC/include
	DIGITALNOTE_BDB_LIB_PATH          = $${DIGITALNOTE_PATH}/../libs/db-6.2.32.NC/lib
	
	## Event library
	DIGITALNOTE_EVENT_INCLUDE_PATH    = $${DIGITALNOTE_PATH}/../libs/libevent-2.1.12-stable/include
	DIGITALNOTE_EVENT_LIB_PATH        = $${DIGITALNOTE_PATH}/../libs/libevent-2.1.12-stable/lib
	
	## GMP library
	## Default (x86_64): apt's libgmp-dev provides /usr/lib/x86_64-linux-gnu/libgmp.a
	## aarch64 cross-compile: built from source by Builder/linux/aarch64/compile_libs.sh
	## via compile/gmp.sh, output at libs/gmp-6.3.0/. Toggle by passing
	## TARGET_ARCH=aarch64 to qmake (CI does this; manual builders should
	## too — see linux/aarch64/ReadMe.md).
	contains(TARGET_ARCH, aarch64) {
		DIGITALNOTE_GMP_INCLUDE_PATH  = $${DIGITALNOTE_PATH}/../libs/gmp-6.3.0/include
		DIGITALNOTE_GMP_LIB_PATH      = $${DIGITALNOTE_PATH}/../libs/gmp-6.3.0/lib
	} else {
		DIGITALNOTE_GMP_INCLUDE_PATH  = /usr/include
		DIGITALNOTE_GMP_LIB_PATH      = /usr/lib/x86_64-linux-gnu
	}
	
	## Miniupnp library
	DIGITALNOTE_MINIUPNP_INCLUDE_PATH = $${DIGITALNOTE_PATH}/../libs/miniupnpc-2.2.8/include
	DIGITALNOTE_MINIUPNP_LIB_PATH     = $${DIGITALNOTE_PATH}/../libs/miniupnpc-2.2.8/lib
	## miniupnpc 2.2.8 = API version 18 — set explicitly since
	## Makefile.mingw does not define MINIUPNPC_API_VERSION correctly on Linux
	DEFINES += MINIUPNPC_API_VERSION=18
	
	## QREncode library
	DIGITALNOTE_QRENCODE_INCLUDE_PATH = $${DIGITALNOTE_PATH}/../libs/qrencode-4.1.1/include
	DIGITALNOTE_QRENCODE_LIB_PATH     = $${DIGITALNOTE_PATH}/../libs/qrencode-4.1.1/lib
	
	## BIP39 (sources compiled directly — no external lib needed)
	DIGITALNOTE_BIP39_INCLUDE_PATH = $${DIGITALNOTE_PATH}/src/bip39/include
	DIGITALNOTE_BIP39_SRC_PATH     = $${DIGITALNOTE_PATH}/src/bip39/src
}
