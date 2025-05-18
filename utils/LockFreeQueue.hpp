#pragma once

#include <atomic>
#include <vector>
#include <optional>
#include <cstddef>

/// A single-producer, single-consumer lock-free queue.
/// T must be MoveConstructible.
template <typename T>
class LockFreeQueue
{
public:
    explicit LockFreeQueue(size_t capacity = 1024)
        : capacity_(capacity), buffer_(capacity), head_(0), tail_(0) {}

    /// Push an element. Returns false if the queue is full.
    bool push(T &&item)
    {
        const size_t head = head_.load(std::memory_order_relaxed);
        const size_t next = increment(head);
        if (next == tail_.load(std::memory_order_acquire))
        {
            return false; // full
        }
        buffer_[head] = std::move(item);
        head_.store(next, std::memory_order_release);
        return true;
    }

    /// Pop an element. Returns false if the queue is empty.
    bool pop(T &item)
    {
        const size_t tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire))
        {
            return false; // empty
        }
        item = std::move(*buffer_[tail]);
        buffer_[tail].reset();
        tail_.store(increment(tail), std::memory_order_release);
        return true;
    }

private:
    size_t increment(size_t idx) const noexcept
    {
        return (idx + 1) % capacity_;
    }

    const size_t capacity_;
    std::vector<std::optional<T>> buffer_;
    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;
};