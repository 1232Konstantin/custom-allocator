#ifndef __UTILS_H_2XAZB0XS455W__
#define __UTILS_H_2XAZB0XS455W__

#include <cstdint>
#include <type_traits>

namespace utils {

/**
 * @brief Factorial function only for positive numbers
 * 
 * @tparam Unsigned 
 * @tparam Require 
 * @param val 
 * @return std::size_t 
 */
template <typename Unsigned,
          typename Require = std::enable_if_t<std::is_unsigned_v<Unsigned>>>
constexpr auto factorial(Unsigned val) -> std::size_t {
  std::size_t res = 1;
  for (Unsigned i = 1; i <= val; i++) {
    res *= i;
  }
  return res;
}

}  // namespace utils

#endif  // __UTILS_H_2XAZB0XS455W__
