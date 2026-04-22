#include "LockFreeQueue.h"
#include "../state/SDKState.h"
#include <atomic>
#include <vector>
#include <cassert>
#include <iostream>

namespace sdk {
namespace base {

// The actual lock-free ring buffer implementation hidden from the header.
// Note: We use alignas(64) to prevent false sharing between producer/consumer threads.
struct alignas(64) LockFreeQueue::Impl {
    Config config;
    std::atomic<SDKState> state{SDKState::UNINITIALIZED};

    // Simplistic lock-free atomic indices
    alignas(64) std::atomic<size_t> head{0};
    alignas(64) std::atomic<size_t> tail{0};

    // Internal buffer. In a real MPMC lock-free queue, this array holds atomic wrapper objects.
    std::vector<std::shared_ptr<MediaFrame>> buffer;

    bool validateState(SDKState expected) const {
        return state.load(std::memory_order_acquire) == expected;
    }
};

LockFreeQueue::LockFreeQueue() : pimpl_(std::make_unique<Impl>()) {}

LockFreeQueue::~LockFreeQueue() {
    // RAII Teardown
    stop();
}

SDKError LockFreeQueue::initialize(const Config& config) {
    if (!pimpl_->validateState(SDKState::UNINITIALIZED)) {
        std::cerr << "LockFreeQueue Error: Already initialized." << std::endl;
        return SDKError::ERR_INVALID_STATE;
    }

    if (config.capacity == 0) {
        return SDKError::ERR_INVALID_PARAM;
    }

    pimpl_->config = config;
    // To distinguish full from empty in a ring buffer, we need Capacity + 1 slots
    pimpl_->buffer.resize(config.capacity + 1);

    pimpl_->state.store(SDKState::READY, std::memory_order_release);
    pimpl_->state.store(SDKState::RUNNING, std::memory_order_release);

    return SDKError::OK;
}

SDKError LockFreeQueue::push(std::shared_ptr<MediaFrame> frame) {
    if (!pimpl_->validateState(SDKState::RUNNING)) {
        return SDKError::ERR_INVALID_STATE;
    }

    // A simplified atomic ring-buffer push concept for MVP.
    // Capacity logic needs to track N elements, so array must be N+1 to distinguish full/empty using modulo math.
    size_t cap = pimpl_->config.capacity + 1;
    size_t current_tail = pimpl_->tail.load(std::memory_order_relaxed);
    size_t next_tail = (current_tail + 1) % cap;

    if (next_tail == pimpl_->head.load(std::memory_order_acquire)) {
        if (pimpl_->config.drop_oldest_on_full) {
            // Drop oldest: advance head
            size_t current_head = pimpl_->head.load(std::memory_order_relaxed);
            pimpl_->head.compare_exchange_weak(current_head, (current_head + 1) % cap, std::memory_order_release, std::memory_order_relaxed);
        } else {
            return SDKError::ERR_QUEUE_FULL;
        }
    }

    // In a fully robust lock-free MPMC queue, slot reservation and publishing requires hazard pointers or double CAS.
    // This is the semantic placeholder demonstrating the atomic boundary logic.
    pimpl_->buffer[current_tail] = std::move(frame);
    pimpl_->tail.store(next_tail, std::memory_order_release);

    return SDKError::OK;
}

SDKError LockFreeQueue::try_pop(std::shared_ptr<MediaFrame>& out_frame) {
    if (!pimpl_->validateState(SDKState::RUNNING)) {
        return SDKError::ERR_INVALID_STATE;
    }

    size_t current_head = pimpl_->head.load(std::memory_order_relaxed);

    if (current_head == pimpl_->tail.load(std::memory_order_acquire)) {
        return SDKError::ERR_QUEUE_EMPTY;
    }

    size_t cap = pimpl_->config.capacity + 1;
    out_frame = std::move(pimpl_->buffer[current_head]);
    pimpl_->head.store((current_head + 1) % cap, std::memory_order_release);

    return SDKError::OK;
}

void LockFreeQueue::stop() {
    SDKState current = pimpl_->state.load(std::memory_order_acquire);
    if (current == SDKState::RUNNING || current == SDKState::READY) {
        pimpl_->state.store(SDKState::STOPPED, std::memory_order_release);
        // Clear references
        for(auto& slot : pimpl_->buffer) {
            slot.reset();
        }
    }
}

// Exported factory
std::unique_ptr<ILockFreeQueue> CreateLockFreeQueue() {
    return std::make_unique<LockFreeQueue>();
}

} // namespace base
} // namespace sdk
