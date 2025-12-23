#pragma once
#include <stdint.h>

#include <string>

namespace base32 {
  enum class error {
    NO_ERROR = 0,
    INVALID_B32_INPUT,
    MEMORY_ALLOCATION_ERROR,
    INVALID_USER_INPUT,
    EMPTY_STRING
  };

  char *encode(const uint8_t *user_data, size_t data_len, error *err_code);

  uint8_t *decode(const char *user_data_untrimmed, size_t data_len, error *err_code);
}  // namespace base32
