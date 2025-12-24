#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>


namespace base32 {
  enum class error: uint8_t {
    NO_ERROR = 0,
    INVALID_B32_INPUT,
    MEMORY_ALLOCATION_ERROR,
    INVALID_USER_INPUT,
    EMPTY_STRING
  };

  using Bytes = std::vector<uint8_t>;

  std::string encode(const Bytes&user_data, error *err_code);

  Bytes decode(std::string_view user_data, error *err_code);
}  // namespace base32
