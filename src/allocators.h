// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef ALLOCATORS_H
#define ALLOCATORS_H

//
// Functions for directly locking/unlocking memory objects.
// Intended for non-dynamically allocated structures.
//
template<typename T>
void LockObject(const T &t);

template<typename T>
void UnlockObject(const T &t);

#endif
