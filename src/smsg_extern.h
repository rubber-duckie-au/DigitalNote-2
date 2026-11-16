#ifndef SMSG_EXTERN_H
#define SMSG_EXTERN_H

#include <atomic>
#include <cstdint>
#include <map>
#include <vector>
#include <boost/thread/thread.hpp>

#include "types/ccriticalsection.h"

namespace leveldb {
	class DB;
}

namespace DigitalNote
{
	namespace SMSG
	{
		class Address;
		class Stored;
		class Options;
		class Bucket;
		
		// Extern
		extern boost::thread_group							ext_thread_group;
		// v2.0.0.9: atomic.  Read without a lock by network, RPC, wallet
		// and worker threads, and written by Start/Enable/Disable/Shutdown.
		// A plain bool here was a C++ data race (undefined behaviour).
		// Atomic makes each read and write well-defined; it does NOT make
		// a check-then-act sequence atomic.
		extern std::atomic<bool>							ext_enabled;
		extern std::map<int64_t, DigitalNote::SMSG::Bucket>	ext_buckets;
		extern std::vector<DigitalNote::SMSG::Address>		ext_addresses;
		extern DigitalNote::SMSG::Options					ext_options;
		extern CCriticalSection								ext_cs;				// all except inbox and outbox
		extern CCriticalSection								ext_cs_db;
		extern leveldb::DB*									ext_db;
	} // namespace SMSG
} // namespace DigitalNote

#endif // SMSG_EXTERN_H
