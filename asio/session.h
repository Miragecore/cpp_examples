#ifndef SESSION_H_
#define SESSION_H_

#include <stdint.h>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <deque>
#include <mutex>

class TCPServer;
using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
 public:
  Session(std::weak_ptr<TCPServer> server, tcp::socket sock, uint64_t session_id);
  virtual ~Session();
  void start();
  void close();
  void enque(std::string& str);
  void enque(std::vector<uint8_t>& buf);
  void setOnClosedCallback(std::function<void(uint64_t)> callback);

 private:
  void write();
  void read();
  void read_until();
  void handle_read(const boost::system::error_code& error,
                   size_t bytes_transferred);
  void handle_write(const boost::system::error_code& error);

  std::weak_ptr<TCPServer> server_;
  tcp::socket socket_;
  uint64_t session_id_;
  std::deque<std::vector<uint8_t>> send_que_;
  boost::asio::streambuf recv_buf_;

  std::mutex send_mutex_;

  std::function<void(uint64_t)> onCloseCallback_;
  std::atomic<bool> to_be_close_;
};

#endif
