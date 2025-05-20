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