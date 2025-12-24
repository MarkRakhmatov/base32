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

suite<"b32_encode"> b32_encode = [] {
  test("empty_input") = [] {
    base32::error err{};
    const auto ek = base32::encode({}, &err);

    expect(ek.empty());
  };

  test("byte_array_all_zeroes") = [] {
    base32::error err{};
    const char *expected_enc = "AAAAAAA=";

    const auto secret_bytes = base32::Bytes{0, 0, 0, 0};
    const auto enc = base32::encode(secret_bytes, &err);

    expect(enc == expected_enc);
  };

  test("array_allzeroes_utf8") = [] {
    base32::error err{};
    const char *expected_enc = "GAYDAMA=";

    const auto enc = base32::encode(stringToBytes("0000"), &err);

    expect(enc == expected_enc);
  };

  test("b32_all_chars") = [] {
    base32::error err{};
    const auto k = stringToBytes("ADFG413!£$%&&((/?^çé*[]#)-.,|<>+");
    const char *k_enc = "IFCEMRZUGEZSDQVDEQSSMJRIFAXT6XWDU7B2SKS3LURSSLJOFR6DYPRL";

    const auto ek = base32::encode(k, &err);

    expect(ek == k_enc);
  };

  test("b32_all_chars_plusone") = [] {
    base32::error err{};
    const auto k = stringToBytes("ADFG413!£$%&&((/?^çé*[]#)-.,|<>+");
    const char *k_enc = "IFCEMRZUGEZSDQVDEQSSMJRIFAXT6XWDU7B2SKS3LURSSLJOFR6DYPRL";

    const auto ek = base32::encode(k, &err);

    expect(ek == k_enc);
  };

  test("b32_rfc4648") = [] {
    base32::error err{};
    const char *k[] = {"", "f", "fo", "foo", "foob", "fooba", "foobar"};
    const char *k_enc[]
        = {"", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======"};

    for (int i = 0; i < 7; i++) {
      const auto ek = base32::encode(stringToBytes(k[i]), &err);
      expect(ek == k_enc[i]);
    }
  };

  test("b32_rfc4648_plusone") = [] {
    base32::error err{};
    const char *k[] = {"", "f", "fo", "foo", "foob", "fooba", "foobar"};
    const char *k_enc[]
        = {"", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======"};

    for (int i = 0; i < 7; i++) {
      const auto ek = base32::encode(stringToBytes(k[i]), &err);
      expect(ek == k_enc[i]);
    }
  };

  test("b32_rfc4648_noplusone") = [] {
    base32::error err{};
    const char *k[] = {"", "f", "fo", "foo", "foob", "fooba", "foobar"};
    const char *k_enc[]
        = {"", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======"};

    for (int i = 0; i < 7; i++) {
      const auto ek = base32::encode(stringToBytes(k[i]), &err);
      expect(ek == k_enc[i]);
    }
  };

  test("b32_encode_input_exceeded") = [] {
    base32::error err{};
    size_t len = 65 * 1024 * 1024;
    const auto k = stringToBytes(std::string(len, ' '));

    const auto ek = base32::encode(k, &err);
    expect(ek.empty());
    expect(err == base32::error::INVALID_USER_INPUT);
  };

  test("test_input_all_zeroes") = [] {
    base32::error err{};
    const base32::Bytes secret_bytes{0, 0, 0, 0};

    const auto encoded_str = base32::encode(secret_bytes, &err);

    expect(err == base32::error::NO_ERROR);
    expect(encoded_str == "AAAAAAA=");
  };
};
