#pragma once

#include "INode.h"
#include "../../base/queue/ILockFreeQueue.h"
#include <vector>
#include <mutex>
#include <atomic>

namespace sdk {
namespace pipeline {

/**
 * @class NodeBase
 * @brief Abstract base implementation of INode.
 *        Handles common boilerplate: state machine, downstream topology (fan-out), and input queuing.
 */
class NodeBase : public INode {
public:
    explicit NodeBase(const std::string& name, size_t queue_capacity = 15);
    virtual ~NodeBase() override;

    // --- INode Implementation ---
    std::string getName() const override;
    uint64_t getId() const override;

    base::SDKError initialize() override;
    base::SDKError start() override;
    base::SDKError stop() override;
    base::SDKState getState() const override;

    base::SDKError addDownstreamNode(std::shared_ptr<INode> downstream_node) override;
    base::SDKError onInputFrame(std::shared_ptr<MediaFrame> frame) override;

protected:
    // Core processing function to be implemented by specialized nodes (Filters, Sinks).
    // Called when a frame is popped from the internal queue.
    virtual base::SDKError processFrame(std::shared_ptr<MediaFrame> frame) = 0;

    // Dispatches the frame to all registered downstream nodes (Fan-out).
    base::SDKError dispatchToDownstream(std::shared_ptr<MediaFrame> frame);

    // Pops a frame from the internal queue for processing.
    base::SDKError tryFetchInputFrame(std::shared_ptr<MediaFrame>& out_frame);

private:
    std::string name_;
    uint64_t id_;

    std::atomic<base::SDKState> state_{base::SDKState::UNINITIALIZED};

    // Internal queue to decouple execution from upstream producers
    std::unique_ptr<base::ILockFreeQueue> input_queue_;
    size_t queue_capacity_;

    // Downstream topology (Fan-out)
    std::mutex topology_mutex_;
    std::vector<std::shared_ptr<INode>> downstream_nodes_;

    static std::atomic<uint64_t> id_generator_;
};

} // namespace pipeline
} // namespace sdk
