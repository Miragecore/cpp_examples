#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <deque>
#include <memory>
#include <mutex>

using boost::asio::ip::tcp;

class TCPClient {
 public:
  TCPClient()
      : socket_(io_context_), resolver_(io_context_), to_be_close_(false) {}

  virtual ~TCPClient(){
    if(proc_){
      if(!io_context_.stopped()){
        io_context_.stop();
      }

      if(proc_->joinable()){
        proc_->join();
      }
      proc_.reset();
    }
  }

  void start(const std::string& host, const std::string& port) {
    // Resolve the server address and port
    resolver_.async_resolve(host, port,
                            boost::bind(&TCPClient::handle_resolve, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::results));

    if(!proc_){
      proc_ = std::make_shared<std::thread>([&](){io_context_.run();});
    }
  }

  void enque(const std::vector<uint8_t>& buf){
    std::scoped_lock _lock(send_que_mtx_);

    size_t current_que_size = send_que_.size();

    if(current_que_size == 0){
      send_que_.push_back(buf);
      write();
    }else{
      if(current_que_size > SEND_QUE_SIZE){
        boost::system::error_code ec;
        fail(ec, "enque");
        return;
      }
      send_que_.push_back(buf);
    }
  }

 private:
  const size_t SEND_QUE_SIZE = 16;

  void fail(const boost::system::error_code& ec, const std::string& from){
    std::cerr << "[" << from << "] " << ec.message() << "(" << ec.value() << ")" << std::endl;
    to_be_close_ = true;
  }

  void read() {
    /*
      boost::asio::async_read_until(socket_, recvbuf_, '\n',
          boost::bind(&TCPClient::handle_read, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    */

    boost::asio::async_read(
        socket_, recvbuf_, boost::asio::transfer_at_least(1),
        // boost::asio::transfer_all(),
        boost::bind(&TCPClient::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  void write(){
    boost::asio::async_write(
        socket_, boost::asio::buffer(send_que_.front()),
        boost::bind(&TCPClient::handle_write, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  void write_hello() {
    std::string message = "Hello, Server!\n";
    enque({message.begin(), message.end()});
  }

  void handle_resolve(const boost::system::error_code& error,
                      tcp::resolver::results_type results) {
    if (!error) {
      // Attempt a connection to the resolved endpoint
      boost::asio::async_connect(socket_, results,
                                 boost::bind(&TCPClient::handle_connect, this,
                                             boost::asio::placeholders::error));
    } else {
      return fail(error, "handle_resolve");
    }
  }

  void handle_connect(const boost::system::error_code& error) {
    if (!error) {
      // Connection established, send a message
      // write_hello();
      read();
      //write_hello();
    } else {
      return fail(error, "handler_connect");
    }
  }

  void handle_write(const boost::system::error_code& error,
                    size_t bytes_transferred) {
    if(to_be_close_) {return;}
    if (!error) {
      // Message sent successfully, now receive a response
      // read();
      //write_hello();
      {
        std::scoped_lock _lock(send_que_mtx_);
        send_que_.pop_front();
        if(!send_que_.empty()){
          write();
        }
      }
    } else {
      return fail(error, "handler_write");
    }
  }

  void handle_read(const boost::system::error_code& error,
                   size_t bytes_transferred) {

    if(to_be_close_) {return;}
    if (!error) {
      recvbuf_.consume(bytes_transferred);
      read();

      /*
            if(isHeader){
              std::istream is(&recvbuf_);
              is.read(reinterpret_cast<char *>(&len), 4);
              std::cout << len;
              isHeader = false;
            }else{
              if(len <= recvbuf_.size()){
                std::cout << "Read All: " <<recvbuf_.size() <<":"
         <<bytes_transferred << std::endl; recvbuf_.consume(len); isHeader =
         true;
              }
            }

            std::cout << "Received: " <<recvbuf_.size() <<":" <<bytes_transferred
         << std::endl;
            */
      /*

      std::copy_n(boost::asio::buffers_begin(sb.data()), len,
            std::ostream_iterator<decltype(ss)::char_type>(ss));

      const boost::asio::streambuf::const_buffers_type buffers = recvbuf_.data();
       */
      // Process the received data
      // std::istream is(&recvbuf_);
      // std::string line;
      // std::getline(is, line);

      // std::cout << "Received: " << line << std::endl;
      // write_hello();
    } else {
      return fail(error, "handler_write");
    }
  }

  boost::asio::io_context io_context_;
  tcp::socket socket_;
  std::shared_ptr<std::thread> proc_;
  tcp::resolver resolver_;
  boost::asio::streambuf recvbuf_;
  std::deque<std::vector<uint8_t>> send_que_;
  std::mutex send_que_mtx_;
  std::atomic_bool to_be_close_;
};

int main(int argc, char* argv[]) {
  try {
    if (argc != 3) {
      std::cerr << "Usage: client <host> <port>\n";
      return 1;
    }

    TCPClient client;
    client.start(argv[1], argv[2]);

  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
