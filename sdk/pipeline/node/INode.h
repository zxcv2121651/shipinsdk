#pragma once

#include <memory>
#include <string>
#include "../../core/MediaFrame.h"
#include "../../base/error/SDKError.h"
#include "../../base/state/SDKState.h"

namespace sdk {
namespace pipeline {

/**
 * @class INode
 * @brief Pure virtual interface for all elements in the DAG Pipeline Engine.
 *        Ensures ABI stability across boundaries.
 */
class INode {
public:
    virtual ~INode() = default;

    // --- Identification ---
    virtual std::string getName() const = 0;
    virtual uint64_t getId() const = 0;

    // --- Lifecycle ---
    virtual base::SDKError initialize() = 0;
    virtual base::SDKError start() = 0;
    virtual base::SDKError stop() = 0;
    virtual base::SDKState getState() const = 0;

    // --- Graph Topology (DAG Fan-out) ---
    /**
     * @brief Connects this node's output to another node's input.
     * @param downstream_node The node that will receive frames from this node.
     */
    virtual base::SDKError addDownstreamNode(std::shared_ptr<INode> downstream_node) = 0;

    // --- Data Flow ---
    /**
     * @brief Called by an upstream node (or external source) to push a frame into this node's queue.
     * @thread_safety Thread-Safe
     */
    virtual base::SDKError onInputFrame(std::shared_ptr<MediaFrame> frame) = 0;
};

} // namespace pipeline
} // namespace sdk
