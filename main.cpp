#include "all.h"

int input() {
  int x{};
  std::cin >> x;
  return x;
}

int factorial(int n) {
  if (n < 2) {
    return n;
  }
  return n * factorial(n - 1);
}

int main() {
  auto n = input();
  std::cout << factorial(n) << std::endl;
}
