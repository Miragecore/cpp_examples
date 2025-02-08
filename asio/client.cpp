#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using boost::asio::ip::tcp;

class TCPClient {
 public:
  TCPClient(boost::asio::io_context& io_context)
      : socket_(io_context), resolver_(io_context) {}

  void start(const std::string& host, const std::string& port) {
    // Resolve the server address and port
    resolver_.async_resolve(host, port,
                            boost::bind(&TCPClient::handle_resolve, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::results));
  }

 private:
  void read() {
    /*
      boost::asio::async_read_until(socket_, response_, '\n',
          boost::bind(&TCPClient::handle_read, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    */

    boost::asio::async_read(
        socket_, response_, boost::asio::transfer_at_least(1),
        // boost::asio::transfer_all(),
        boost::bind(&TCPClient::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  std::string message = "Hello, Server!\n";
  void write_hello() {
    boost::asio::async_write(
        socket_, boost::asio::buffer(message),
        boost::bind(&TCPClient::handle_write, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  void handle_resolve(const boost::system::error_code& error,
                      tcp::resolver::results_type results) {
    if (!error) {
      // Attempt a connection to the resolved endpoint
      boost::asio::async_connect(socket_, results,
                                 boost::bind(&TCPClient::handle_connect, this,
                                             boost::asio::placeholders::error));
    } else {
      std::cerr << "Resolve error: " << error.message() << std::endl;
    }
  }

  void handle_connect(const boost::system::error_code& error) {
    if (!error) {
      // Connection established, send a message
      // write_hello();
      read();
      write_hello();
    } else {
      std::cerr << "Connect error: " << error.message() << std::endl;
    }
  }

  void handle_write(const boost::system::error_code& error,
                    size_t bytes_transferred) {
    if (!error) {
      // Message sent successfully, now receive a response
      // read();
      write_hello();
      std::cout << "Client Write" << std::endl;
    } else {
      std::cerr << "Write error: " << error.message() << std::endl;
    }
  }

  void handle_read(const boost::system::error_code& error,
                   size_t bytes_transferred) {
    static bool isHeader = true;
    static uint32_t len = 0;
    if (!error) {
      std::cout << "Client Received: " << bytes_transferred << std::endl;
      response_.consume(bytes_transferred);
      read();

      /*
            if(isHeader){
              std::istream is(&response_);
              is.read(reinterpret_cast<char *>(&len), 4);
              std::cout << len;
              isHeader = false;
            }else{
              if(len <= response_.size()){
                std::cout << "Read All: " <<response_.size() <<":"
         <<bytes_transferred << std::endl; response_.consume(len); isHeader =
         true;
              }
            }

            std::cout << "Received: " <<response_.size() <<":" <<bytes_transferred
         << std::endl;
            */
      /*

      std::copy_n(boost::asio::buffers_begin(sb.data()), len,
            std::ostream_iterator<decltype(ss)::char_type>(ss));

      const boost::asio::streambuf::const_buffers_type buffers = response_.data();
       */
      // Process the received data
      // std::istream is(&response_);
      // std::string line;
      // std::getline(is, line);

      // std::cout << "Received: " << line << std::endl;
      // write_hello();
    } else {
      std::cerr << "Read error: " << error.message() << std::endl;
    }
  }

  tcp::socket socket_;
  tcp::resolver resolver_;
  boost::asio::streambuf response_;
};

int main(int argc, char* argv[]) {
  try {
    if (argc != 3) {
      std::cerr << "Usage: client <host> <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    TCPClient client(io_context);
    client.start(argv[1], argv[2]);

    io_context.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
