#ifndef SESSION_H_
#define SESSION_H_

#include <stdint.h>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

class TCPServer;
using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>{
public:
  Session(std::weak_ptr<TCPServer> server,
          tcp::socket sock,
          uint64_t session_id);
  virtual ~Session();
  void start();
private:

  void read();
  void session_closed();
  void handle_read(const boost::system::error_code& error,
                   size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);

  std::weak_ptr<TCPServer> server_;
  tcp::socket socket_;
  uint64_t session_id_;
  boost::asio::streambuf request_;
};

#endif
