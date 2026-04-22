#pragma once

#include "ILockFreeQueue.h"

namespace sdk {
namespace base {

/**
 * @class LockFreeQueue
 * @brief Concrete implementation of ILockFreeQueue using the PIMPL idiom.
 */
class LockFreeQueue : public ILockFreeQueue {
public:
    LockFreeQueue();
    ~LockFreeQueue() override;

    // Delete copy and move semantics to prevent accidental misuse
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;
    LockFreeQueue(LockFreeQueue&&) = delete;
    LockFreeQueue& operator=(LockFreeQueue&&) = delete;

    SDKError initialize(const Config& config) override;
    SDKError push(std::shared_ptr<MediaFrame> frame) override;
    SDKError try_pop(std::shared_ptr<MediaFrame>& out_frame) override;
    void stop() override;
    size_t getMemoryFootprint() const override;

private:
    // PIMPL idiom: Hide implementation details to reduce compile times
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace base
} // namespace sdk
