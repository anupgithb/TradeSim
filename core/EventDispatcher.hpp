#pragma once

#include <mutex>
#include <vector>
#include <functional>

/// A thread-safe, in-process publish/subscribe dispatcher for events of type T.
template <typename T>
class EventDispatcher
{
public:
    /// Handler signature for event callbacks.
    using Handler = std::function<void(const T &)>;

    /// Subscribe to this event. Handlers will be invoked in the publisher's thread.
    void subscribe(const Handler &handler)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        handlers_.push_back(handler);
    }

    /// Publish an event to all subscribers.
    void publish(const T &event)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto &handler : handlers_)
        {
            handler(event);
        }
    }

private:
    std::mutex mutex_;
    std::vector<Handler> handlers_;
};