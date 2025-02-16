#include "server.h"

#include <iostream>
#include <mutex>
#include <thread>

#include "session.h"

using boost::asio::ip::tcp;
class TCPServer;

Session::Session(std::weak_ptr<TCPServer> server,
                 tcp::socket sock,
                 uint64_t session_id)
    : server_(server),
      socket_(std::move(sock)),
      //strand_(boost::asio::make_strand(sock.get_executor())),
      session_id_(session_id),
      to_be_close_(false) {}

Session::~Session() {
  if (socket_.is_open()) {
    socket_.close();
  }
  std::cout << "session " << session_id_ << " destructed" << std::endl;
}

void Session::start() {
  std::string hello_msg = "12345678901234567890";
  enque(hello_msg);
  read();
}

void Session::enque(std::string& str) {
  
  //boost::asio::post(strand_, [&](){
  //});
  /*``
  std::scoped_lock lock(send_mutex_);
  bool is_not_writing = send_que_.empty();
  send_que_.emplace_back(std::vector<uint8_t>(str.begin(), str.end()));
  if (is_not_writing) {
    write();
  }
  */
}

void Session::enque(std::vector<uint8_t>& buf) {
  std::scoped_lock lock(send_mutex_);
  bool is_not_writing = send_que_.empty();
  send_que_.emplace_back(std::vector<uint8_t>(buf.begin(), buf.end()));
  if (is_not_writing) {
    write();
  }
}

void Session::setOnClosedCallback(std::function<void(uint64_t)> callback) {
  onCloseCallback_ = callback;
}

void Session::write() {
  boost::asio::async_write(socket_, boost::asio::buffer(send_que_.front()),
                           boost::bind(&Session::handle_write, shared_from_this(),
                                       boost::asio::placeholders::error));
}

void Session::read() {
  boost::asio::async_read(
      socket_, recv_buf_, boost::asio::transfer_at_least(1),
      boost::bind(&Session::handle_read, shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

void Session::read_until() {
  boost::asio::async_read_until(
      socket_, recv_buf_, '\n',
      boost::bind(&Session::handle_read, shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

void Session::close() {
  if (onCloseCallback_) {
    onCloseCallback_(session_id_);
  }
  to_be_close_ = true;
}

void Session::handle_read(const boost::system::error_code& error,
                          size_t bytes_transferred) {
  if (to_be_close_) {
    return;
  }
  if (!error) {
    // Process the received data
    // std::istream is(&recv_buf_);
    // std::string line;
    // std::getline(is, line);
    recv_buf_.consume(bytes_transferred);
    std::cout << "Session Received: " << bytes_transferred << std::endl;
    read();

    /*
    // Send a response back to the client
    std::string response = "1234567890";
    std::string send_str = "";
    for(int i = 0; i < 1024; i++){
      send_str += response;
    }

    uint32_t len = send_str.length();

    memcpy(&send_str[0], &len, sizeof(uint32_t));

    boost::asio::async_write(socket_, boost::asio::buffer(send_str),
                             boost::bind(&Session::handle_write, this,
                                         boost::asio::placeholders::error));
    */
  } else {
    std::cerr << "Read error: " << error.message() << std::endl;

    close();
  }
}

void Session::handle_write(const boost::system::error_code& error) {
  if (to_be_close_) {
    return;
  }
  if (!error) {
    // std::cout << "Response sent successfully." << std::endl;
    std::scoped_lock lock(send_mutex_);
    // TODO: just for test
    // send_que_.pop_front();
    if (send_que_.size() > 0) {
      write();
    }
    // read();
  } else {
    std::cerr << "Write error: " << error.message() << std::endl;
    // Close the socket after sending the response
    close();
  }
}

TCPServer::TCPServer(short port)
    : io_context_(std::make_shared<boost::asio::io_context>()),
      acceptor_(*io_context_, tcp::endpoint(tcp::v4(), port)),
      session_counter_(std::atomic<uint64_t>(0)) {
  start_accept();
  //io_thread_ = std::thread([io_context_](){io_context_->run();});
  io_thread_ = std::thread([&](){io_context_->run();});
}

TCPServer::~TCPServer(){
  io_thread_.join();
}

void TCPServer::start_accept() {
  // Create a new socket for the incoming connection
  socket_.emplace(acceptor_.get_executor());

  // Asynchronously accept a new connection
  acceptor_.async_accept(*socket_, boost::bind(&TCPServer::handle_accept, this,
                                               boost::asio::placeholders::error));
}

void TCPServer::handle_accept(const boost::system::error_code& error) {
  if (!error) {
    // Connection accepted, start reading data from the client
    std::shared_ptr<Session> session = std::make_shared<Session>(
        shared_from_this(), std::move(*socket_), session_counter_);
    sessions_[session_counter_] = session;

    session->setOnClosedCallback([&](uint64_t session_id) {
      if (sessions_.find(session_id) != sessions_.end()) {
        sessions_.erase(session_id);
      }
    });

    std::cout << "start session ID : " << session_counter_ << std::endl;
    session->start();
    session_counter_++;
  }

  // Accept another connection
  start_accept();
}

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: server <port>\n";
      return 1;
    }

    //boost::asio::io_context io_context;

    // Create and run the server
    std::shared_ptr<TCPServer> pServer =
        std::make_shared<TCPServer>(std::atoi(argv[1]));

    //io_context.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
