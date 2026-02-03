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


namespace base32 {
  constexpr uint8_t gBitsPerByte = 8;
  constexpr uint8_t gBytesPerB32Block = 5;
  constexpr uint8_t gBitsPerB32Block = gBitsPerByte*gBytesPerB32Block;

  constexpr uint8_t gB32Padding6 = 6;
  constexpr uint8_t gB32Padding4 = 4;
  constexpr uint8_t gB32Padding3 = 3;
  constexpr uint8_t gB32Padding1 = 1;

  /*!
   *  \brief gMaxEncodeInputLen
   *  64 MB should be more than enough
   */
  constexpr size_t gMaxEncodeInputLen = 64ULL * 1024 * 1024;

  /*!
   * \brief gMaxDecodeBasE32InputLen
   *
   * if 64 MB of data is encoded than it should be also possible to decode it. That's why a bigger input is allowed for decoding
   */
  constexpr size_t gMaxDecodeBasE32InputLen = ((gMaxEncodeInputLen * gBitsPerByte + 4) / gBytesPerB32Block);

  constexpr std::array<uint8_t, 33> gB32Alphabet{"ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"};
  constexpr uint8_t gMaxAlphabetPosions = 128;

  /*!
   * \brief buildPositionsInAlphabet
   *
   * positions in base 32 alphabet, O(1) cache
   * \return
   */
  constexpr std::array<uint8_t, gMaxAlphabetPosions> buildPositionsInAlphabet() {
    std::array<uint8_t, gMaxAlphabetPosions> table{};
    table.fill(std::numeric_limits<uint8_t>::max());

    for(size_t i = 0; i < std::size(gB32Alphabet); ++i) {
      table.at(gB32Alphabet.at(i)) = static_cast<uint8_t>(i);
    }

    return table;
  }

  /*!
   * \brief gPositionsInAlphabet
   */
  constexpr std::array<uint8_t, gMaxAlphabetPosions> gPositionsInAlphabet = buildPositionsInAlphabet();

  /*!
   * \brief buildAlphabetLookupTable
   * \return alphabet lookup table to reuse it later
   */
  constexpr std::array<bool, gMaxAlphabetPosions> buildAlphabetLookupTable() {
    std::array<bool, gMaxAlphabetPosions> table{};
    for (const uint8_t pos: gB32Alphabet) {
      table.at(pos) = true;
    }
    table['='] = true;

    return table;
  }

  /*!
   * \brief gAlphabetLookupTable
   */
  constexpr std::array<bool, gMaxAlphabetPosions> gAlphabetLookupTable = buildAlphabetLookupTable();

  /*!
   * \brief inAlphabet
   * \param val
   * \return true if val in base 32 alphabet
   */
  constexpr bool inAlphabet(char val) {
    if (val < 0) {
      return false;
    }

    return gAlphabetLookupTable.at(val);
  }

  std::string encode(const Bytes& userData, Error &errCode) {
    if (userData.size() > gMaxEncodeInputLen) {
      errCode = Error::MaxLengthExceeded;
      return {};
    }

    const size_t userDataChars = userData.size();
    const size_t totalBits = userDataChars*8;
    uint8_t numOfEquals = 0;
    switch (totalBits % gBitsPerB32Block) {
      case 0:
        break;
      case gBitsPerByte:
        numOfEquals = gB32Padding6;
        break;
      case uint8_t(2*gBitsPerByte):
        numOfEquals = gB32Padding4;
        break;
      case uint8_t(3*gBitsPerByte):
        numOfEquals = gB32Padding3;
        break;
      case uint8_t(4*gBitsPerByte):
        numOfEquals = gB32Padding1;
        break;
      default:
        std::unreachable();
    }
    const size_t outputLength = (userDataChars * 8 + 4) / gBytesPerB32Block;
    std::string encodedData(outputLength + numOfEquals, '\0');

    for (size_t i = 0, j = 0; i < userDataChars; i += gBytesPerB32Block) {
      uint64_t quintuple = 0;

      for (size_t k = 0; k < gBytesPerB32Block; k++) {
        quintuple = (quintuple << gBitsPerByte) | (i + k < userDataChars ? userData.at(i + k) : 0U);
      }

      constexpr uint64_t mask = 0x1F;

      for (int8_t shift = gBitsPerB32Block - gBytesPerB32Block; shift >= 0; shift -= gBytesPerB32Block) {
        encodedData.at(j++) = static_cast<char>(gB32Alphabet.at((quintuple >> static_cast<uint8_t>(shift)) & mask));
      }
    }

    for (uint8_t i = 0; i < numOfEquals; i++) {
      encodedData[outputLength + i] = '=';
    }

    errCode = Error::NoError;

    return encodedData;
  }

  Bytes decode(std::string_view userData, Error &errCode) {
    if (userData.size() > gMaxDecodeBasE32InputLen) {
      errCode = Error::MaxLengthExceeded;
      return {};
    }

    size_t userDataChars = userData.size();
    for (auto chr: userData | std::views::reverse) {
      // As it's not known whether data_len is with or without the +1 for the null byte, a manual
      // check is required.
      if (chr == '=' || chr == '\0') {
        userDataChars -= 1;
      } else {
        break;
      }
    }

    const size_t outputLength = userDataChars > 0? (5*userDataChars - 4)/8: 0;  // round up
    Bytes decodedData;
    decodedData.reserve(outputLength);

    uint8_t mask{0};
    uint8_t currentByte{0};
    uint8_t bitsLeft{gBitsPerByte};
    for (size_t i = 0; i < userDataChars; i++) {
      if (userData[i] == ' ') {
        continue;
      }
      else if (!inAlphabet(userData[i]))
      {
        errCode = Error::InvalidB32Input;
        return {};
      }
      const uint8_t charIndex = gPositionsInAlphabet.at(userData[i]);
      if (bitsLeft > gBytesPerB32Block) {
        mask = charIndex << static_cast<uint8_t>(bitsLeft - gBytesPerB32Block);
        currentByte |= mask;
        bitsLeft -= gBytesPerB32Block;
      } else {
        mask = charIndex >> static_cast<uint8_t>(gBytesPerB32Block - bitsLeft);
        currentByte |= mask;
        decodedData.push_back(currentByte);
        currentByte = (charIndex << static_cast<uint8_t>(gBitsPerByte - gBytesPerB32Block + bitsLeft));
        bitsLeft += gBitsPerByte - gBytesPerB32Block;
      }
    }

    errCode = Error::NoError;

    return decodedData;
  }
}  // namespace base32
