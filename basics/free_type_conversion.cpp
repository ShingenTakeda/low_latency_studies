#include <cstdint>
#include <cstdio>

// Conversion between uint64 into double with pointers
int main() {
  double x = 100;
  const double orig_x = x;
  uint64_t *x_as_uint = (uint64_t *)(&x);
  *x_as_uint |= 0x8000000000000000; // Mudar o sinal por operação de bit
  printf("orig_x : %0.2f\nx: %0.2f\n&x: %p\n&x_as_uint: %p\n", orig_x, x, &x,
         &x_as_uint); // Converção gratuita em questão de velocidade, ponteiro
                      // em x86_64 consume 64 bits de espaço
  return 0;
}
