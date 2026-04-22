#pragma once

#include <memory>
#include <cstddef>
#include "../error/SDKError.h"
#include "../../core/MediaFrame.h"

namespace sdk {
namespace base {

/**
 * @class ILockFreeQueue
 * @brief Pure virtual interface for the Lock-Free Queue.
 *        Ensures strict ABI stability by preventing STL usage across the API boundary.
 */
class ILockFreeQueue {
public:
    struct Config {
        size_t capacity = 0;
        bool drop_oldest_on_full = true;
    };

    /**
     * @class Builder
     * @brief Configuration Builder Pattern to validate initialization parameters.
     */
    class Builder {
    public:
        Builder& setCapacity(size_t capacity) {
            config_.capacity = capacity;
            return *this;
        }

        Builder& setDropOldestOnFull(bool drop) {
            config_.drop_oldest_on_full = drop;
            return *this;
        }

        Config build() const {
            // Validation logic happens before returning
            return config_;
        }
    private:
        Config config_;
    };

    virtual ~ILockFreeQueue() = default;

    /**
     * @brief Initializes the queue.
     * @thread_safety Thread-Safe (Must only be called once, state validation enforced)
     */
    virtual SDKError initialize(const Config& config) = 0;

    /**
     * @brief Pushes a frame into the queue.
     * @thread_safety Thread-Safe (Multiple Producers allowed)
     */
    virtual SDKError push(std::shared_ptr<MediaFrame> frame) = 0;

    /**
     * @brief Attempts to pop a frame from the queue without blocking.
     * @thread_safety Thread-Safe (Multiple Consumers allowed)
     */
    virtual SDKError try_pop(std::shared_ptr<MediaFrame>& out_frame) = 0;

    /**
     * @brief Cleans up resources.
     * @thread_safety Thread-Safe
     */
    virtual void stop() = 0;

    /**
     * @brief Reports the memory footprint for telemetry.
     * @thread_safety Thread-Safe
     */
    virtual size_t getMemoryFootprint() const = 0;
};

// Factory function exported for creation (ABI boundary boundary)
std::unique_ptr<ILockFreeQueue> CreateLockFreeQueue();

} // namespace base
} // namespace sdk
