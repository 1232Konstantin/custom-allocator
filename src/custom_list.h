#ifndef __CUSTOM_LIST_H_5SFYQIQ4OKQ3__
#define __CUSTOM_LIST_H_5SFYQIQ4OKQ3__

#include <iterator>
#include <memory>
#include <type_traits>

namespace custom {

/**
 * @brief Cyclic double linked list with custom allocator support
 *
 * @tparam T - stored elements type
 */
template <typename T, typename Allocator = std::allocator<T>>
class list {
  static_assert(std::is_default_constructible_v<T>,
                "Type must me default constructable");

  struct node {
    node() = default;
    node(const T &data) : data(data) {}

    node *next{nullptr};
    node *prev{nullptr};
    T data{};
  };

  template <typename node_type>
  struct node_iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;

    node_iterator(node_type *ptr) : ptr{ptr} {}

    auto operator*() noexcept -> reference { return ptr->data; }

    auto operator++() noexcept -> node_iterator & {
      ptr = ptr->next;
      return *this;
    }

    auto operator--() noexcept -> node_iterator & {
      ptr = ptr->prev;
      return *this;
    }

    auto operator++(int) noexcept -> node_iterator {
      auto ret = *this;
      ptr = ptr->next;
      return ret;
    }

    auto operator--(int) noexcept -> node_iterator {
      auto ret = *this;
      ptr = ptr->prev;
      return ret;
    }

    bool operator==(const node_iterator &other) const noexcept {
      return ptr == other.ptr;
    }

    bool operator!=(const node_iterator &other) const noexcept {
      return !(*this == other);
    }

    node_type *ptr;
  };

  using NodeAllocator =
      typename std::allocator_traits<Allocator>::template rebind_alloc<node>;
  using NodeAllocatorTraits =
      typename std::allocator_traits<Allocator>::template rebind_traits<node>;

 public:
  using iterator = node_iterator<node>;
  using const_iterator = node_iterator<const node>;

  list(const Allocator &allocator = Allocator{}) noexcept
      : _block(), _allocator(allocator) {
    _block.next = &_block;
    _block.prev = &_block;
  };

  ~list() {
    for (auto *block = _block.next; block != &_block;) {
      auto *next = block->next;
      NodeAllocatorTraits::deallocate(_allocator, block, 1);
      block = next;
    }
  }

  list(const list<T> &other) = delete;             // Not implemented
  list &operator=(const list<T> &other) = delete;  // Not implemented
  list(list<T> &&other) = delete;                  // Not implemented
  list &operator=(list<T> &&other) = delete;       // Not implemented

  auto begin() noexcept -> iterator { return {_block.next}; }
  auto begin() const noexcept -> const_iterator { return {_block.next}; }

  auto end() noexcept -> iterator { return {&_block}; }
  auto end() const noexcept -> const_iterator { return {&_block}; }

  void push_back(const T &data) {
    auto *new_node = NodeAllocatorTraits::allocate(_allocator, 1);  // NOLINT
    NodeAllocatorTraits::construct(_allocator, new_node, data);

    _block.prev->next = new_node;
    new_node->prev = _block.prev;
    _block.prev = new_node;
    new_node->next = &_block;
  }

  bool empty() const noexcept { return _block.next == &_block; }

  auto size() const noexcept -> std::size_t {
    return std::distance(begin(), end());
  }

  auto get_allocator() const noexcept -> const NodeAllocator & { return _allocator; }

 private:
  node _block{};
  NodeAllocator _allocator;
};

}  // namespace custom

#endif  // __CUSTOM_LIST_H_5SFYQIQ4OKQ3__
