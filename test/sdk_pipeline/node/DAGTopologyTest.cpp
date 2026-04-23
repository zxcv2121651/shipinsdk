#include <gtest/gtest.h>
#include "../../../sdk/pipeline/node/SourceNode.h"
#include "../../../sdk/pipeline/node/SinkNode.h"

using namespace sdk::pipeline;
using namespace sdk::base;

class TestSourceNode : public SourceNode {
public:
    TestSourceNode() : SourceNode("TestSource") {}

    SDKError produceFrame(int pts) {
        auto frame = std::make_shared<sdk::core::MediaFrame>();
        frame->pts = pts;
        return emitFrame(frame);
    }
};

class TestSinkNode : public SinkNode {
public:
    TestSinkNode() : SinkNode("TestSink") {}

    SDKError processFrame(std::shared_ptr<sdk::core::MediaFrame> frame) override {
        last_pts_ = frame->pts;
        frames_processed_++;
        return SDKError::OK;
    }

    // Helper to simulate worker thread popping and processing
    void drainQueue() {
        std::shared_ptr<sdk::core::MediaFrame> frame;
        while(tryFetchInputFrame(frame) == SDKError::OK) {
            processFrame(frame);
        }
    }

    int last_pts_ = -1;
    int frames_processed_ = 0;
};

TEST(DAGTopologyTest, SourceToSinkFanOut) {
    auto source = std::make_shared<TestSourceNode>();
    auto sink1 = std::make_shared<TestSinkNode>();
    auto sink2 = std::make_shared<TestSinkNode>();

    ASSERT_EQ(source->initialize(), SDKError::OK);
    ASSERT_EQ(sink1->initialize(), SDKError::OK);
    ASSERT_EQ(sink2->initialize(), SDKError::OK);

    // Build Topology: Source -> [Sink1, Sink2]
    ASSERT_EQ(source->addDownstreamNode(sink1), SDKError::OK);
    ASSERT_EQ(source->addDownstreamNode(sink2), SDKError::OK);

    // Start all nodes
    ASSERT_EQ(source->start(), SDKError::OK);
    ASSERT_EQ(sink1->start(), SDKError::OK);
    ASSERT_EQ(sink2->start(), SDKError::OK);

    // Produce a frame from the source
    ASSERT_EQ(source->produceFrame(1024), SDKError::OK);

    // Simulate consumers draining their queues
    sink1->drainQueue();
    sink2->drainQueue();

    // Verify fan-out success
    EXPECT_EQ(sink1->frames_processed_, 1);
    EXPECT_EQ(sink1->last_pts_, 1024);

    EXPECT_EQ(sink2->frames_processed_, 1);
    EXPECT_EQ(sink2->last_pts_, 1024);

    // Prevent Source from accepting external inputs directly
    auto illegal_frame = std::make_shared<sdk::core::MediaFrame>();
    EXPECT_EQ(source->onInputFrame(illegal_frame), SDKError::ERR_INVALID_STATE);

    // Prevent Sink from accepting downstream nodes
    auto sink3 = std::make_shared<TestSinkNode>();
    EXPECT_EQ(sink1->addDownstreamNode(sink3), SDKError::ERR_INVALID_STATE);
}

TEST(DAGTopologyTest, PreventSelfLoop) {
    auto sink = std::make_shared<TestSinkNode>();
    ASSERT_EQ(sink->initialize(), SDKError::OK);
    // Sink prevents topology additions anyway, but base validation applies
    // Create a generic NodeBase derived class to test self-loop specifically if needed,
    // but we can just use source for this test.
    auto source = std::make_shared<TestSourceNode>();
    ASSERT_EQ(source->initialize(), SDKError::OK);
    EXPECT_EQ(source->addDownstreamNode(source), SDKError::ERR_INVALID_PARAM);
}
