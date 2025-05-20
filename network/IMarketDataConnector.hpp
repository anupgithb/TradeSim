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
