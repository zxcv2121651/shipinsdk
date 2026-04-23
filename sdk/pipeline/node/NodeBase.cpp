#include "NodeBase.h"

namespace sdk {
namespace pipeline {

std::atomic<uint64_t> NodeBase::id_generator_{0};

NodeBase::NodeBase(const std::string& name, size_t queue_capacity)
    : name_(name), queue_capacity_(queue_capacity) {
    id_ = ++id_generator_;
    input_queue_ = base::CreateLockFreeQueue();
}

NodeBase::~NodeBase() {
    stop();
}

std::string NodeBase::getName() const {
    return name_;
}

uint64_t NodeBase::getId() const {
    return id_;
}

base::SDKError NodeBase::initialize() {
    if (state_.load() != base::SDKState::UNINITIALIZED) {
        return base::SDKError::ERR_INVALID_STATE;
    }

    auto q_config = base::ILockFreeQueue::Builder()
                        .setCapacity(queue_capacity_)
                        .setDropOldestOnFull(true) // Prevent OOM snowballing by default
                        .build();

    base::SDKError err = input_queue_->initialize(q_config);
    if (err != base::SDKError::OK) {
        return err;
    }

    state_.store(base::SDKState::READY);
    return base::SDKError::OK;
}

base::SDKError NodeBase::start() {
    if (state_.load() != base::SDKState::READY) {
        return base::SDKError::ERR_INVALID_STATE;
    }
    state_.store(base::SDKState::RUNNING);
    return base::SDKError::OK;
}

base::SDKError NodeBase::stop() {
    base::SDKState expected_running = base::SDKState::RUNNING;
    base::SDKState expected_ready = base::SDKState::READY;

    if (state_.compare_exchange_strong(expected_running, base::SDKState::STOPPED) ||
        state_.compare_exchange_strong(expected_ready, base::SDKState::STOPPED)) {

        input_queue_->stop();

        std::lock_guard<std::mutex> lock(topology_mutex_);
        downstream_nodes_.clear();
    }
    return base::SDKError::OK;
}

base::SDKState NodeBase::getState() const {
    return state_.load();
}

base::SDKError NodeBase::addDownstreamNode(std::shared_ptr<INode> downstream_node) {
    if (!downstream_node) {
        return base::SDKError::ERR_INVALID_PARAM;
    }

    std::lock_guard<std::mutex> lock(topology_mutex_);
    // Prevent self-loops
    if (downstream_node->getId() == this->id_) {
        return base::SDKError::ERR_INVALID_PARAM;
    }
    downstream_nodes_.push_back(downstream_node);
    return base::SDKError::OK;
}

base::SDKError NodeBase::onInputFrame(std::shared_ptr<core::MediaFrame> frame) {
    if (state_.load() != base::SDKState::RUNNING) {
        return base::SDKError::ERR_INVALID_STATE;
    }
    // Push the frame into this node's internal queue.
    // The execution thread assigned to this node will pop it later.
    return input_queue_->push(frame);
}

base::SDKError NodeBase::tryFetchInputFrame(std::shared_ptr<core::MediaFrame>& out_frame) {
    if (state_.load() != base::SDKState::RUNNING) {
        return base::SDKError::ERR_INVALID_STATE;
    }
    return input_queue_->try_pop(out_frame);
}

base::SDKError NodeBase::dispatchToDownstream(std::shared_ptr<core::MediaFrame> frame) {
    if (state_.load() != base::SDKState::RUNNING) {
        return base::SDKError::ERR_INVALID_STATE;
    }

    std::lock_guard<std::mutex> lock(topology_mutex_);
    for (auto& node : downstream_nodes_) {
        // Zero-copy fan-out: we pass the same shared_ptr to multiple downstream queues.
        // The underlying MediaFrame (and its RHITexture) is shared.
        node->onInputFrame(frame);
    }
    return base::SDKError::OK;
}

} // namespace pipeline
} // namespace sdk
