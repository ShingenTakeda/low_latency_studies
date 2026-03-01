#include <cstdint>
#include <cstdio>
#include <iostream>

// Bitwise optimizations

// Non-exclusive states
// You could call this a mask
enum STATE { MOVING, ATTACKING, JUMPING };

int main() {
  // If the underlying data can just be either a 0 or 1,
  // the expression x = y && z could be written as this:
  bool y = true;
  bool z = false;
  bool x =
      y & z; // x should be false(0), since && is basically an AND operation

  std::cout << "x state: " << x << "\n";

  uint8_t my_state =
      JUMPING | ATTACKING; // Small uint to store state, with 255 possibilities!

  // Check if any of these states apply
  if (my_state & (MOVING | ATTACKING | JUMPING)) {
    printf("Is in valid state!\nstate: %02B\n", my_state);
  }

  return 0;
}
