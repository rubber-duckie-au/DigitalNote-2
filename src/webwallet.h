#ifndef WEBWALLET_H
#define WEBWALLET_H

#include <set>
#include <string>
#include <memory>

// v2.0.0.9 Boost.Beast migration:
// The web-wallet connector previously used the vendored websocketpp 0.8.2 library,
// which relies on boost::asio::io_service (removed in Boost 1.87). It has been
// reimplemented on Boost.Beast, which ships with Boost itself (>= 1.66) and tracks
// Boost.Asio, so it will not rot against future Boost versions.
//
// The EXTERNAL contract is unchanged: Start()/Shutdown()/ext_mode and the JSON text
// frames sent to connected clients are identical to the websocketpp implementation.
// The websocketpp connection_hdl (a weak_ptr) is replaced by a shared_ptr<session>;
// this type is PRIVATE to the two webwallet translation units - no other file
// references it (verified: connection_hdl / Webwallet::server / Webwallet::connections
// appear only in webwallet.cpp and webwallet_broadcast.cpp).

namespace DigitalNote
{
	namespace Webwallet
	{
		class broadcast;
		class session;   // one live client connection (defined in webwallet_broadcast.cpp)

		// Was: std::set<websocketpp::connection_hdl, std::owner_less<...>>
		// Now: a set of shared_ptr to Beast-backed sessions. Same concept
		// (the set of currently-open client connections), different element type.
		typedef std::set<std::shared_ptr<DigitalNote::Webwallet::session>> connections;

		enum action_type
		{
			SUBSCRIBE,
			UNSUBSCRIBE,
			MESSAGE,
			STOP_COMMAND
		};

		struct action
		{
			Webwallet::action_type					type;
			std::shared_ptr<DigitalNote::Webwallet::session>	hdl;
			std::string								msg;

			action(Webwallet::action_type t, std::shared_ptr<DigitalNote::Webwallet::session> h) : type(t), hdl(h)
			{

			}

			action(Webwallet::action_type t, std::shared_ptr<DigitalNote::Webwallet::session> h, const std::string &m): type(t), hdl(h), msg(m)
			{

			}

			action(Webwallet::action_type t, const std::string &m): type(t), msg(m)
			{

			}

			action(Webwallet::action_type t): type(t)
			{

			}
		};

		extern Webwallet::connections	ext_connections;
		extern Webwallet::broadcast		ext_broadcast;

		extern bool ext_mode;
		extern bool ext_connector_enabled;

		bool Start(bool fDontStart);
		bool Shutdown();
		void ThreadWebsocket();
		void SendUpdate(const std::string &msg);

		void Subscribe();
		void Unsubscribe();
	} // namespace Webwallet
} // namespace DigitalNote

#endif // WEBWALLET_H
