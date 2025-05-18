// #pragma once

// #include <functional>
// #include <string>

// /// Callback type invoked on receiving raw JSON message
// using JsonCallback = std::function<void(const std::string &)>;

// /// Interface for market data connectors
// class IMarketDataConnector
// {
// public:
//     virtual ~IMarketDataConnector() = default;

//     /// Start the connector. The provided callback is invoked for each incoming message.
//     virtual void start(JsonCallback onMessage) = 0;

//     /// Stop the connector and clean up resources.
//     virtual void stop() = 0;
// };

#ifndef IMARKETDATACONNECTOR_HPP
#define IMARKETDATACONNECTOR_HPP

#include <functional>
#include <string>

class IMarketDataConnector
{
public:
    using JsonCallback = std::function<void(const std::string &)>;

    virtual ~IMarketDataConnector() = default;

    virtual void start(JsonCallback callback) = 0;
    virtual void stop() = 0;
};

#endif // IMARKETDATACONNECTOR_HPP
