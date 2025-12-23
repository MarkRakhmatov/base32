#include <boost/ut.hpp>
#include <cstring>

#include "base32/base32.hpp"

using namespace boost::ut;

suite<"b32_encode"> b32_encode = [] {
  test("null_input") = [] {
    base32::error err;
    const uint8_t *k = nullptr;

    char *ek = base32::encode(k, 5, &err);

    expect(ek == nullptr);
  };

  test("invalid_or_empty") = [] {
    base32::error err;

    base32::encode(NULL, 30, &err);
    expect(err == base32::error::INVALID_USER_INPUT);

    char *k_enc = encode((const unsigned char *)"asdiasjdijis", 0, &err);
    expect(strcmp(k_enc, "") == 0_i);
    expect(err == base32::error::EMPTY_STRING);

    free(k_enc);
  };

  test("byte_array_all_zeroes") = [] {
    base32::error err;
    const char *expected_enc = "AAAAAAA=";

    uint8_t secret_bytes[] = {0, 0, 0, 0};
    char *enc = base32::encode(secret_bytes, 4, &err);

    expect(strcmp(enc, expected_enc) == 0);
    free(enc);
  };

  test("array_allzeroes_utf8") = [] {
    base32::error err;
    const char *expected_enc = "GAYDAMA=";

    char *enc = base32::encode((const unsigned char *)"0000", 4, &err);

    expect(strcmp(enc, expected_enc) == 0);
    free(enc);
  };

  test("b32_all_chars") = [] {
    base32::error err;
    const char *k = "ADFG413!£$%&&((/?^çé*[]#)-.,|<>+";
    const char *k_enc = "IFCEMRZUGEZSDQVDEQSSMJRIFAXT6XWDU7B2SKS3LURSSLJOFR6DYPRL";

    char *ek = base32::encode((const unsigned char *)k, strlen(k), &err);

    expect(strcmp(ek, k_enc) == 0);

    free(ek);
  };

  test("b32_all_chars_plusone") = [] {
    base32::error err;
    const char *k = "ADFG413!£$%&&((/?^çé*[]#)-.,|<>+";
    const char *k_enc = "IFCEMRZUGEZSDQVDEQSSMJRIFAXT6XWDU7B2SKS3LURSSLJOFR6DYPRL";

    char *ek = base32::encode((const unsigned char *)k, strlen(k) + 1, &err);

    expect(strcmp(ek, k_enc) == 0);

    free(ek);
  };

  test("b32_rfc4648") = [] {
    base32::error err;
    const char *k[] = {"", "f", "fo", "foo", "foob", "fooba", "foobar"};
    const char *k_enc[]
        = {"", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======"};

    for (int i = 0; i < 7; i++) {
      char *ek = base32::encode((const unsigned char *)k[i], strlen(k[i]), &err);
      expect(strcmp(ek, k_enc[i]) == 0_i);
      free(ek);
    }
  };

  test("b32_rfc4648_plusone") = [] {
    base32::error err;
    const char *k[] = {"", "f", "fo", "foo", "foob", "fooba", "foobar"};
    const char *k_enc[]
        = {"", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======"};

    for (int i = 0; i < 7; i++) {
      char *ek = base32::encode((const unsigned char *)k[i], strlen(k[i]) + 1, &err);
      expect(strcmp(ek, k_enc[i]) == 0);
      free(ek);
    }
  };

  test("b32_rfc4648_noplusone") = [] {
    base32::error err;
    const char *k[] = {"", "f", "fo", "foo", "foob", "fooba", "foobar"};
    const char *k_enc[]
        = {"", "MY======", "MZXQ====", "MZXW6===", "MZXW6YQ=", "MZXW6YTB", "MZXW6YTBOI======"};

    for (int i = 0; i < 7; i++) {
      char *ek = base32::encode((const unsigned char *)k[i], strlen(k[i]), &err);
      expect(strcmp(ek, k_enc[i]) == 0);
      free(ek);
    }
  };

  test("b32_encode_input_exceeded") = [] {
    base32::error err;
    const char *k = "test";
    size_t len = 65 * 1024 * 1024;

    char *ek = base32::encode((const unsigned char *)k, len, &err);
    expect(ek == nullptr);
    expect(err == base32::error::INVALID_USER_INPUT);
  };

  test("test_input_all_zeroes") = [] {
    base32::error err;
    const uint8_t secret_bytes[] = {0, 0, 0, 0};

    char *encoded_str = base32::encode(secret_bytes, 4, &err);

    expect(err == base32::error::NO_ERROR);
    expect(strcmp(encoded_str, "AAAAAAA=") == 0);

    free(encoded_str);
  };
};
