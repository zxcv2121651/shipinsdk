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
        // Using a raw pointer here since shared_from_this() requires the pool itself to be owned by a shared_ptr,
        // which might not be the case in all SDK lifecycles. It is the responsibility of the SDK
        // engine to ensure the ObjectPool outlives the inflight frames.
        ObjectPool<T>* raw_pool = this;

        return std::shared_ptr<T>(obj.release(), [raw_pool](T* ptr) {
            std::unique_ptr<T> uptr(ptr); // ensure cleanup
            if (raw_pool) {
                raw_pool->release(std::move(uptr));
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
