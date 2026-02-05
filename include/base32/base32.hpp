#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/*! \mainpage base32
 *
 * \section intro_sec Introduction
 *
 * C++ 23 base 32 encoding and decoding library
 *
 */

/*! \brief namespace that contains base32 encoding and decoding functions
 *
 */

namespace base32 {
  /*!
   * \brief The error enum
   */
  enum class Error: uint8_t {
    NoError = 0,
    InvalidB32Input,
    MaxLengthExceeded,
    EmptyString
  };

  /*!
   * \brief bytes alias
   */
  using Bytes = std::vector<uint8_t>;

  /*! \brief Encode bytes as base 32 string
   *
   *  The encoding process represents 40-bit groups of input bits as output strings of 8 encoded characters.
   *
   *  \param userData: max size is 64 MB
   *  \param errCode
   *  \return base32 encoded string
   *
   *  \callgraph
   *  \callergraph
   */
  std::string encode(const Bytes& userData, Error& errCode);


  /*! \brief Decode bytes as base 32 string
   *
   *
   *  \param userData encoded base 32 string
   *  \param errCode
   *  \return base32 encoded string
   *
   *  \callgraph
   *  \callergraph
   */
  Bytes decode(std::string_view userData, Error& errCode);
}  // namespace base32
