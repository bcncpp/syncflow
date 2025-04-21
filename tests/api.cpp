// udt_socket_test.cpp

#include <gtest/gtest.h>
#include <memory>
#include <set>
#include <condition_variable>
#include <mutex>

// Include your actual class header here
#include "api.h"

// Basic smoke test
TEST(UDTSocketTest, ConstructAndDestruct) {
    EXPECT_NO_THROW({
        udt_socket socket;
    });
}

// Ensure socket cannot be copied
TEST(UDTSocketTest, IsNotCopyable) {
    static_assert(!std::is_copy_constructible_v<udt_socket>, "udt_socket should not be copy constructible");
    static_assert(!std::is_copy_assignable_v<udt_socket>, "udt_socket should not be copy assignable");
}

// Test default values in constructor
TEST(UDTSocketTest, DefaultInitialization) {
    udt_socket socket;

    // Access private members via friend class or wrapper if needed
    // For this example, let's assume udt_socket has some getters for test visibility
    // Since you didn’t provide any, this is more of a suggestion:
    //
    // EXPECT_EQ(socket.status(), udt_status::initial);
    // EXPECT_EQ(socket.timestamp(), 0);
}

// Test queued socket containers
TEST(UDTSocketTest, QueuedSocketsInitiallyEmpty) {
    udt_socket socket;

    // Here we assume access via some friend test wrapper or reflection.
    // In practice, you might want to expose protected methods or use friend test classes
    // for unit testing internals (GoogleTest supports `FRIEND_TEST` macro)
}

// Optional: Test thread-safety constructs don't deadlock on destruction
TEST(UDTSocketTest, ThreadPrimitivesSafeOnDestruct) {
    std::unique_ptr<udt_socket> socket = std::make_unique<udt_socket>();
    // Let it go out of scope to test destruction safety
}

