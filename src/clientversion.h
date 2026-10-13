#ifndef CLIENTVERSION_H
#define CLIENTVERSION_H

//
// client versioning
//

// These need to be macros, as version.cpp's and bitcoin-qt.rc's voodoo requires it
#define CLIENT_VERSION_MAJOR       2
#define CLIENT_VERSION_MINOR       0
#define CLIENT_VERSION_REVISION    0
#define CLIENT_VERSION_BUILD       9

// Set to true for release, false for prerelease or test build
//
// Stays false through milestones M0-M6 of v2.0.0.8 development.  Switched
// to true at M7 (the actual release build).
//
// v2.0.0.9 W-18 (2026-10-08): set FALSE again for release candidates.  It was
// left true on rc1/rc2/rc3, which was simply inaccurate -- they are not
// releases.  Set it back to true only for the build that is actually tagged.
#define CLIENT_VERSION_IS_RELEASE  false

// v2.0.0.9 W-18: distinguishes release candidates from each other and from the
// final build.
//
// >>> WHY THIS EXISTS. <<<  FormatFullVersion() returns CLIENT_BUILD, which was
// assembled purely from the four numbers above -- so rc1, rc2 and rc3 reported
// an IDENTICAL version string over RPC, and the protocol version does not move
// between candidates either.  During a soak with several rcs in circulation,
// "did that node get the new build?" got a misleading answer, and results could
// be attributed to the wrong binary.
//
// >>> DO NOT "FIX" THIS BY RE-ENABLING share/genbuild.{sh,bat}. <<<  That path
// was deliberately disabled in v2.0.0.8 (see version.cpp): the most recent
// reachable tag was an old v1.0.x, so `git describe` reported v1.0.x-<n>-g<hash>
// for a v2.0.0.8 codebase -- a worse lie than the fixed string.  Reinstating it
// requires fixing the repository's tag state FIRST.
//
// Set to "" (empty) for the tagged release build.
#define CLIENT_VERSION_CANDIDATE   "rc4-"

// Converts the parameter X to a string after macro replacement on X has been performed.
// Don't merge these into one macro!
#define STRINGIZE(X) DO_STRINGIZE(X)
#define DO_STRINGIZE(X) #X

#endif // CLIENTVERSION_H
