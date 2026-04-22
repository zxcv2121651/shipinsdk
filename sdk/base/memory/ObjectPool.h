#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <functional>

namespace sdk {
namespace base {

/**
 * @class ObjectPool
 * @brief Thread-safe object pool to eliminate dynamic allocation overhead on the hot path (MEM-03).
 */
template <typename T>
class ObjectPool : public std::enable_shared_from_this<ObjectPool<T>> {
public:
    explicit ObjectPool(size_t preallocate_count = 0) {
        for (size_t i = 0; i < preallocate_count; ++i) {
            pool_.push_back(std::make_unique<T>());
        }
    }

    /**
     * @brief Acquires an object from the pool.
     *        When the returned shared_ptr goes out of scope, it is returned to the pool automatically.
     */
    std::shared_ptr<T> acquire() {
        std::unique_ptr<T> obj;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!pool_.empty()) {
                obj = std::move(pool_.back());
                pool_.pop_back();
            }
        }

        // If pool is empty, allocate a new one (happens outside lock to minimize contention)
        if (!obj) {
            obj = std::make_unique<T>();
        }

        // Create a custom deleter that returns the object to this pool.
        // Capturing a weak_ptr to the pool ensures safety if the pool is destroyed before the object.
        std::weak_ptr<ObjectPool<T>> weak_pool;
        try {
            weak_pool = this->weak_from_this();
        } catch (const std::bad_weak_ptr&) {
            // Fallback for cases where the pool was allocated on the stack (non-shared ownership).
            // It is unsafe to use the pool after it goes out of scope in this mode.
        }

        ObjectPool<T>* raw_pool = this;

        return std::shared_ptr<T>(obj.release(), [weak_pool, raw_pool](T* ptr) {
            std::unique_ptr<T> uptr(ptr); // ensure cleanup
            if (auto pool = weak_pool.lock()) {
                pool->release(std::move(uptr));
            } else {
                // If weak_ptr couldn't be obtained initially, fallback to raw pointer assumption.
                // In a production codebase, ObjectPool MUST be allocated via std::make_shared.
                if (raw_pool && weak_pool.expired() == false) {
                   // Only execute if not previously assigned and expired
                   raw_pool->release(std::move(uptr));
                }
            }
        });
    }

private:
    void release(std::unique_ptr<T> obj) {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.push_back(std::move(obj));
    }

    std::mutex mutex_;
    std::vector<std::unique_ptr<T>> pool_;
};

} // namespace base
} // namespace sdk
