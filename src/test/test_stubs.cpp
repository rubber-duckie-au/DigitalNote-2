// Copyright (c) 2026 DigitalNote XDN developers
// Distributed under the MIT software license.
// SPDX-License-Identifier: MIT
//
// src/test/test_stubs.cpp -- init-layer stubs for the unit test binaries.
//
// ADDED 2026-07-28 (TODO 12.A.2 step 4).
//
// WHY THIS FILE EXISTS.
// The dncore static library deliberately EXCLUDES init.cpp and noui.cpp.
// init.cpp is the daemon bring-up path: linking it into a test binary would
// drag the whole node startup surface in, and worse, it starts threads.  A
// unit test must not be able to accidentally spin up node threads.
//
// But a handful of globals and shutdown hooks declared in init.h are
// referenced from code that IS in dncore (util.cpp, main.cpp and the wallet
// layer all consult them).  Excluding init.cpp therefore leaves those symbols
// undefined at link time.
//
// The standard answer -- and the one Bitcoin Core uses in its own test
// fixture -- is to provide inert stubs.  That is this file.
//
// SEMANTICS.  Every stub here is deliberately inert:
//   * StartShutdown() does nothing.  A test that trips a shutdown path should
//     not tear anything down; if a test ever depends on shutdown actually
//     happening, that test wants an integration fixture, not this stub.
//   * ShutdownRequested() always returns false, so loops that poll it run
//     their normal path rather than immediately bailing out.
//   * The flags take the same defaults init.cpp gives them before any
//     command-line parsing.
//
// DO NOT add behaviour here.  If a test needs real init semantics it belongs
// in the functional harness (TODO 12.A.4), not in a unit test.

#include "init.h"
#include "main_extern.h"
#include "ui_interface.h"

// --- Globals normally defined in init.cpp -----------------------------------
// init.cpp:78, :82, :84.  Defaults match the pre-argument-parsing state.
bool fConfChange = false;
bool fUseFastIndex = false;
bool fWalletLoadComplete = false;

// init.cpp:79-80.  Set from -addrlifespan and the wallet derivation-method
// selection during argument parsing; the defaults below match what init.cpp
// assigns (init.cpp:470, :474).
unsigned int nNodeLifespan = 7;
unsigned int nDerivationMethodIndex = 0;

// init.cpp:73.  No wallet is instantiated for unit tests.  A test that needs a
// live CWallet wants the functional harness (TODO 12.A.4), not this stub.
class CWallet;
CWallet* pwalletMain = NULL;

// noui.cpp.  The UI signal hub.  Unit tests have no UI, so an inert default-
// constructed instance is correct -- signals fire into nothing.
CClientUIInterface uiInterface;

// --- Shutdown hooks normally defined in init.cpp ----------------------------
// init.cpp:119, :124.  Inert by design -- see the note above.
void StartShutdown()
{
	// Intentionally empty.  Unit tests never shut the node down because they
	// never start it.
}

bool ShutdownRequested()
{
	return false;
}
