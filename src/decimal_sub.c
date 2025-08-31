#include "decimal.h"

int decimal_sub(decimal value_1, decimal value_2, decimal *result) {
  decimal_ex new_value_1 = {{0, 0, 0, 0, 0, 0, 0}},
                 new_value_2 = {{0, 0, 0, 0, 0, 0, 0}},
                 new_result = {{0, 0, 0, 0, 0, 0, 0}};

  int end = 0;
  prepare_values(value_1, value_2, &new_value_1, &new_value_2);
  decimal_subtraction(0, new_value_1, new_value_2, &new_result, 0);

  if (new_result.bits[3] || new_result.bits[4] || new_result.bits[5] ||
      ((new_result.bits[6] >> 16) & 0xFF) > 28)
    end = reduce_scale_bank_decimal_rounding_manual(&new_result);
  change_end(&end, new_result, result);

  return end;
}