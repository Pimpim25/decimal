#include "decimal.h"

int is_equal(decimal value_1, decimal value_2) {
  decimal_ex new_value_1 = {{0, 0, 0, 0, 0, 0, 0}},
                 new_value_2 = {{0, 0, 0, 0, 0, 0, 0}};
  int sign_1 = (value_1.bits[3] >> 31) & 1,
      sign_2 = (value_2.bits[3] >> 31) & 1, status = 0;

  if (sign_1 != sign_2)
    status = 0;
  else {
    prepare_values(value_1, value_2, &new_value_1, &new_value_2);
    decimal_subtraction(0, new_value_1, new_value_2, &new_value_1, 0);
    status = !((new_value_1.bits[6] >> 31) & 1);

    if (status)
      status =
          (new_value_1.bits[0] || new_value_1.bits[1] || new_value_1.bits[2])
              ? 0
              : 1;
  }

  return status;
}
