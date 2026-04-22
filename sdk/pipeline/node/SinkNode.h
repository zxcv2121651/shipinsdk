#pragma once

#include "NodeBase.h"

namespace sdk {
namespace pipeline {

/**
 * @class SinkNode
 * @brief Represents the exit point of the DAG (e.g., Display, Hardware Encoder).
 *        Sink nodes consume frames but do not dispatch them further downstream.
 */
class SinkNode : public NodeBase {
public:
    explicit SinkNode(const std::string& name, size_t queue_capacity = 15)
        : NodeBase(name, queue_capacity) {}
    virtual ~SinkNode() = default;

    // Override to prevent adding downstream nodes, since Sinks are dead ends.
    base::SDKError addDownstreamNode(std::shared_ptr<INode> downstream_node) override {
        return base::SDKError::ERR_INVALID_STATE;
    }
};

} // namespace pipeline
} // namespace sdk
