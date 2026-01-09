#pragma once
#include <string>

namespace csl {
  std::string getString();

  constexpr int factorialConstexpr(int input) noexcept {
    if (input < 2) {
      return 1;
    }

    return input * factorialConstexpr(input - 1);
  }

  int factorial(int input) noexcept;
}  // namespace csl
