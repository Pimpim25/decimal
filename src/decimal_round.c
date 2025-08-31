#include "decimal.h"

int decimal_round(decimal value, decimal *result) {
  if (!result) return 1;  // проверка аргументов
  int status = 0, remainder = 0;

  remainder = pre_decimal_round(&status, &value);

  if (!((value.bits[3] >> 16) & 0xFF) && remainder > 4) {
    decimal_round_to_bigger_value(&status, value, result);
  } else {
    for (int i = 0; i < 3; i++) result->bits[i] = value.bits[i];
  }
  result->bits[3] = value.bits[3] & ~(0xFF << 16);

  return status;
}