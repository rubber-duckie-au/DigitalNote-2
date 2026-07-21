#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <set>
#include <string>

#include <boost/thread.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include "util.h"
#include "webwallet.h"
#include "webwallet_broadcast.h"

// v2.0.0.9 Boost.Beast migration of the web-wallet connector (was websocketpp 0.8.2).
//
// Behaviour preserved from the websocketpp version:
//   * plain ws:// server (no TLS), listening on the given port (7778),
//   * push-only: text frames carrying the JSON string produced upstream in
//     webwallet.cpp via write_string(json_spirit::Value(msg), false),
//   * broadcast-to-all via the same action queue + mutex + condition_variable model,
//   * per-connection send failures are swallowed and DO NOT abort the broadcast loop
//     (websocketpp passed an error_code and ignored it; we catch per-session and continue),
//   * SUBSCRIBE/UNSUBSCRIBE on connect/disconnect; STOP_COMMAND closes all + stops listening.
//
// Consumers (browser ws clients on port 7778) receive byte-identical text frames:
// WebSocket framing is RFC 6455, independent of the library, and the JSON payload is
// unchanged.

namespace beast     = boost::beast;
namespace websocket = boost::beast::websocket;
namespace net       = boost::asio;
using tcp           = boost::asio::ip::tcp;

std::mutex									m_action_lock;
std::mutex									m_connection_lock;
boost::thread*								m_thread = NULL;
std::condition_variable						m_action_cond;
std::queue<DigitalNote::Webwallet::action>	m_actions;

namespace DigitalNote
{
namespace Webwallet
{

// One connected client. Owns its Beast websocket stream. All writes are posted onto
// the stream's strand so sendMessage() (called from other threads) is safe.
class session : public std::enable_shared_from_this<session>
{
	public:
		explicit session(tcp::socket &&socket)
			: m_ws(std::move(socket)), m_strand(m_ws.get_executor())
		{

		}

		// Complete the websocket handshake, then start reading (reads are drained and
		// ignored - this is a push-only server, but we must read to observe close frames).
		void start()
		{
			auto self = shared_from_this();

			m_ws.async_accept(
				[self](beast::error_code ec)
				{
					if (ec)
					{
						DigitalNote::Webwallet::ext_broadcast.on_close(self);

						return;
					}

					// register the connection
					DigitalNote::Webwallet::ext_broadcast.on_open(self);

					self->do_read();
				}
			);
		}

		// Queue a text-frame write of msg. Serialised on the strand.
		void send(const std::string &msg)
		{
			auto self = shared_from_this();
			auto copy = std::make_shared<std::string>(msg);

			net::post(
				m_strand,
				[self, copy]()
				{
					self->m_outbox.push_back(copy);

					if (self->m_outbox.size() > 1)
					{
						// a write is already in progress
						return;
					}

					self->do_write();
				}
			);
		}

		// Initiate a clean close (going-away), then let the read loop unwind.
		void close()
		{
			auto self = shared_from_this();

			net::post(
				m_strand,
				[self]()
				{
					beast::error_code ec;

					self->m_ws.close(websocket::close_code::going_away, ec);
				}
			);
		}

	private:
		void do_read()
		{
			auto self = shared_from_this();

			m_ws.async_read(
				m_buffer,
				[self](beast::error_code ec, std::size_t)
				{
					if (ec)
					{
						// closed or error -> deregister
						DigitalNote::Webwallet::ext_broadcast.on_close(self);

						return;
					}

					// push-only server: discard whatever the client sent, keep reading
					self->m_buffer.consume(self->m_buffer.size());

					self->do_read();
				}
			);
		}

		void do_write()
		{
			auto self = shared_from_this();

			m_ws.text(true);   // matches websocketpp opcode::text
			m_ws.async_write(
				net::buffer(*m_outbox.front()),
				[self](beast::error_code ec, std::size_t)
				{
					self->m_outbox.pop_front();

					if (ec)
					{
						// per-connection send failure: swallow (matches old behaviour of
						// ignoring the error_code); the read loop will deregister on close.
						return;
					}

					if (!self->m_outbox.empty())
					{
						self->do_write();
					}
				}
			);
		}

		websocket::stream<beast::tcp_stream>				m_ws;
		net::strand<net::any_io_executor>					m_strand;
		beast::flat_buffer									m_buffer;
		std::deque<std::shared_ptr<const std::string>>		m_outbox;
};

// The io_context + acceptor live here (were owned by websocketpp's server before).
static net::io_context*		g_ioc = NULL;
static tcp::acceptor*		g_acceptor = NULL;

broadcast::broadcast()
{
	// nothing to init up-front; the io_context/acceptor are created in run()
	// (websocketpp did init_asio()+handler registration in the ctor; with Beast the
	// per-connection handlers live on the session, so there is nothing to register here).
}

static void do_accept()
{
	g_acceptor->async_accept(
		[](beast::error_code ec, tcp::socket socket)
		{
			if (!ec)
			{
				std::make_shared<DigitalNote::Webwallet::session>(std::move(socket))->start();
			}

			// keep accepting unless the acceptor was closed
			if (g_acceptor->is_open())
			{
				do_accept();
			}
		}
	);
}

void broadcast::run(uint16_t port)
{
	LogPrint("webwallet", "webwallet: run is starting. \n");

	try
	{
		g_ioc = new net::io_context(1);
		g_acceptor = new tcp::acceptor(*g_ioc, tcp::endpoint(tcp::v4(), port));

		// Start the accept loop
		do_accept();

		LogPrint("webwallet", "webwallet: Creating processing thread \n");

		m_thread = new boost::thread(boost::bind(&process_messages));

		LogPrint("webwallet", "webwallet: ext_server is starting. \n");

		// Run the ASIO event loop (blocks until the io_context runs out of work,
		// i.e. after stop() closes the acceptor and all connections).
		g_ioc->run();

		LogPrint("webwallet", "webwallet: ext_server is now closed. \n");
	}
	catch (const std::exception &e)
	{
		LogPrint("webwallet", "webwallet: ERROR: Failed to start websocket. \n");
		LogPrint("webwallet", e.what());
	}

	if (m_thread != NULL)
	{
		m_thread->join();
	}
}

void broadcast::stop()
{
	LogPrint("webwallet", "webwallet: Requesting websocket to stop.\n");

	std::lock_guard<std::mutex> guard(m_action_lock);

	m_actions.push(action(STOP_COMMAND));
	m_action_cond.notify_all();
}

void broadcast::on_open(std::shared_ptr<DigitalNote::Webwallet::session> hdl)
{
	{
		std::lock_guard<std::mutex> guard(m_action_lock);

		m_actions.push(action(SUBSCRIBE, hdl));
	}

	m_action_cond.notify_all();
}

void broadcast::on_close(std::shared_ptr<DigitalNote::Webwallet::session> hdl)
{
	{
		std::lock_guard<std::mutex> guard(m_action_lock);

		m_actions.push(action(UNSUBSCRIBE, hdl));
	}

	m_action_cond.notify_all();
}

void broadcast::sendMessage(const std::string &msg)
{
	if (!DigitalNote::Webwallet::ext_connector_enabled)
	{
		return;
	}

	LogPrint("webwallet", "webwallet: Sending sendMessage to queue \n");
	LogPrint("webwallet", "webwallet: %s \n", msg);

	std::lock_guard<std::mutex> guard(m_action_lock);

	m_actions.push(action(MESSAGE, msg));

	LogPrint("webwallet", "webwallet: m_actions size %d .\n", m_actions.size());
	LogPrint("webwallet", "webwallet: notyfing all .\n");

	m_action_cond.notify_all();
}

void broadcast::process_messages()
{
	while(true)
	{
		LogPrint("webwallet", "webwallet: Locked m_action_lock.\n");

		std::unique_lock<std::mutex> lock(m_action_lock);

		while(m_actions.empty())
		{
			LogPrint("webwallet", "webwallet: Waiting for new actions.\n");

			m_action_cond.wait(lock);
		}

		action a = m_actions.front();
		m_actions.pop();

		lock.unlock();

		if (a.type == SUBSCRIBE)
		{
			LogPrint("webwallet", "webwallet: Connection SUBSCRIBE.\n");

			std::lock_guard<std::mutex> guard(m_connection_lock);

			DigitalNote::Webwallet::ext_connections.insert(a.hdl);
		}
		else if (a.type == UNSUBSCRIBE)
		{
			LogPrint("webwallet", "webwallet: Connection UNSUBSCRIBE.\n");

			std::lock_guard<std::mutex> guard(m_connection_lock);

			DigitalNote::Webwallet::ext_connections.erase(a.hdl);
		}
		else if (a.type == MESSAGE)
		{
			LogPrint("webwallet", "webwallet: Connection MESSAGE.\n");
			std::lock_guard<std::mutex> guard(m_connection_lock);

			DigitalNote::Webwallet::connections::iterator it;

			for (it = DigitalNote::Webwallet::ext_connections.begin(); it != DigitalNote::Webwallet::ext_connections.end(); ++it)
			{
				// per-connection send: session::send swallows its own write errors,
				// so one dead client cannot abort the broadcast (matches old behaviour).
				(*it)->send(a.msg);
			}
		}
		else if (a.type == STOP_COMMAND)
		{
			LogPrint("webwallet", "webwallet: STOP_COMMAND.\n");

			try
			{
				if (g_acceptor != NULL && g_acceptor->is_open())
				{
					beast::error_code ec;

					g_acceptor->close(ec);
				}

				LogPrint("webwallet", "webwallet: Websocket server stopped listening. \n");
			}
			catch (const std::exception &e)
			{
				LogPrint("webwallet", "webwallet: ERROR: Failed to stop websocket server. \n");
				LogPrint("webwallet", e.what());
			}

			std::lock_guard<std::mutex> guard(m_connection_lock);

			{
				DigitalNote::Webwallet::connections::iterator it;

				for (it = DigitalNote::Webwallet::ext_connections.begin(); it != DigitalNote::Webwallet::ext_connections.end(); ++it)
				{
					(*it)->close();
				}
			}

			LogPrint("webwallet", "webwallet: Sent close request to all connections.\n");

			break;
		}
		else {
			LogPrint("webwallet", "webwallet: undefined COMMAND.\n");
			// undefined.
		}
	}

	LogPrint("webwallet", "webwallet: Leaving process_messages.\n");
}

} // namespace Webwallet
} // namespace DigitalNote
