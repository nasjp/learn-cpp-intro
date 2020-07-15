#include "all.h"

struct fractional {
  int num;
  int denom;

  fractional(int num, int denom = 1) : num(num), denom(denom) {
    std::cout << "constructed >> "s << std::endl
              << "num: "s << fractional::num << std::endl
              << "denom: " << fractional::denom << std::endl;
  }

  ~fractional() {
    std::cout << "destructed >> "s << std::endl
              << "num: "s << fractional::num << std::endl
              << "denom: " << fractional::denom << std::endl;
  }

  void set(int num) {
    fractional::num = num;
    fractional::denom = 1;
  }
  void set(int num, int denom) {
    fractional::num = num;
    fractional::denom = denom;
  }
};

fractional operator+(fractional &l, fractional &r) {
  // 分母が同じなら
  if (l.denom == r.denom)
    // 単に分子を足す
    return fractional{l.num + r.num, l.denom};

  // 分母を合わせて分子を足す
  return fractional{l.num * r.denom + r.num * l.denom, l.denom * r.denom};
}

fractional operator-(fractional &l, fractional &r) {
  // 分母が同じ
  if (l.denom == r.denom)
    return fractional{l.num - r.num, l.denom};

  return fractional{l.num * r.denom - r.num * l.denom, l.denom * r.denom};
}

fractional operator*(fractional &l, fractional &r) {
  return fractional{l.num * r.num, l.denom * r.denom};
}

fractional operator/(fractional &l, fractional &r) {
  return fractional{l.num * r.denom, l.denom * r.num};
}

int main() {
  fractional a = 1;
  std::cout << "------------line1" << std::endl;
  fractional b = a + a;
  std::cout << "------------line2" << std::endl;
  fractional c = a + b;
  std::cout << "------------line3" << std::endl;
}
