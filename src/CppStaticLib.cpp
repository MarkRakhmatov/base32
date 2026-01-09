#include <string>
#include "CppStaticLib/CppStaticLib.hpp"


namespace csl {

  constexpr int g_maxIntFactorialInput = 12;

  std::string getString() { return "cpp static lib example"; }

  int factorial(int input) noexcept {
    if (input > g_maxIntFactorialInput) {
      return -1;
    }

    if (input < 2) {
      return 1;
    }

    return input * factorial(input - 1);
  }

}  // namespace csl
