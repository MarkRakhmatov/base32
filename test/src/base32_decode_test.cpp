#include <stdio.h>

#include <boost/ut.hpp>
#include <cstring>

#include "base32/base32.hpp"

using namespace boost::ut;

suite<"decode"> b32_decode = [] {
  test("all_chars") = [] {
    base32::error err;
    const char *k = "IFCEMRZUGEZSDQVDEQSSMJRIFAXT6XWDU7B2SKS3LURSSLJOFR6DYPRL";
    const char *k_dec = "ADFG413!£$%&&((/?^çé*[]#)-.,|<>+";

    uint8_t *dk = base32::decode(k, strlen(k) + 1, &err);

    expect(strcmp((char *)dk, k_dec) == 0_i);

    free(dk);
  };

  test("all_chars_noplusone") = [] {
    base32::error err;
    const char *k = "IFCEMRZUGEZSDQVDEQSSMJRIFAXT6XWDU7B2SKS3LURSSLJOFR6DYPRL";
    const char *k_dec = "ADFG413!£$%&&((/?^çé*[]#)-.,|<>+";

    uint8_t *dk = base32::decode(k, strlen(k), &err);

    expect(strcmp((char *)(dk), k_dec) == 0_i);

    free(dk);
  };

  test("rfc4648") = [] {
    base32::error err;
    const char *k[]
        = {"", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======"};
    const char *k_dec[] = {"", "f", "fo", "foo", "foob", "fooba", "foobar"};

    for (int i = 0; i < 7; i++) {
      uint8_t *dk = base32::decode(k[i], strlen(k[i]) + 1, &err);
      expect(strcmp((char *)dk, k_dec[i]) == 0_i);
      free(dk);
    }
  };

  test("rfc4648_noplusone") = [] {
    base32::error err;
    const char *k[]
        = {"", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======"};
    const char *k_dec[] = {"", "f", "fo", "foo", "foob", "fooba", "foobar"};

    for (int i = 0; i < 7; i++) {
      uint8_t *dk = base32::decode(k[i], strlen(k[i]), &err);
      expect(strcmp((char *)dk, k_dec[i]) == 0_i);
      free(dk);
    }
  };

  test("invalid_input") = [] {
    base32::error err;
    const char *k = "£&/(&/";
    size_t len = strlen(k);

    uint8_t *dk = base32::decode(k, len, &err);

    expect(dk == nullptr);
    expect(err == base32::error::INVALID_B32_INPUT);
  };

  test("input_exceeded") = [] {
    base32::error err;
    const char *k = "ASDF";
    size_t len = 128 * 1024 * 1024;

    uint8_t *dk = base32::decode(k, len, &err);

    expect(dk == nullptr);
    expect(err == base32::error::INVALID_USER_INPUT);
  };

  test("input_whitespaces") = [] {
    base32::error err;
    const char *k = "MZ XW 6Y TB";
    const char *expected = "fooba";

    uint8_t *dk = base32::decode(k, strlen(k), &err);

    expect(strcmp((const char *)dk, expected) == 0_i);

    free(dk);
  };

  test("encode_null") = [] {
    const char *token = "LLFTSZYMUGKHEDQBAAACAZAMUFKKVFLS";
    base32::error err;

    uint8_t *binary = base32::decode(token, strlen(token) + 1, &err);
    expect(err == base32::error::NO_ERROR);

    char *result = base32::encode(binary, 20, &err);
    expect(err == base32::error::NO_ERROR);
    free(binary);

    expect(strcmp(result, token) == 0);
    free(result);
  };

  test("empty_string") = [] {
    base32::error err;

    uint8_t *binary = base32::decode("", 0, &err);
    expect(err == base32::error::EMPTY_STRING);
    const char *expected = "";
    expect(strcmp((const char *)binary, expected) == 0);

    free(binary);
  };

  test("byte_array_all_zeroes") = [] {
    base32::error err;
    const char *token = "AAAAAAA=";

    uint8_t *binary = base32::decode(token, strlen(token) + 1, &err);
    expect(err == base32::error::NO_ERROR);
    for (int i = 0; i < 4; i++) {
      expect(binary[i] == 0_i);
    }

    free(binary);
  };
};
