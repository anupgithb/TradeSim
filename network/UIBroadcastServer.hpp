// // network/UIBroadcastServer.hpp
// #pragma once

// #include <boost/asio.hpp>
// #include <boost/beast/websocket.hpp>
// #include <boost/beast/core.hpp>
// #include <set>
// #include <memory>
// #include <mutex>
// #include <string>

// namespace net = boost::asio;
// namespace beast = boost::beast;
// namespace ws = beast::websocket;
// using tcp = net::ip::tcp;

// /// WebSocket server that broadcasts JSON text messages to all connected clients.
// class UIBroadcastServer
// {
// public:
//     /// Construct on given io_context and TCP port
//     UIBroadcastServer(net::io_context &ioc, unsigned short port);
//     ~UIBroadcastServer();

//     /// Start accepting connections (non-blocking)
//     void run();

//     /// Broadcast a text message (JSON) to all sessions
//     void broadcast(const std::string &message);

// private:
//     /// Begin accepting incoming TCP connections
//     void doAccept();
//     /// Handler for a completed accept
//     void onAccept(beast::error_code ec, tcp::socket socket);

//     net::io_context &ioc_;
//     tcp::acceptor acceptor_;
//     std::set<std::shared_ptr<ws::stream<tcp::socket>>> sessions_;
//     std::mutex sessionsMutex_;
// };

// network/UIBroadcastServer.hpp

#pragma once

#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <nlohmann/json.hpp>
#include <set>
#include <memory>
#include <mutex>
#include <functional>
#include <string>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace ws = beast::websocket;
using tcp = net::ip::tcp;

/// WebSocket server that broadcasts JSON text messages to all connected clients,
/// and can receive control messages of type "settings" from them.
class UIBroadcastServer
{
public:
    /// Construct on given io_context and TCP port
    UIBroadcastServer(net::io_context &ioc, unsigned short port);
    ~UIBroadcastServer();

    /// Start accepting connections (non-blocking)
    void run();

    /// Broadcast a text message (JSON) to all sessions
    void broadcast(const std::string &message);

    /// Register a callback for incoming control messages (type="settings")
    void setOnControl(std::function<void(const nlohmann::json &)> cb)
    {
        controlCallback_ = std::move(cb);
    }

private:
    /// Begin accepting incoming TCP connections
    void doAccept();

    /// Handler for a completed accept
    void onAccept(beast::error_code ec, tcp::socket socket);

    /// Begin an async read for control messages on a session
    void doRead(std::shared_ptr<ws::stream<tcp::socket>> session);

    net::io_context &ioc_;
    tcp::acceptor acceptor_;

    // Active WebSocket sessions
    std::set<std::shared_ptr<ws::stream<tcp::socket>>> sessions_;
    std::mutex sessionsMutex_;

    // Callback invoked on control messages
    std::function<void(const nlohmann::json &)> controlCallback_;
};
