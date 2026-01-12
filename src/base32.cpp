#include "base32/base32.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <array>
#include <limits>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

constexpr uint8_t BITS_PER_BYTE = 8;
constexpr uint8_t BYTES_PER_B32_BLOCK = 5;
constexpr uint8_t BITS_PER_B32_BLOCK = BITS_PER_BYTE*BYTES_PER_B32_BLOCK;

constexpr uint8_t B32_PADDING_6 = 6;
constexpr uint8_t B32_PADDING_4 = 4;
constexpr uint8_t B32_PADDING_3 = 3;
constexpr uint8_t B32_PADDING_1 = 1;

// 64 MB should be more than enough
constexpr size_t MAX_ENCODE_INPUT_LEN = 64ULL * 1024 * 1024;

// if 64 MB of data is encoded than it should be also possible to decode it. That's why a bigger
// input is allowed for decoding
constexpr size_t MAX_DECODE_BASE32_INPUT_LEN = ((MAX_ENCODE_INPUT_LEN * BITS_PER_BYTE + 4) / BYTES_PER_B32_BLOCK);

constexpr std::array<uint8_t, 33> b32_alphabet{"ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"};
constexpr uint8_t MAX_ALPHABET_POSIONS = 128;
constexpr std::array<uint8_t, MAX_ALPHABET_POSIONS> build_positions_in_alphabet() {
  std::array<uint8_t, MAX_ALPHABET_POSIONS> table{};
  table.fill(std::numeric_limits<uint8_t>::max());

  for(size_t i = 0; i < std::size(b32_alphabet); ++i) {
    table.at(b32_alphabet.at(i)) = (uint8_t)i;
  }

  return table;
}

constexpr std::array<uint8_t, MAX_ALPHABET_POSIONS> positions_in_alphabet = build_positions_in_alphabet();

constexpr std::array<bool, MAX_ALPHABET_POSIONS> build_alphabet_lookup_table() {
  std::array<bool, MAX_ALPHABET_POSIONS> table{};
  for (const uint8_t pos: b32_alphabet) {
    table.at(pos) = true;
  }
  table['='] = true;

  return table;
}

constexpr std::array<bool, MAX_ALPHABET_POSIONS> alphabet_lookup_table = build_alphabet_lookup_table();


constexpr bool in_alphabet(char val) {
  if (val < 0) {
    return false;
  }

  return alphabet_lookup_table.at(val);
}

namespace base32 {

  // The encoding process represents 40-bit groups of input bits as output strings of 8 encoded
  // characters.
  std::string encode(const Bytes&user_data, error &err_code) {
    if (user_data.size() > MAX_ENCODE_INPUT_LEN) {
      err_code = error::MAX_LENGTH_EXCEEDED;
      return {};
    }

    const size_t user_data_chars = user_data.size();
    const size_t total_bits = user_data_chars*8;
    uint8_t num_of_equals = 0;
    switch (total_bits % BITS_PER_B32_BLOCK) {
      case 0:
        break;
      case BITS_PER_BYTE:
        num_of_equals = B32_PADDING_6;
        break;
      case uint8_t(2*BITS_PER_BYTE):
        num_of_equals = B32_PADDING_4;
        break;
      case uint8_t(3*BITS_PER_BYTE):
        num_of_equals = B32_PADDING_3;
        break;
      case uint8_t(4*BITS_PER_BYTE):
        num_of_equals = B32_PADDING_1;
        break;
      default:
        std::unreachable();
    }
    const size_t output_length = (user_data_chars * 8 + 4) / BYTES_PER_B32_BLOCK;
    std::string encoded_data(output_length + num_of_equals, '\0');

    for (size_t i = 0, j = 0; i < user_data_chars; i += BYTES_PER_B32_BLOCK) {
      uint64_t quintuple = 0;

      for (size_t k = 0; k < BYTES_PER_B32_BLOCK; k++) {
        quintuple = (quintuple << BITS_PER_BYTE) | (i + k < user_data_chars ? user_data.at(i + k) : 0U);
      }

      constexpr uint64_t mask = 0x1F;

      for (int8_t shift = BITS_PER_B32_BLOCK - BYTES_PER_B32_BLOCK; shift >= 0; shift -= BYTES_PER_B32_BLOCK) {
        encoded_data.at(j++) = static_cast<char>(b32_alphabet.at((quintuple >> static_cast<uint8_t>(shift)) & mask));
      }
    }

    for (uint8_t i = 0; i < num_of_equals; i++) {
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
    uint8_t bits_left{BITS_PER_BYTE};
    for (size_t i = 0; i < user_data_chars; i++) {
      if (user_data[i] == ' ') {
        continue;
      }
      else if (!in_alphabet(user_data[i]))
      {
        err_code = error::INVALID_B32_INPUT;
        return {};
      }
      const uint8_t char_index = positions_in_alphabet.at(user_data[i]);
      if (bits_left > BYTES_PER_B32_BLOCK) {
        mask = char_index << static_cast<uint8_t>(bits_left - BYTES_PER_B32_BLOCK);
        current_byte |= mask;
        bits_left -= BYTES_PER_B32_BLOCK;
      } else {
        mask = char_index >> static_cast<uint8_t>(BYTES_PER_B32_BLOCK - bits_left);
        current_byte |= mask;
        decoded_data.push_back(current_byte);
        current_byte = (char_index << static_cast<uint8_t>(BITS_PER_BYTE - BYTES_PER_B32_BLOCK + bits_left));
        bits_left += BITS_PER_BYTE - BYTES_PER_B32_BLOCK;
      }
    }

    err_code = error::NO_ERROR;

    return decoded_data;
  }
}  // namespace base32
