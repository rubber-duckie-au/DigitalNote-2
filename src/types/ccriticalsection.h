#ifndef CCRITICALSECTION_H
#define CCRITICALSECTION_H

//#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include "annotatedmixin.h"

/** Wrapped boost mutex: supports recursive locking, but no waiting  */
// TODO: We should move away from using the recursive lock by default.
typedef AnnotatedMixin<boost::recursive_mutex> CCriticalSection;

/** Wrapped boost mutex: supports waiting but not recursive locking */
//typedef AnnotatedMixin<boost::mutex> CWaitableCriticalSection;

#endif // CCRITICALSECTION_H
