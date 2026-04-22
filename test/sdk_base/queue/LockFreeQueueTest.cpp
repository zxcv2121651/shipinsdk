#include <gtest/gtest.h>
#include "../../../sdk/base/queue/ILockFreeQueue.h"

using namespace sdk::base;

class LockFreeQueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        queue = CreateLockFreeQueue();
    }

    std::unique_ptr<ILockFreeQueue> queue;
};

TEST_F(LockFreeQueueTest, FailsWithoutInit) {
    auto frame = std::make_shared<MediaFrame>();
    EXPECT_EQ(queue->push(frame), SDKError::ERR_INVALID_STATE);

    std::shared_ptr<MediaFrame> out;
    EXPECT_EQ(queue->try_pop(out), SDKError::ERR_INVALID_STATE);
}

TEST_F(LockFreeQueueTest, InitValidatesCapacity) {
    auto config = ILockFreeQueue::Builder().setCapacity(0).build();
    EXPECT_EQ(queue->initialize(config), SDKError::ERR_INVALID_PARAM);
}

TEST_F(LockFreeQueueTest, DoubleInitFails) {
    auto config = ILockFreeQueue::Builder().setCapacity(10).build();
    EXPECT_EQ(queue->initialize(config), SDKError::OK);
    EXPECT_EQ(queue->initialize(config), SDKError::ERR_INVALID_STATE);
}

TEST_F(LockFreeQueueTest, PushAndPopWorks) {
    auto config = ILockFreeQueue::Builder().setCapacity(2).build();
    ASSERT_EQ(queue->initialize(config), SDKError::OK);

    auto frame1 = std::make_shared<MediaFrame>(); frame1->pts = 100;
    EXPECT_EQ(queue->push(frame1), SDKError::OK);

    std::shared_ptr<MediaFrame> out;
    EXPECT_EQ(queue->try_pop(out), SDKError::OK);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->pts, 100);

    // Queue is now empty
    EXPECT_EQ(queue->try_pop(out), SDKError::ERR_QUEUE_EMPTY);
}

TEST_F(LockFreeQueueTest, DropOldestStrategyWorks) {
    auto config = ILockFreeQueue::Builder()
                    .setCapacity(2)
                    .setDropOldestOnFull(true)
                    .build();
    ASSERT_EQ(queue->initialize(config), SDKError::OK);

    auto frame1 = std::make_shared<MediaFrame>(); frame1->pts = 1;
    auto frame2 = std::make_shared<MediaFrame>(); frame2->pts = 2;
    auto frame3 = std::make_shared<MediaFrame>(); frame3->pts = 3;

    EXPECT_EQ(queue->push(frame1), SDKError::OK);
    EXPECT_EQ(queue->push(frame2), SDKError::OK);

    // Pushing third frame into capacity=2 with drop_oldest=true should succeed and drop frame1
    EXPECT_EQ(queue->push(frame3), SDKError::OK);

    std::shared_ptr<MediaFrame> out;
    EXPECT_EQ(queue->try_pop(out), SDKError::OK);
    EXPECT_EQ(out->pts, 2); // Frame 1 was dropped, so oldest is now Frame 2

    EXPECT_EQ(queue->try_pop(out), SDKError::OK);
    EXPECT_EQ(out->pts, 3);
}

TEST_F(LockFreeQueueTest, RejectOnFullStrategyWorks) {
    auto config = ILockFreeQueue::Builder()
                    .setCapacity(2)
                    .setDropOldestOnFull(false)
                    .build();
    ASSERT_EQ(queue->initialize(config), SDKError::OK);

    auto frame1 = std::make_shared<MediaFrame>();
    auto frame2 = std::make_shared<MediaFrame>();
    auto frame3 = std::make_shared<MediaFrame>();

    EXPECT_EQ(queue->push(frame1), SDKError::OK);
    EXPECT_EQ(queue->push(frame2), SDKError::OK);

    // Pushing third frame should fail
    EXPECT_EQ(queue->push(frame3), SDKError::ERR_QUEUE_FULL);
}
