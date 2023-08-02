#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <utility>

#include <custom_allocator.h>
#include <custom_list.h>
#include <utils.h>

int main() {
  constexpr auto SIZE = 10;

  {
    std::map<int, int> non_allocator_map;
    for (uint32_t key = 0; key < SIZE; key++) {
      non_allocator_map[key] = utils::factorial(key);  // NOLINT
    }
  }

  {
    using PairAllocator = custom::memory_pool<std::pair<const int, int>>;
    using CustomAllocatorMap = std::map<int, int, std::less<>, PairAllocator>;

    CustomAllocatorMap allocator_map{PairAllocator{SIZE}};
    for (uint32_t key = 0; key < SIZE; key++) {
      allocator_map[key] = utils::factorial(key);  // NOLINT
    }

    for (const auto &[key, value] : allocator_map) {
      std::cout << key << ' ' << value << std::endl;
    }
  }

  {
    custom::list<int> non_allocator_list;
    std::generate_n(std::back_inserter(non_allocator_list), 10,
                    [counter = 0]() mutable { return counter++; });
  }

  {
    using IntAllocator = custom::memory_pool<int>;
    custom::list<int, IntAllocator> allocator_list{IntAllocator{SIZE}};
    std::generate_n(std::back_inserter(allocator_list), 10,
                    [counter = 0]() mutable { return counter++; });

    for (const auto &elem : allocator_list) {
      std::cout << elem << std::endl;
    }
  }
}