#ifndef SMSG_EXTERN_H
#define SMSG_EXTERN_H

#include <cstdint>
#include <map>
#include <vector>
#include <boost/signals2/signal.hpp>
#include <boost/thread/thread.hpp>

#include "json/json_spirit_value.h"
#include "ccriticalsection.h"


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
		extern bool 										ext_enabled;
		extern std::map<int64_t, DigitalNote::SMSG::Bucket>	ext_buckets;
		extern std::vector<DigitalNote::SMSG::Address>      ext_addresses;
		extern DigitalNote::SMSG::Options					ext_options;
		extern CCriticalSection								ext_cs;            // all except inbox and outbox
		extern CCriticalSection								ext_cs_db;
		extern leveldb::DB*									ext_db;
		
		// Inbox db changed, called with lock DigitalNote::SMSG::ext_cs_db held.
		extern boost::signals2::signal<void (DigitalNote::SMSG::Stored& inboxHdr)>	ext_signal_NotifyInboxChanged;
		extern boost::signals2::signal<void (json_spirit::Object& inboxHdr)>		ext_signal_NotifyInboxChangedJson;
		// Outbox db changed, called with lock DigitalNote::SMSG::ext_cs_db held.
		extern boost::signals2::signal<void (DigitalNote::SMSG::Stored& outboxHdr)>	ext_signal_NotifyOutboxChanged;
		extern boost::signals2::signal<void (json_spirit::Object& outboxHdr)>		ext_signal_NotifyOutboxChangedJson;
		// Wallet Unlocked, called after all messages received while locked have been processed.
		extern boost::signals2::signal<void ()>										ext_signal_NotifyWalletUnlocked;
		
	} // namespace SMSG
} // namespace DigitalNote

#endif // SMSG_EXTERN_H
