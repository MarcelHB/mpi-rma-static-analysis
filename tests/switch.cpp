#include <cstdint>

int main () {
  uint64_t a = 0;

  switch (a) {
    case 0:
      a = 1;
      break;
    case 1:
      a += 1;
      break;
    default:
      a *= 2;
      break;
  }

  return 0;
}
