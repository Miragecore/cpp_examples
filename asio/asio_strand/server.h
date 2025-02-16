#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdint.h>

#include <atomic>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <memory>
#include <optional>
#include <unordered_map>

class Session;

using boost::asio::ip::tcp;

class TCPServer : public std::enable_shared_from_this<TCPServer> {
 public:
  TCPServer(short port);
  virtual ~TCPServer();

 private:
  friend class Session;

  void start_accept();
  void handle_accept(const boost::system::error_code& error);

  std::shared_ptr<boost::asio::io_context> io_context_;
  std::thread io_thread_;

  std::atomic<std::uint64_t> session_counter_;
  std::unordered_map<uint64_t, std::shared_ptr<Session>> sessions_;
  tcp::acceptor acceptor_;
  // Use std::optional to defer socket construction
  std::optional<tcp::socket> socket_;
  boost::asio::streambuf request_;
};

#endif
