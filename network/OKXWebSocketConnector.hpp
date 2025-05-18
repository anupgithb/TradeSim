// /*
// #pragma once
// #include "network/IMarketDataConnector.hpp"
// #include "utils/LockFreeQueue.hpp"
// #include <boost/asio.hpp>
// #include <boost/beast.hpp>
// #include <boost/beast/websocket.hpp>
// #include <atomic>
// #include <thread>

// namespace net = boost::asio;
// namespace beast = boost::beast;
// using tcp = net::ip::tcp;

// class OKXWebSocketConnector : public IMarketDataConnector
// {
// public:
//     OKXWebSocketConnector(LockFreeQueue<std::string> &queue);
//     ~OKXWebSocketConnector();

//     void start(JsonCallback onMsg) override;
//     void stop() override;

// private:
//     void run();

//     LockFreeQueue<std::string> &rawQueue_;
//     std::atomic<bool> running_{false};
//     std::thread thread_;

//     // Beast/Asio objects
//     net::io_context ioc_;
//     tcp::resolver resolver_{ioc_};
//     beast::websocket::stream<beast::tcp_stream> ws_{ioc_};
//     std::string host_ = "ws.gomarket-cpp.goquant.io";
//     std::string port_ = "443";
//     std::string target_ = "/ws/l2-orderbook/okx/BTC-USDT-SWAP";
// };
// */
// #pragma once

// #include "utils/LockFreeQueue.hpp"
// #include <boost/asio/strand.hpp>
// #include <boost/asio/ssl/context.hpp>
// #include <boost/beast/websocket/ssl.hpp>

// class OKXWebSocketConnector
// {
// public:
//     using JsonCallback = std::function<void(const std::string &)>;

//     explicit OKXWebSocketConnector(LockFreeQueue<std::string> &queue);
//     ~OKXWebSocketConnector();

//     void start(JsonCallback cb = nullptr);
//     void stop();

// private:
//     void run();

//     LockFreeQueue<std::string> &rawQueue_;
//     boost::asio::io_context ioc_;
//     boost::asio::ip::tcp::resolver resolver_;

//     // SSL/TLS context
//     boost::asio::ssl::context sslCtx_;
//     // WebSocket over TLS
//     boost::beast::websocket::stream<
//         boost::beast::ssl_stream<
//             boost::asio::ip::tcp::socket>>
//         ws_;

//     std::string host_, port_, target_;
//     std::thread thread_;
//     std::atomic<bool> running_{false};
// };

#pragma once

#include "network/IMarketDataConnector.hpp"
#include "utils/LockFreeQueue.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <string>
#include <thread>
#include <atomic>

namespace ssl = boost::asio::ssl;
namespace websocket = boost::beast::websocket;
using tcp = boost::asio::ip::tcp;

class OKXWebSocketConnector : public IMarketDataConnector
{
public:
    using JsonCallback = IMarketDataConnector::JsonCallback;

    explicit OKXWebSocketConnector(LockFreeQueue<std::string> &queue);
    ~OKXWebSocketConnector() override;

    void start(JsonCallback cb = nullptr) override;
    void stop() override;

private:
    void run();

    LockFreeQueue<std::string> &rawQueue_;
    boost::asio::io_context ioc_;
    tcp::resolver resolver_;
    ssl::context sslCtx_;
    websocket::stream<ssl::stream<tcp::socket>> ws_;

    std::string host_;   // e.g. "ws.okx.com"
    std::string port_;   // e.g. "8443"
    std::string target_; // e.g. "/ws/v5/public"

    std::thread thread_;
    std::atomic<bool> running_{false};
};