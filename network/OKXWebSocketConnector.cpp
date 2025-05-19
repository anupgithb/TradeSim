// #include "network/OKXWebSocketConnector.hpp"
// #include <spdlog/spdlog.h>
// #include <boost/beast/core.hpp>
// #include <boost/beast/ssl.hpp>
// #include <boost/beast/websocket/ssl.hpp>
// #include <boost/asio/connect.hpp>
// #include <boost/asio/ssl/stream.hpp>

// namespace ssl = boost::asio::ssl;
// namespace websocket = boost::beast::websocket;
// using tcp = boost::asio::ip::tcp;

// OKXWebSocketConnector::OKXWebSocketConnector(LockFreeQueue<std::string> &queue)
//     : rawQueue_(queue),
//       ioc_(),
//       resolver_(ioc_),
//       sslCtx_(ssl::context::tlsv12_client),
//       ws_(ioc_, sslCtx_)
// {
//     host_ = "ws.okx.com";
//     port_ = "8443";
//     target_ = "/ws/v5/public";
// }

// OKXWebSocketConnector::~OKXWebSocketConnector()
// {
//     stop();
// }

// void OKXWebSocketConnector::start(JsonCallback)
// {
//     if (running_)
//         return;
//     running_ = true;
//     thread_ = std::thread(&OKXWebSocketConnector::run, this);
// }

// void OKXWebSocketConnector::stop()
// {
//     if (!running_)
//         return;
//     running_ = false;
//     ioc_.stop();
//     if (thread_.joinable())
//         thread_.join();
// }

// void OKXWebSocketConnector::run()
// {
//     try
//     {
//         // Resolve DNS
//         auto const results = resolver_.resolve(host_, port_);

//         // Connect TCP socket
//         boost::asio::connect(ws_.next_layer().next_layer(), results.begin(), results.end());

//         // Perform SSL handshake on the socket
//         ws_.next_layer().handshake(ssl::stream_base::client);

//         // Set SNI Hostname
//         SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str());

//         // Decorate WebSocket handshake
//         ws_.set_option(websocket::stream_base::decorator(
//             [](websocket::request_type &req)
//             {
//                 req.set(boost::beast::http::field::user_agent,
//                         std::string(BOOST_BEAST_VERSION_STRING) + " OKXBot");
//             }));

//         // WebSocket handshake
//         ws_.handshake(host_, target_);
//         spdlog::info("[WS] Connected & handshake succeeded");

//         // Read loop
//         beast::flat_buffer buffer;
//         while (running_)
//         {
//             ws_.read(buffer);
//             spdlog::info("[WS] Received {} bytes", buffer.size());

//             auto msg = beast::buffers_to_string(buffer.data());
//             buffer.consume(buffer.size());

//             rawQueue_.push(std::move(msg));
//             spdlog::info("[WS] Pushed tick into queue");
//         }

//         // Close cleanly
//         ws_.close(websocket::close_code::normal);
//     }
//     catch (const std::exception &e)
//     {
//         spdlog::error("[WS] Error: {}", e.what());
//     }
// }

#include "network/OKXWebSocketConnector.hpp"
#include <nlohmann/json.hpp>
#include <boost/asio/connect.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <spdlog/spdlog.h>
#include <openssl/ssl.h>

OKXWebSocketConnector::OKXWebSocketConnector(LockFreeQueue<std::string> &queue)
    : rawQueue_(queue), ioc_(), resolver_(ioc_), sslCtx_(ssl::context::tlsv12_client), ws_(ioc_, sslCtx_), host_("ws.gomarket-cpp.goquant.io"),
      port_("443"),
      target_("/ws/l2-orderbook/okx/BTC-USDT-SWAP")
{
}

OKXWebSocketConnector::~OKXWebSocketConnector()
{
    stop();
}

void OKXWebSocketConnector::start(JsonCallback)
{
    if (running_)
        return;
    running_ = true;
    thread_ = std::thread(&OKXWebSocketConnector::run, this);
}

void OKXWebSocketConnector::stop()
{
    if (!running_)
        return;
    running_ = false;
    ioc_.stop();
    if (thread_.joinable())
        thread_.join();
}

void OKXWebSocketConnector::run()
{
    try
    {
        // Resolve DNS
        auto const results = resolver_.resolve(host_, port_);

        // Connect TCP
        boost::asio::connect(
            ws_.next_layer().next_layer(),
            results.begin(), results.end());

        // Set SNI hostname (required for many modern TLS servers)
        if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str()))
        {
            throw boost::beast::system_error(
                boost::beast::error_code(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()),
                "Failed to set SNI Host Name");
        }

        // Perform TLS handshake
        ws_.next_layer().handshake(ssl::stream_base::client);

        // Set SNI Hostname
        if (!SSL_set_tlsext_host_name(
                ws_.next_layer().native_handle(), host_.c_str()))
        {
            spdlog::warn("[WS] SNI host name setup failed");
        }

        // Decorate WebSocket handshake
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::request_type &req)
            {
                req.set(boost::beast::http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) + " OKXBot");
            }));

        // WebSocket handshake
        ws_.handshake(host_, target_);
        spdlog::info("[WS] Connected & handshake succeeded");

        // // Read loop
        // boost::beast::flat_buffer buffer;
        // while (running_)
        // {
        //     ws_.read(buffer);
        //     spdlog::info("[WS] Received {} bytes", buffer.size());

        //     auto msg = boost::beast::buffers_to_string(buffer.data());
        //     buffer.consume(buffer.size());

        //     rawQueue_.push(std::move(msg));
        //     spdlog::info("[WS] Pushed tick into queue");
        // }

        // // Close cleanly
        // ws_.close(websocket::close_code::normal);

        // Send subscription message
        nlohmann::json subscribeMsg = {
            {"op", "subscribe"},
            {"args", {{{"channel", "books"}, {"instId", "BTC-USDT"}}}}};

        std::string subStr = subscribeMsg.dump();
        ws_.write(boost::asio::buffer(subStr));
        spdlog::info("[WS] Sent subscription message: {}", subStr);

        // Read loop
        boost::beast::flat_buffer buffer;
        while (running_)
        {
            ws_.read(buffer);
            spdlog::info("[WS] Received {} bytes", buffer.size());

            auto msg = boost::beast::buffers_to_string(buffer.data());
            buffer.consume(buffer.size());

            rawQueue_.push(std::move(msg));
            spdlog::info("[WS] Pushed tick into queue");
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("[WS] Error: {}", e.what());
    }
}
