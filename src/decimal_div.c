#include "decimal.h"

int decimal_div(decimal value_1, decimal value_2, decimal *result) {
  decimal_ex new_value_1 = {{0, 0, 0, 0, 0, 0, 0}},
                 new_value_2 = {{0, 0, 0, 0, 0, 0, 0}},
                 new_result = {{0, 0, 0, 0, 0, 0, 0}};

  int end = 0;

  if (!value_2.bits[0] && !value_2.bits[1] && !value_2.bits[2]) end = 3;

  if (end != 3) {
    preuse_scale(0, value_1, &new_value_1);
    preuse_scale(0, value_2, &new_value_2);

    int decimal_add_scale = 0;

    decimal_division(&decimal_add_scale, new_value_1, new_value_2, &new_result);

    use_scale_and_sign(decimal_add_scale, 1, value_1, value_2, &new_result);
  }

  if (new_result.bits[3] || new_result.bits[4] || new_result.bits[5] ||
      ((new_result.bits[6] >> 16) & 0xFF) > 28)
    end = reduce_scale_bank_decimal_rounding_manual(&new_result);
  change_end(&end, new_result, result);

  return end;
}