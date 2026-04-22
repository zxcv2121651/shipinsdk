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
    std::atomic_flag spinlock = ATOMIC_FLAG_INIT;

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

    // Given the extreme complexity of building a true Lock-Free MPMC queue with hazard pointers
    // from scratch, and avoiding data races with std::shared_ptr instances (as noted in review),
    // we use a lightweight SpinLock for the MVP to satisfy both performance and absolute safety.
    // (Tasks CON-01, CON-02: Use SpinLock for sub-microsecond waits to prevent context switches).
    while (pimpl_->spinlock.test_and_set(std::memory_order_acquire)) {
        // spin
    }

    size_t current_tail = pimpl_->tail.load(std::memory_order_relaxed);
    size_t next_tail = (current_tail + 1) % cap;
    size_t current_head = pimpl_->head.load(std::memory_order_relaxed);

    if (next_tail == current_head) {
        if (pimpl_->config.drop_oldest_on_full) {
            // Drop oldest safely under lock
            pimpl_->buffer[current_head].reset(); // Free memory
            pimpl_->head.store((current_head + 1) % cap, std::memory_order_relaxed);
        } else {
            pimpl_->spinlock.clear(std::memory_order_release);
            return SDKError::ERR_QUEUE_FULL;
        }
    }

    // Write and advance tail
    pimpl_->buffer[current_tail] = std::move(frame);
    pimpl_->tail.store(next_tail, std::memory_order_release);

    pimpl_->spinlock.clear(std::memory_order_release);

    return SDKError::OK;
}

SDKError LockFreeQueue::try_pop(std::shared_ptr<MediaFrame>& out_frame) {
    if (!pimpl_->validateState(SDKState::RUNNING)) {
        return SDKError::ERR_INVALID_STATE;
    }

    while (pimpl_->spinlock.test_and_set(std::memory_order_acquire)) {
        // spin
    }

    size_t current_head = pimpl_->head.load(std::memory_order_relaxed);

    if (current_head == pimpl_->tail.load(std::memory_order_acquire)) {
        pimpl_->spinlock.clear(std::memory_order_release);
        return SDKError::ERR_QUEUE_EMPTY;
    }

    size_t cap = pimpl_->config.capacity + 1;
    out_frame = std::move(pimpl_->buffer[current_head]);
    pimpl_->head.store((current_head + 1) % cap, std::memory_order_release);

    pimpl_->spinlock.clear(std::memory_order_release);
    return SDKError::OK;
}

void LockFreeQueue::stop() {
    SDKState expected_running = SDKState::RUNNING;
    SDKState expected_ready = SDKState::READY;

    if (pimpl_->state.compare_exchange_strong(expected_running, SDKState::STOPPED, std::memory_order_release) ||
        pimpl_->state.compare_exchange_strong(expected_ready, SDKState::STOPPED, std::memory_order_release)) {

        while (pimpl_->spinlock.test_and_set(std::memory_order_acquire)) {
            // spin
        }

        // Clear references safely under lock
        for(auto& slot : pimpl_->buffer) {
            slot.reset();
        }

        pimpl_->spinlock.clear(std::memory_order_release);
    }
}

size_t LockFreeQueue::getMemoryFootprint() const {
    size_t size = sizeof(LockFreeQueue) + sizeof(Impl);
    // Add dynamically allocated buffer size
    size += pimpl_->buffer.capacity() * sizeof(std::shared_ptr<MediaFrame>);
    return size;
}

// Exported factory
std::unique_ptr<ILockFreeQueue> CreateLockFreeQueue() {
    return std::make_unique<LockFreeQueue>();
}

} // namespace base
} // namespace sdk
