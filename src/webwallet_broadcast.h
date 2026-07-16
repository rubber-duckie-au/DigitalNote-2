#ifndef WEBWALLET_BROADCAST_H
#define WEBWALLET_BROADCAST_H

#include <cstdint>
#include <memory>
#include <string>

// v2.0.9 Boost.Beast migration. See webwallet.h for rationale.
// The public interface of broadcast is preserved 1:1 with the websocketpp version
// (run/stop/on_open/on_close/sendMessage/process_messages) so webwallet.cpp is
// unchanged except for the connection handle type (connection_hdl -> shared_ptr<session>).

namespace DigitalNote
{
	namespace Webwallet
	{
		class session;

		class broadcast
		{
			public:
				broadcast();

				void run(uint16_t port);
				void stop();
				void on_open(std::shared_ptr<DigitalNote::Webwallet::session> hdl);
				void on_close(std::shared_ptr<DigitalNote::Webwallet::session> hdl);
				void sendMessage(const std::string &msg);
				static void process_messages();
		};
	} // namespace Webwallet
} // namespace DigitalNote

#endif // WEBWALLET_BROADCAST_H
