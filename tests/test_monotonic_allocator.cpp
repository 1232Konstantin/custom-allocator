#include <new>

#include <gtest/gtest.h>

#include "custom_allocator.h"

class MonotonicAllocatorTest : public ::testing::Test {
 public:
  static constexpr auto ALLOCATOR_SIZE = 8;
  custom::monotonic_allocator<uint8_t> allocator{ALLOCATOR_SIZE};
};

TEST_F(MonotonicAllocatorTest, Allocate) {  // NOLINT`
  ASSERT_TRUE(allocator.allocate(4) != nullptr);
  ASSERT_TRUE(allocator.allocate(4) != nullptr);
  ASSERT_THROW(allocator.allocate(4), std::bad_alloc);
}

TEST_F(MonotonicAllocatorTest, Deallocate_ActuallyDoNothing) {  // NOLINT`
  auto *data = allocator.allocate(4);
  allocator.deallocate(data, 4);
}

TEST_F(MonotonicAllocatorTest, SatisfyAllocatorTraits) {  // NOLINT
  constexpr auto TEST_VAL = 0x42;

  using custom_allocator_traits = std::allocator_traits<decltype(allocator)>;

  auto *memory = custom_allocator_traits::allocate(allocator, 1);
  custom_allocator_traits::construct(allocator, memory, TEST_VAL);

  ASSERT_EQ(*memory, TEST_VAL);

  custom_allocator_traits::destroy(allocator, memory);
  custom_allocator_traits::deallocate(allocator, memory, 1);

  using rebinded = custom_allocator_traits::rebind_alloc<float>;
  ASSERT_TRUE((std::is_same_v<rebinded, custom::monotonic_allocator<float>>));

  ASSERT_TRUE((std::is_same_v<
               custom_allocator_traits::propagate_on_container_copy_assignment,
               std::true_type>));
  ASSERT_TRUE((std::is_same_v<
               custom_allocator_traits::propagate_on_container_copy_assignment,
               std::true_type>));
}
