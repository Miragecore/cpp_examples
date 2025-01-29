#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdint.h>
#include <atomic>
#include <optional>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

class Session;

using boost::asio::ip::tcp;

class TCPServer : public std::enable_shared_from_this<TCPServer>{
public:
    TCPServer(boost::asio::io_context& io_context, short port);
private:
  friend class Session;

  void start_accept();
  void session_closed(uint64_t session_id);
  void handle_accept(const boost::system::error_code& error);

  std::atomic<std::uint64_t> session_counter_;
  std::unordered_map<uint64_t, std::shared_ptr<Session>> sessions_;
  tcp::acceptor acceptor_;
  std::optional<tcp::socket> socket_; // Use std::optional to defer socket construction
  boost::asio::streambuf request_;
};


#endif
