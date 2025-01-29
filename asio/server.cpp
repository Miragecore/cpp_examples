#include <iostream>
#include "server.h"
#include "session.h"

using boost::asio::ip::tcp;
class TCPServer;

Session::Session(std::weak_ptr<TCPServer> server,
        tcp::socket sock,
        uint64_t session_id): 
  server_(server),
  socket_(std::move(sock)),
  session_id_(session_id){
}

Session::~Session(){
  std::cout << "session " << session_id_ << " destruct" << std::endl;
}

void Session::start(){
  read();
}

void Session::read(){
  boost::asio::async_read_until(socket_, request_, '\n',
      boost::bind(&Session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}

void Session::session_closed(){
  std::shared_ptr<TCPServer> pServer = server_.lock();
  if(pServer){
    pServer->session_closed(session_id_);
  }
}

void Session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        // Process the received data
        std::istream is(&request_);
        std::string line;
        std::getline(is, line);
        std::cout << "Received: " << line << std::endl;

        // Send a response back to the client
        std::string response = "Hello from server!\n";
        boost::asio::async_write(socket_, boost::asio::buffer(response),
            boost::bind(&Session::handle_write, this,
                boost::asio::placeholders::error));
    }
    else
    {
        std::cerr << "Read error: " << error.message() << std::endl;

        // Close the socket if there's an error
        if (socket_.is_open())
        {
            socket_.close();
            session_closed();
        }
    }
}

void Session::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
        std::cout << "Response sent successfully." << std::endl;
        read();
    }
    else
    {
        std::cerr << "Write error: " << error.message() << std::endl;
      // Close the socket after sending the response
      if (socket_.is_open())
      {
          socket_.close();
          session_closed();
      }
    }
}

TCPServer::TCPServer(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      session_counter_(std::atomic<uint64_t>(0))
{
    start_accept();
}

void TCPServer::start_accept()
{
    // Create a new socket for the incoming connection
    socket_.emplace(acceptor_.get_executor());

    // Asynchronously accept a new connection
    acceptor_.async_accept(*socket_,
        boost::bind(&TCPServer::handle_accept, this,
            boost::asio::placeholders::error));
}

void TCPServer::session_closed(uint64_t session_id){
  std::cout << "erase session : " << session_id << std::endl;
  sessions_.erase(session_id);
}

void TCPServer::handle_accept(const boost::system::error_code& error)
{
    if (!error)
    {
      // Connection accepted, start reading data from the client
      std::shared_ptr<Session> session 
        = std::make_shared<Session>(shared_from_this(),
                                    std::move(*socket_),
                                    session_counter_);
      sessions_[session_counter_] = session;
      std::cout << "start session ID : " << session_counter_ << std::endl;
      session->start();
      session_counter_++;
    }

    // Accept another connection
    start_accept();
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        // Create and run the server
        std::shared_ptr<TCPServer> pServer 
          = std::make_shared<TCPServer>(io_context, std::atoi(argv[1]));

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
