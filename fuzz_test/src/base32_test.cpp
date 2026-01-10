#include "fuzztest/fuzztest.h"
#include "gtest/gtest.h"
#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"

#include <base32/base32.hpp>
#include <cstdlib>
#include <string_view>

void base32DecodeNotCrashes(std::string_view encoded) {
  base32::error err{};
  base32::decode(encoded, err);
}

FUZZ_TEST(Base32Suite, base32DecodeNotCrashes);

void base32EncodeNotCrashes(const base32::Bytes& bytes) {
  base32::error err{};
  base32::encode(bytes, err);
}

FUZZ_TEST(Base32Suite, base32EncodeNotCrashes);
