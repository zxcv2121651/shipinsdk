#pragma once

#include "NodeBase.h"

namespace sdk {
namespace pipeline {

/**
 * @class SourceNode
 * @brief Represents the entry point of the DAG (e.g., Camera, Microphone, File Demuxer).
 *        Source nodes typically do not receive input frames via `onInputFrame`,
 *        but rather generate them internally via hardware callbacks and dispatch them downstream.
 */
class SourceNode : public NodeBase {
public:
    explicit SourceNode(const std::string& name) : NodeBase(name, 1) {}
    virtual ~SourceNode() = default;

    // Source nodes usually ignore external inputs
    base::SDKError onInputFrame(std::shared_ptr<core::MediaFrame> frame) override {
        return base::SDKError::ERR_INVALID_STATE;
    }

protected:
    // Source nodes don't process external queue frames
    base::SDKError processFrame(std::shared_ptr<core::MediaFrame> frame) override {
        return base::SDKError::ERR_INVALID_STATE;
    }

    /**
     * @brief Called by hardware callbacks (e.g., AVCaptureSession or Camera2).
     *        Generates the shared MediaFrame and fires it down the DAG.
     */
    base::SDKError emitFrame(std::shared_ptr<core::MediaFrame> frame) {
        return dispatchToDownstream(frame);
    }
};

} // namespace pipeline
} // namespace sdk
