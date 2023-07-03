#ifndef __CUSTOM_ALLOCATOR_H_VWON6HLFFEGV__
#define __CUSTOM_ALLOCATOR_H_VWON6HLFFEGV__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <new>
#include <queue>
#include <type_traits>

namespace custom {

template <typename T>
class monotonic_allocator {
 public:
  using value_type = T;

  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;

  monotonic_allocator(std::size_t size) : _buffer(size) {}

  auto allocate(std::size_t size) -> T* {
    if (size * sizeof(T) > _buffer.size() - _allocated) {
      throw std::bad_alloc{};
    }

    auto* ptr = reinterpret_cast<T*>(_buffer.data()) + _allocated;  // NOLINT
    _allocated += sizeof(T) * size;
    return ptr;
  }

  void deallocate(T* /*ptr*/, std::size_t /*size*/) noexcept {
    // do nothing
  }

 private:
  std::size_t _allocated = 0;
  std::vector<std::byte> _buffer;
};

enum class pool_strategy { grow, throw_bad_alloc };

template <typename T, std::size_t BlockSize = 200,
          pool_strategy Strategy = pool_strategy::throw_bad_alloc>
struct memory_pool {
 private:
  struct memory_block {
    std::array<std::byte, BlockSize> data = {std::byte{0x00}};
  };

 public:
  using value_type = T;

  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;

  memory_pool(std::size_t initial_blocks_count) noexcept
      : _free_blocks(), _slab(initial_blocks_count) {
    for (auto& block : _slab) {
      _free_blocks.push(&block);
    }
  }

  auto allocate(std::size_t size) -> T* {
    if (sizeof(T) * size > BlockSize) {
      throw std::bad_alloc{};
    }

    if (has_free_blocks()) {
      auto free = get_free();
      return reinterpret_cast<T*>(free->data.data());  // NOLINT
    }

    if constexpr (Strategy == pool_strategy::grow) {
      auto new_block = create_block();
      return reinterpret_cast<T*>(new_block->data.data());  // NOLINT
    }

    throw std::bad_alloc{};
  }

  auto free_blocks() const noexcept -> std::size_t {
    return _free_blocks.size();
  }

  auto deallocate(T* ptr, std::size_t /*size*/) noexcept {
    auto byte_ptr = reinterpret_cast<std::byte*>(ptr);  // NOLINT
    auto block =
        std::find_if(_slab.begin(), _slab.end(), [byte_ptr](const auto& block) {
          return byte_ptr >= block.data.data() &&
                 byte_ptr <= block.data.data() + BlockSize;
        });
    _free_blocks.push(&*block);
  }

  template <typename U>
  struct rebind {
    using other = memory_pool<U, BlockSize, Strategy>;
  };

 private:
  auto get_free() noexcept -> memory_block* {
    if (has_free_blocks()) {
      auto* block = _free_blocks.front();
      _free_blocks.pop();
      return block;
    }

    return nullptr;
  }

  bool has_free_blocks() const noexcept { return !_free_blocks.empty(); }

  auto create_block() {
    _slab.push_back({});
    return _slab.end() - 1;
  }

  std::queue<memory_block*> _free_blocks;
  std::deque<memory_block> _slab;
};

template <typename T, typename U, std::size_t BlockSize>
auto operator==(const memory_pool<T, BlockSize>& lhs,
                const memory_pool<U, BlockSize>& rhs) noexcept -> bool {
  return lhs._slab.data() == rhs._slab.data();
}

template <typename T, typename U, std::size_t BlockSize>
auto operator!=(const memory_pool<T, BlockSize>& lhs,
                const memory_pool<U, BlockSize>& rhs) noexcept -> bool {
  return !(lhs == rhs);
}

}  // namespace custom

#endif  // __CUSTOM_ALLOCATOR_H_VWON6HLFFEGV__
