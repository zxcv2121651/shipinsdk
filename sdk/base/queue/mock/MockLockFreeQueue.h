#pragma once

#include <gmock/gmock.h>
#include "../ILockFreeQueue.h"

namespace sdk {
namespace base {
namespace mock {

/**
 * @class MockLockFreeQueue
 * @brief GMock implementation of ILockFreeQueue for downstream testing.
 *        Satisfies atomic task SDK-000007.
 */
class MockLockFreeQueue : public ILockFreeQueue {
public:
    MOCK_METHOD(SDKError, initialize, (const Config& config), (override));
    MOCK_METHOD(SDKError, push, (std::shared_ptr<MediaFrame> frame), (override));
    MOCK_METHOD(SDKError, try_pop, (std::shared_ptr<MediaFrame>& out_frame), (override));
    MOCK_METHOD(void, stop, (), (override));
};

} // namespace mock
} // namespace base
} // namespace sdk
