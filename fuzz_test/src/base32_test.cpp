#include "fuzztest/fuzztest.h"
#include "gtest/gtest.h"
#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"

#include <base32/base32.hpp>
#include <cstdlib>
#include <string_view>

TEST(Base32Suite, factorial2) {
  const char *token = "LLFTSZYMUGKHEDQBAAACAZAMUFKKVFLS";
  base32::error err{};

  const auto binary = base32::decode(token, &err);
  EXPECT_EQ(err, base32::error::NO_ERROR);

  const auto result = base32::encode(binary, &err);
  EXPECT_EQ(err,  base32::error::NO_ERROR);
  EXPECT_EQ(result,  token);
}

void base32DecodeNotCrashes(std::string_view encoded) {
  base32::error err{};
  base32::decode(encoded, &err);
}

FUZZ_TEST(Base32Suite, base32DecodeNotCrashes);

void base32EncodeNotCrashes(const base32::Bytes& bytes) {
  base32::error err{};
  base32::encode(bytes, &err);
}

FUZZ_TEST(Base32Suite, base32EncodeNotCrashes);
