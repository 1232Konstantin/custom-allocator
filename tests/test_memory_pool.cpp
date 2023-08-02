#include <memory>
#include <new>
#include <type_traits>

#include <custom_allocator.h>
#include <gtest/gtest.h>

class MemoryPoolTest : public ::testing::Test {
 public:
  static constexpr auto BLOCKS_COUNT = 2;
  static constexpr auto BLOCK_SIZE = 300;
  custom::memory_pool<uint8_t, BLOCK_SIZE> allocator{BLOCKS_COUNT};
};

using namespace ::testing;

// NOLINTNEXTLINE
TEST_F(MemoryPoolTest, AllocateMemory) {
  ASSERT_TRUE(allocator.allocate(1) != nullptr);
  ASSERT_TRUE(allocator.allocate(BLOCK_SIZE) != nullptr);
}

// NOLINTNEXTLINE
TEST_F(MemoryPoolTest, DeallocateMemory) {
  auto *block_1 = allocator.allocate(1);
  ASSERT_EQ(allocator.free_blocks(), BLOCKS_COUNT - 1);

  allocator.deallocate(block_1, 1);
  ASSERT_EQ(allocator.free_blocks(), BLOCKS_COUNT);
}

// NOLINTNEXTLINE
TEST_F(MemoryPoolTest, OnThrowStrategy_ThrowBadAlloc_WhenNoSpaceAvailable) {
  ASSERT_TRUE(allocator.allocate(1) != nullptr);
  ASSERT_TRUE(allocator.allocate(BLOCK_SIZE) != nullptr);
  ASSERT_THROW(allocator.allocate(1), std::bad_alloc);
}

// NOLINTNEXTLINE
TEST_F(MemoryPoolTest, OnGrowStrategy_AllocateNewBlocks_WhenNoSpaceAvailable) {
  auto allocator =
      custom::memory_pool<uint8_t, BLOCK_SIZE, custom::pool_strategy::grow>{
          BLOCKS_COUNT};
  ASSERT_TRUE(allocator.allocate(1) != nullptr);
  ASSERT_TRUE(allocator.allocate(BLOCK_SIZE) != nullptr);
  ASSERT_TRUE(allocator.allocate(BLOCK_SIZE) != nullptr);
  ASSERT_TRUE(allocator.allocate(BLOCK_SIZE) != nullptr);
}

// NOLINTNEXTLINE
TEST_F(MemoryPoolTest, IfAllocatesBiggerThanBlockSize_ThrowBadAlloc) {
  EXPECT_THROW(allocator.allocate(BLOCK_SIZE + 1), std::bad_alloc);
}

// NOLINTNEXTLINE
TEST_F(MemoryPoolTest, SatisfyAllocatorTraits) {
  constexpr auto TEST_VAL = 0x42;

  using custom_allocator_traits = std::allocator_traits<decltype(allocator)>;

  auto *memory = custom_allocator_traits::allocate(allocator, 1);
  custom_allocator_traits::construct(allocator, memory, TEST_VAL);

  ASSERT_EQ(*memory, TEST_VAL);

  custom_allocator_traits::destroy(allocator, memory);
  custom_allocator_traits::deallocate(allocator, memory, 1);

  using rebinded = custom_allocator_traits::rebind_alloc<float>;
  ASSERT_TRUE(
      (std::is_same_v<rebinded, custom::memory_pool<float, BLOCK_SIZE>>));

  ASSERT_TRUE((std::is_same_v<
               custom_allocator_traits::propagate_on_container_copy_assignment,
               std::true_type>));
  ASSERT_TRUE((std::is_same_v<
               custom_allocator_traits::propagate_on_container_copy_assignment,
               std::true_type>));
}
