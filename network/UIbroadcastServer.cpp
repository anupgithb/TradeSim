// // network/UIBroadcastServer.cpp
// #include "network/UIBroadcastServer.hpp"
// #include <boost/asio/dispatch.hpp>
// #include <boost/beast/websocket.hpp>
// #include <boost/beast/core.hpp>
// #include <iostream>

// UIBroadcastServer::UIBroadcastServer(net::io_context &ioc, unsigned short port)
//     : ioc_(ioc), acceptor_(ioc_, tcp::endpoint(tcp::v4(), port))
// {
// }

// UIBroadcastServer::~UIBroadcastServer()
// {
//     // Close acceptor to stop new connections
//     beast::error_code ec;
//     acceptor_.close(ec);
// }

// void UIBroadcastServer::run()
// {
//     doAccept();
// }

// void UIBroadcastServer::doAccept()
// {
//     acceptor_.async_accept(
//         net::make_strand(ioc_),
//         beast::bind_front_handler(&UIBroadcastServer::onAccept, this));
// }

// void UIBroadcastServer::onAccept(beast::error_code ec, tcp::socket socket)
// {
//     if (ec)
//     {
//         std::cerr << "[UI] Accept error: " << ec.message() << std::endl;
//     }
//     else
//     {
//         // Create WebSocket session
//         auto ws = std::make_shared<ws::stream<tcp::socket>>(std::move(socket));
//         // Accept WebSocket handshake
//         ws->set_option(ws::stream_base::timeout::suggested(beast::role_type::server));
//         ws->accept(ec);
//         if (ec)
//         {
//             std::cerr << "[UI] WebSocket accept error: " << ec.message() << std::endl;
//         }
//         else
//         {
//             // Add to session set
//             std::lock_guard<std::mutex> lock(sessionsMutex_);
//             sessions_.insert(ws);
//         }
//     }
//     // Continue accepting
//     doAccept();
// }

// void UIBroadcastServer::broadcast(const std::string &message)
// {
//     std::lock_guard<std::mutex> lock(sessionsMutex_);
//     for (auto it = sessions_.begin(); it != sessions_.end();)
//     {
//         auto ws = *it;
//         try
//         {
//             ws->text(true);
//             ws->write(net::buffer(message));
//             ++it;
//         }
//         catch (const beast::system_error &se)
//         {
//             // Remove closed sessions
//             std::cerr << "[UI] Write error, removing session: " << se.code().message() << std::endl;
//             it = sessions_.erase(it);
//         }
//     }
// }

// network/UIBroadcastServer.cpp

#include "network/UIBroadcastServer.hpp"
#include <boost/asio/dispatch.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace ws = beast::websocket;
using tcp = net::ip::tcp;

UIBroadcastServer::UIBroadcastServer(net::io_context &ioc, unsigned short port)
    : ioc_(ioc),
      acceptor_(ioc_, tcp::endpoint(tcp::v4(), port))
{
}

UIBroadcastServer::~UIBroadcastServer()
{
    // Stop accepting new connections
    beast::error_code ec;
    acceptor_.close(ec);
}

void UIBroadcastServer::run()
{
    doAccept();
}

void UIBroadcastServer::doAccept()
{
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(&UIBroadcastServer::onAccept, this));
}

void UIBroadcastServer::onAccept(beast::error_code ec, tcp::socket socket)
{
    if (ec)
    {
        std::cerr << "[UI] Accept error: " << ec.message() << std::endl;
    }
    else
    {
        auto ws = std::make_shared<ws::stream<tcp::socket>>(std::move(socket));

        // Perform the WebSocket handshake
        ws->set_option(ws::stream_base::timeout::suggested(beast::role_type::server));
        ws->accept(ec);
        if (ec)
        {
            std::cerr << "[UI] WebSocket accept error: " << ec.message() << std::endl;
        }
        else
        {
            // Store session and start reading control messages
            {
                std::lock_guard<std::mutex> lock(sessionsMutex_);
                sessions_.insert(ws);
            }
            doRead(ws);
        }
    }

    // Continue accepting new clients
    doAccept();
}

void UIBroadcastServer::doRead(std::shared_ptr<ws::stream<tcp::socket>> ws)
{
    auto buffer = std::make_shared<beast::flat_buffer>();
    ws->async_read(
        *buffer,
        [this, ws, buffer](beast::error_code ec, std::size_t)
        {
            if (ec)
            {
                // Remove closed sessions
                std::lock_guard<std::mutex> lock(sessionsMutex_);
                sessions_.erase(ws);
                return;
            }

            // Parse incoming message as JSON control
            std::string msg = beast::buffers_to_string(buffer->data());
            try
            {
                auto j = nlohmann::json::parse(msg);
                if (j.contains("type") && j["type"] == "settings" && controlCallback_)
                {
                    controlCallback_(j["params"]);
                }
            }
            catch (const std::exception &ex)
            {
                std::cerr << "[UI] Invalid JSON control message: " << ex.what() << std::endl;
            }

            // Continue reading
            doRead(ws);
        });
}

void UIBroadcastServer::broadcast(const std::string &message)
{
    std::lock_guard<std::mutex> lock(sessionsMutex_);
    for (auto it = sessions_.begin(); it != sessions_.end();)
    {
        auto ws = *it;
        try
        {
            ws->text(true);
            ws->write(net::buffer(message));
            ++it;
        }
        catch (const beast::system_error &se)
        {
            std::cerr << "[UI] Write error, removing session: " << se.code().message() << std::endl;
            it = sessions_.erase(it);
        }
    }
}
