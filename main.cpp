#include "all.h"

auto print = [](auto const &x) { std::cout << x << std::endl; };

int main() {
  int x = 0;

  int &ref = x;
  // OK
  ++ref;

  const int &const_ref = ref;

  print(const_ref);

  ++ref;
  print(const_ref);
}
