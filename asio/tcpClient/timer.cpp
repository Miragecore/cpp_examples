#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer/timer.hpp>
#include <iostream>
#include <mutex>
#include <deque>
#include <atomic>

#define DEBUG
#ifdef DEBUG
#define DEBUG_MSG(x) std::cout << x << std::endl;
#else
#define DEBUG_MSG
#endif


using namespace boost::timer;
using namespace std::chrono_literals;
namespace asio = boost::asio;

using tcp = asio::ip::tcp;
using SEND_BUF_QUE = std::deque<std::vector<uint8_t>>;

class TcpClient {
public:
  enum class State {
    IDLE,
    CONNECTING,
    CONNECTED,
    FAIL,
    IO_DONE
  };

  TcpClient(): socket_(io_),
    resolver_(io_),
    state_(State::IDLE){
  }

  virtual ~TcpClient(){
    dispose();
  }

  void setFailCallback(std::function<void(const boost::system::error_code& ec, const std::string& what)> cb){
    on_fail_cb_ = cb;
  }

  void setReceiveCallback(std::function<void(const std::vector<uint8_t>&)> cb){
    on_receive_cb_ = cb;  
  }

  void run(){
    DEBUG_MSG(__func__);
    tryConnect();
    io_thread_ = std::make_shared<std::thread>(std::move(std::thread([&]() { io_.run();  state_ = State::IO_DONE;})));
  }

  void enque(const std::vector<uint8_t>& buf){
    std::scoped_lock _lock(mtx_);
    size_t que_size = send_buf_que_.size();

    if(que_size > 10){
      boost::system::error_code ec;
      onFail(ec, "Que Overflow");
    }

    send_buf_que_.push_back(buf);

    if(que_size == 0){ // que was empty
      write();
    }
  }

  std::atomic<State> state_;

protected:

  void onFail(const boost::system::error_code &ec,
              const std::string &what) {
    state_ = State::FAIL;
    DEBUG_MSG("OnFail" << ec.message() << what);
    if (on_fail_cb_) {
      on_fail_cb_(ec, what);
    }
  }

  void read(){
    if(state_ != State::CONNECTED){
      return;
    }
    asio::async_read(socket_,
                     recv_buf_,
                     asio::transfer_at_least(4),
                     std::bind(&TcpClient::onReceive,
                               this,
                               std::placeholders::_1,
                               std::placeholders::_2)); 
  }

  virtual void onReceive(const boost::system::error_code &ec,
                                  const std::size_t& len) {
    DEBUG_MSG(__func__);
    (void)len;
    if(ec){
      onFail(ec, "onReceive");
      return;
    }

    //method 1.
    //std::vector<uint8_t> buf(len);
    //std::istream is(&recv_buf_);
    //is.read(reinterpret_cast<char *>(&buf[0]), len);
    //method 2.
    size_t buf_size = recv_buf_.size();
    std::vector<uint8_t> buf(buf_size);

    std::memcpy(buf.data(), boost::asio::buffer_cast<const char*>(recv_buf_.data()), buf_size);
    recv_buf_.consume(buf_size);

    if(on_receive_cb_){
      on_receive_cb_(buf);
    }

    read();
  }

  std::deque<std::vector<uint8_t>> send_buf_que_;
  asio::streambuf recv_buf_;

  std::function<void(std::vector<uint8_t>&)> on_receive_cb_;
  std::function<void(const boost::system::error_code& ec, const std::string& what)> on_fail_cb_;

private:

  void dispose(){
    if(!io_.stopped()){
      io_.stop();
    }
    if(io_thread_){
      if(io_thread_->joinable()){
        io_thread_->join();
        io_thread_.reset();
      }
    }
  }

  void tryConnect(){
    DEBUG_MSG(__func__);
    state_ = State::CONNECTING;
    std::string host = "localhost";
    std::string port = "12341";
    resolver_.async_resolve(host, port, [&](const boost::system::error_code& ec, asio::ip::tcp::resolver::iterator ep_iter){
      if (!ec) {
        std::cout << "Resolved the domain to: " << std::endl;
        for (auto it = ep_iter; it != tcp::resolver::iterator(); ++it) {
          std::cout << it->endpoint() << std::endl;
          connect(it);
          break;
        }
      } else {
        std::cout << "Error resolving domain: " << ec.message() << std::endl;
      }
    });
  }

  void connect(tcp::resolver::iterator ep_iter){
    DEBUG_MSG(__func__);
    asio::async_connect(socket_,
                        ep_iter,
                        std::bind(&TcpClient::onConnect,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
  }

  void onConnect(const boost::system::error_code& ec,
                 tcp::resolver::iterator ep_iter){
    DEBUG_MSG(__func__);
    (void)ep_iter;
    if(ec){
      onFail(ec, "onConnect");
      return;
    }
    state_ = State::CONNECTED;
    read();
  }

  void write(){
    DEBUG_MSG(__func__);
    if(state_ != State::CONNECTED){
      return;
    }

    asio::async_write(socket_,
                      asio::buffer(send_buf_que_.front()),
                      std::bind(&TcpClient::onWrite,
                                this,
                                std::placeholders::_1,
                                std::placeholders::_2));
  }

  void onWrite(const boost::system::error_code &ec,
               const std::size_t&) {
    DEBUG_MSG(__func__);
    if (ec) {
      return onFail(ec, "onWrite");
    }

    std::scoped_lock _lock(mtx_);
    send_buf_que_.pop_front();
    if (!send_buf_que_.empty()) {
      write();
    }
  }

  asio::io_context io_;
  std::shared_ptr<std::thread> io_thread_;

  tcp::socket socket_;
  tcp::resolver resolver_;

  std::mutex mtx_;
};

class BClient : public TcpClient{

public : 
  BClient(){
  }

  virtual ~BClient(){
  }

  virtual void onReceive(const boost::system::error_code &ec,
                                  const std::size_t& len) override{
    DEBUG_MSG(__func__ << "ov");
    (void)len;
    if(ec){
      onFail(ec, "onReceive - override");
      return;
    }
    DEBUG_MSG("onReceive override");

    //method 1.
    //std::vector<uint8_t> buf(len);
    //std::istream is(&recv_buf_);
    //is.read(reinterpret_cast<char *>(&buf[0]), len);
    //method 2.
    size_t buf_size = recv_buf_.size();
    std::vector<uint8_t> buf(buf_size);

    std::memcpy(buf.data(), boost::asio::buffer_cast<const char*>(recv_buf_.data()), buf_size);
    recv_buf_.consume(buf_size);

    if(on_receive_cb_){
      on_receive_cb_(buf);
    }

    read();
  }
};

int main(){
  std::shared_ptr<BClient> te = std::make_shared<BClient>();
  te->run();

  while(1);

  std::cout << "end" << std::endl;


  return 0;
}
