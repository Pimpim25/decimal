#include "decimal.h"

int decimal_truncate(decimal value, decimal *result) {
  int scale = (value.bits[3] >> 16) & 0xFF, remainder = 0, end = 0,
      sign = (value.bits[3] >> 31) & 0x01;
  uint64_t test1 = 0;
  if (!result || scale < 0 || scale > 28) {
    return 1;
  }

  while (scale > 0) {
    remainder = 0;
    for (int i = 2; i >= 0; i--) {
      test1 = (uint64_t)remainder * (UINT32_MAX + 1ULL) + value.bits[i];
      value.bits[i] = (uint32_t)(test1 / 10ULL);
      remainder = (int)(test1 % 10ULL);
    }
    scale--;
  }
  for (int i = 0; i < 3; i++) {
    result->bits[i] = value.bits[i];
  }
  result->bits[3] = 0;  // сброс scale
  result->bits[3] |= ((sign & 0x1U) << 31);

  return end;
}