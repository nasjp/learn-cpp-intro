#include "all.h"

int main() {
  [](bool b) -> int {
    if (b)
      return 0;
    else
      return 0.0;
  }(true);
}
