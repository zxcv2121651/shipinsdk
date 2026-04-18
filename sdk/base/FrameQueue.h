#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "../core/MediaFrame.h"

class FrameQueue {
public:
    explicit FrameQueue(size_t max_size = 15) : max_size_(max_size) {}

    // Push frame into the queue with a drop strategy
    void push(std::shared_ptr<MediaFrame> frame) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (queue_.size() >= max_size_) {
            // Drop oldest frame strategy for both video and audio
            // For video, we drop oldest to keep up with realtime.
            // For audio, we still drop if it overflows (ideally the max_size should be larger for audio).
            queue_.pop();
        }

        queue_.push(std::move(frame));
        cond_.notify_one();
    }

    // Blocking pop
    std::shared_ptr<MediaFrame> pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return !queue_.empty(); });

        auto frame = std::move(queue_.front());
        queue_.pop();
        return frame;
    }

    // Non-blocking try_pop
    std::shared_ptr<MediaFrame> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return nullptr;
        }

        auto frame = std::move(queue_.front());
        queue_.pop();
        return frame;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    std::queue<std::shared_ptr<MediaFrame>> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    size_t max_size_;
};
