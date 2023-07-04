#include <algorithm>
#include <iterator>

#include <gtest/gtest.h>

#include "custom_allocator.h"
#include "custom_list.h"

TEST(CustomListTest, CreateEmptyList) {  // NOLINT
  custom::list<int> list{};
  ASSERT_TRUE(list.empty());
}

TEST(CustomListTest, PushBack) {  // NOLINT
  custom::list<int> list{};
  ASSERT_TRUE(list.empty());
  ASSERT_EQ(list.size(), 0);

  list.push_back(42);
  ASSERT_FALSE(list.empty());
  ASSERT_EQ(*list.begin(), 42);
  ASSERT_EQ(list.size(), 1);

  auto begin = list.begin();

  list.push_back(43);
  ASSERT_EQ(*++begin, 43);
  ASSERT_EQ(list.size(), 2);

  list.push_back(44);
  ASSERT_EQ(*++begin, 44);
  ASSERT_EQ(list.size(), 3);

  std::vector<int> data_from_list{};
  std::copy(list.begin(), list.end(), std::back_inserter(data_from_list));

  std::vector<int> expected = {42, 43, 44};
  ASSERT_EQ(data_from_list, expected);
}

TEST(CustomListTest, CreateWithBlockAllocator) {  // NOLINT
  constexpr auto BLOCKS = 100;

  using custom_int_allocator = custom::memory_pool<int>;
  custom::list<int, custom_int_allocator> list_with_allocator{
      custom_int_allocator{BLOCKS}};

  auto &allocator = list_with_allocator.get_allocator();
  ASSERT_EQ(allocator.free_blocks(), BLOCKS);

  list_with_allocator.push_back(0x42);
  ASSERT_EQ(allocator.free_blocks(), BLOCKS - 1);
}