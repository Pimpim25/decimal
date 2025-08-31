#include "decimal.h"

int decimal_negate(decimal value, decimal *result) {
  int end = 0, scale = (value.bits[3] >> 16) & 0xFF;
  if (result && scale >= 0 && scale < 29) {
    for (int i = 0; i < 3; i++) {
      result->bits[i] = value.bits[i];
    }
    result->bits[3] = value.bits[3] ^ (1U << 31);
  } else {
    end = 1;
  }
  return end;
}