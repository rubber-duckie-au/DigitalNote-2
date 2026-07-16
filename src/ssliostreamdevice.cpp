#include "boost_ioservices.h"

#include "ssliostreamdevice.h"

template <typename Protocol>
SSLIOStreamDevice<Protocol>::SSLIOStreamDevice(boost::asio::ssl::stream<typename Protocol::socket> &streamIn, bool fUseSSLIn) : stream(streamIn)
{
	fUseSSL = fUseSSLIn;
	fNeedHandshake = fUseSSLIn;
}

template <typename Protocol>
void SSLIOStreamDevice<Protocol>::handshake(boost::asio::ssl::stream_base::handshake_type role)
{
	if (!fNeedHandshake)
	{
		return;
	}
	
	fNeedHandshake = false;
	stream.handshake(role);
}

template <typename Protocol>
std::streamsize SSLIOStreamDevice<Protocol>::read(char* s, std::streamsize n)
{
	handshake(boost::asio::ssl::stream_base::server); // HTTPS servers read first
	
	if (fUseSSL)
	{
		return stream.read_some(boost::asio::buffer(s, n));
	}
	
	return stream.next_layer().read_some(boost::asio::buffer(s, n));
}

template <typename Protocol>
std::streamsize SSLIOStreamDevice<Protocol>::write(const char* s, std::streamsize n)
{
	handshake(boost::asio::ssl::stream_base::client); // HTTPS clients write first
	
	if (fUseSSL)
	{
		return boost::asio::write(stream, boost::asio::buffer(s, n));
	}
	
	return boost::asio::write(stream.next_layer(), boost::asio::buffer(s, n));
}



template <typename Protocol>
bool SSLIOStreamDevice<Protocol>::connect(const std::string& server, const std::string& port)
{
	// Boost Version < 1.70 handling (Updated) - Thank you https://github.com/g1itch
	boost::asio::ip::tcp::resolver resolver(GetIOService(stream));
	// v2.0.0.9 (Boost 1.87 asio): resolver::query and resolver::iterator were removed.
	// resolve(host, service) now returns a results_type range that is directly iterable.
	// The close-and-try-next-endpoint retry semantics below are preserved unchanged.
	boost::system::error_code error = boost::asio::error::host_not_found;
	boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(server.c_str(), port.c_str());

	boost::asio::ip::tcp::resolver::results_type::const_iterator endpoint_iterator = endpoints.begin();
	boost::asio::ip::tcp::resolver::results_type::const_iterator end = endpoints.end();

	while (error && endpoint_iterator != end)
	{
		stream.lowest_layer().close();
		stream.lowest_layer().connect(*endpoint_iterator++, error);
	}
	
	if (error)
	{
		return false;
	}
	
	return true;
}

// Define template type
template class SSLIOStreamDevice<boost::asio::ip::tcp>;

