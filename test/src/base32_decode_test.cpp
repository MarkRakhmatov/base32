#include <cstdio>

#include <boost/ut.hpp>
#include <cstring>

#include "base32/base32.hpp"

using namespace boost::ut;

constexpr base32::Bytes stringToBytes(std::string_view str) {
  base32::Bytes result;
  for (const auto ch: str) {
    result.push_back(ch);
  }

  return result;
}

suite<"decode"> b32_decode = [] {
  test("all_chars") = [] {
    base32::error err{};
    const char *k = "IFCEMRZUGEZSDQVDEQSSMJRIFAXT6XWDU7B2SKS3LURSSLJOFR6DYPRL";
    const base32::Bytes k_dec = stringToBytes("ADFG413!£$%&&((/?^çé*[]#)-.,|<>+");

    const auto dk = base32::decode(k, &err);

    expect(dk == k_dec);
  };

  test("all_chars_noplusone") = [] {
    base32::error err{};
    const char *k = "IFCEMRZUGEZSDQVDEQSSMJRIFAXT6XWDU7B2SKS3LURSSLJOFR6DYPRL";
    const auto k_dec = stringToBytes("ADFG413!£$%&&((/?^çé*[]#)-.,|<>+");

    const auto dk = base32::decode(k, &err);

    expect(dk == k_dec);
  };

  test("rfc4648") = [] {
    base32::error err{};
    const char *k[]
        = {"", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======"};
    const char *k_dec[] = {"", "f", "fo", "foo", "foob", "fooba", "foobar"};

    for (int i = 0; i < 7; i++) {
      const auto dk = base32::decode(k[i], &err);
      expect(dk == stringToBytes(k_dec[i]));
    }
  };

  test("rfc4648_noplusone") = [] {
    base32::error err{};
    const char *k[]
        = {"", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======"};
    const char *k_dec[] = {"", "f", "fo", "foo", "foob", "fooba", "foobar"};

    for (int i = 0; i < 7; i++) {
      const auto dk = base32::decode(k[i], &err);
      expect(dk == stringToBytes(k_dec[i]));
    }
  };

  test("invalid_input") = [] {
    base32::error err{};
    const char *k = "£&/(&/";

    const auto dk = base32::decode(k, &err);

    expect(dk.empty());
    expect(err == base32::error::INVALID_B32_INPUT);
  };

  test("input_exceeded") = [] {
    base32::error err{};
    size_t len = 128 * 1024 * 1024;
    const std::string k(len, ' ');

    const auto dk = base32::decode(k, &err);

    expect(dk.empty());
    expect(err == base32::error::INVALID_USER_INPUT);
  };

  test("input_whitespaces") = [] {
    base32::error err{};
    const char *k = "MZ XW 6Y TB";
    const auto expected = stringToBytes("fooba");

    const auto dk = base32::decode(k, &err);

    expect(dk == expected);
  };

  test("encode_null") = [] {
    const char *token = "LLFTSZYMUGKHEDQBAAACAZAMUFKKVFLS";
    base32::error err{};

    const auto binary = base32::decode(token, &err);
    expect(err == base32::error::NO_ERROR);

    const auto result = base32::encode(binary, &err);
    expect(err == base32::error::NO_ERROR);

    expect(result == token);
  };

  test("empty_string") = [] {
    base32::error err{};

    const auto binary = base32::decode("", &err);
    expect(err == base32::error::NO_ERROR);
    expect(binary.empty());
  };

  test("whitespace_string") = [] {
    base32::error err{};

    const auto binary = base32::decode(" ", &err);
    expect(err == base32::error::NO_ERROR);
    expect(binary.empty());
  };

  test("byte_array_all_zeroes") = [] {
    base32::error err{};
    const char *token = "AAAAAAA=";

    const auto binary = base32::decode(token, &err);
    expect(err == base32::error::NO_ERROR);
    for (int i = 0; i < 4; i++) {
      expect(binary[i] == 0_i);
    }
  };
};
