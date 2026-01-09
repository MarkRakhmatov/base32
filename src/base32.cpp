#include "base32/base32.hpp"

#include <cstdlib>
#include <cstring>
#include <array>
#include <limits>
#include <ranges>

constexpr uint8_t BITS_PER_BYTE = 8;
constexpr uint8_t BITS_PER_B32_BLOCK = 5;

// 64 MB should be more than enough
constexpr size_t MAX_ENCODE_INPUT_LEN = 64 * 1024 * 1024;

// if 64 MB of data is encoded than it should be also possible to decode it. That's why a bigger
// input is allowed for decoding
constexpr size_t MAX_DECODE_BASE32_INPUT_LEN = ((MAX_ENCODE_INPUT_LEN * 8 + 4) / 5);

constexpr std::array<uint8_t, 33> b32_alphabet{"ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"};

constexpr std::array<uint8_t, 128> build_positions_in_alphabet() {
  std::array<uint8_t, 128> table{};
  table.fill(std::numeric_limits<uint8_t>::max());

  for(size_t i = 0; i < std::size(b32_alphabet); ++i) {
    table[b32_alphabet[i]] = (uint8_t)i;
  }

  return table;
}

constexpr std::array<uint8_t, 128> positions_in_alphabet = build_positions_in_alphabet();

constexpr std::array<bool, 128> build_alphabet_lookup_table() {
  std::array<bool, 128> table{};
  for (const uint8_t p: b32_alphabet) {
    table[p] = 1;
  }
  table['='] = 1;

  return table;
}

constexpr std::array<bool, 128> alphabet_lookup_table = build_alphabet_lookup_table();


constexpr bool in_alphabet(char c) {
  if (c < 0) {
    return false;
  }

  return alphabet_lookup_table[c];
}

namespace base32 {

  // The encoding process represents 40-bit groups of input bits as output strings of 8 encoded
  // characters. The input data must be null terminated.
  std::string encode(const Bytes&user_data, error &err_code) {
    if (user_data.size() > MAX_ENCODE_INPUT_LEN) {
      err_code = error::MAX_LENGTH_EXCEEDED;
      return {};
    }

    size_t user_data_chars = user_data.size();
    size_t total_bits = user_data_chars*8;
    int num_of_equals = 0;
    switch (total_bits % 40) {
      case 8:
        num_of_equals = 6;
        break;
      case 16:
        num_of_equals = 4;
        break;
      case 24:
        num_of_equals = 3;
        break;
      case 32:
        num_of_equals = 1;
        break;
    }

    const size_t output_length = (user_data_chars * 8 + 4) / 5;
    std::string encoded_data(output_length + num_of_equals, '\0');

    for (size_t i = 0, j = 0; i < user_data_chars; i += 5) {
      uint64_t quintuple = 0;

      for (size_t k = 0; k < 5; k++) {
        quintuple = (quintuple << 8) | (i + k < user_data_chars ? user_data[i + k] : 0);
      }

      for (int shift = 35; shift >= 0; shift -= 5) {
        encoded_data[j++] = static_cast<char>(b32_alphabet[(quintuple >> shift) & 0x1F]);
      }
    }

    for (int i = 0; i < num_of_equals; i++) {
      encoded_data[output_length + i] = '=';
    }

    err_code = error::NO_ERROR;

    return encoded_data;
  }

  Bytes decode(std::string_view user_data, error &err_code) {
    if (user_data.size() > MAX_DECODE_BASE32_INPUT_LEN) {
      err_code = error::MAX_LENGTH_EXCEEDED;
      return {};
    }

    size_t user_data_chars = user_data.size();
    for (auto chr: user_data | std::views::reverse) {
      // As it's not known whether data_len is with or without the +1 for the null byte, a manual
      // check is required.
      if (chr == '=' || chr == '\0') {
        user_data_chars -= 1;
      } else {
        break;
      }
    }

    const size_t output_length = user_data_chars > 0? (5*user_data_chars - 4)/8: 0;  // round up
    Bytes decoded_data;
    decoded_data.reserve(output_length);

    uint8_t mask{0};
    uint8_t current_byte{0};
    uint8_t bits_left{8};
    for (size_t i = 0; i < user_data_chars; i++) {
      if (user_data[i] == ' ') {
        continue;
      }
      else if (!in_alphabet(user_data[i]))
      {
        err_code = error::INVALID_B32_INPUT;
        return {};
      }
      const uint8_t char_index = positions_in_alphabet[user_data[i]];
      if (bits_left > BITS_PER_B32_BLOCK) {
        mask = char_index << (bits_left - BITS_PER_B32_BLOCK);
        current_byte |= mask;
        bits_left -= BITS_PER_B32_BLOCK;
      } else {
        mask = char_index >> (BITS_PER_B32_BLOCK - bits_left);
        current_byte |= mask;
        decoded_data.push_back(current_byte);
        current_byte = (char_index << (BITS_PER_BYTE - BITS_PER_B32_BLOCK + bits_left));
        bits_left += BITS_PER_BYTE - BITS_PER_B32_BLOCK;
      }
    }

    err_code = error::NO_ERROR;

    return decoded_data;
  }
}  // namespace base32
