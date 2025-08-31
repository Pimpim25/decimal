#include "decimal.h"

void decimal_addition(int i, decimal_ex value_1, decimal_ex value_2,
              decimal_ex *result) {
  result->bits[6] = value_2.bits[6];
  uint64_t test1 = 0;
  int sign_1 = (value_1.bits[6] >> 31) & 1,
      sign_2 = (value_2.bits[6] >> 31) & 1;

  if (sign_1 == sign_2) {
    test1 = (uint64_t)value_1.bits[i] + (uint64_t)value_2.bits[i];
    if (i > 0) test1 += result->bits[i];
    if (test1 > UINT32_MAX) {
      result->bits[i] = (uint32_t)test1;
      if (i + 1 < 6) {
        result->bits[i + 1] += (uint32_t)(test1 >> 32);
        decimal_addition(i + 1, value_1, value_2, result);
      }
    } else {
      result->bits[i++] = test1;
      if (i < 6) decimal_addition(i, value_1, value_2, result);
    }
  } else if (sign_1 && !sign_2) {
    value_1.bits[6] &= ~(1u << 31);
    decimal_subtraction(0, value_2, value_1, result, 0);
  } else if (!sign_1 && sign_2) {
    value_2.bits[6] &= ~(1u << 31);
    decimal_subtraction(0, value_1, value_2, result, 0);
  }
}

void prepare_values(decimal value_1, decimal value_2,
                    decimal_ex *new_value_1, decimal_ex *new_value_2) {
  int scale = 0, scale_1 = (value_1.bits[3] >> 16) & 0xFF,
      scale_2 = (value_2.bits[3] >> 16) & 0xFF;

  scale = scale_1 > scale_2 ? scale_1 : scale_2;

  preuse_scale(1, value_1, new_value_1);
  preuse_scale(1, value_2, new_value_2);

  if (scale_1 < scale)
    use_scale(value_1, &new_value_1, scale, scale_1);
  else if (scale_2 < scale)
    use_scale(value_2, &new_value_2, scale, scale_2);
}

void use_scale(decimal value, decimal_ex **new_value, int scale,
               int small_scale) {
  int raz = scale - small_scale;
  uint64_t test1 = 0;
  uint32_t remainder = 0;

  for (int i = 0; i < raz; i++) {
    remainder = 0;
    test1 = 0;
    for (int j = 0; j < 6; j++) {
      test1 = (uint64_t)(*new_value)->bits[j] * 10ULL + remainder;
      (*new_value)->bits[j] = 0;
      (*new_value)->bits[j] = (uint32_t)test1;
      remainder = (uint32_t)(test1 >> 32);
    }
  }

  (*new_value)->bits[6] = 0;
  (*new_value)->bits[6] |= ((scale & 0xFF) << 16);
  (*new_value)->bits[6] |= (value.bits[3] & (1u << 31));
}

void preuse_scale(int with_scale, decimal value,
                  decimal_ex *new_value) {
  for (int i = 0; i < 3; i++) {
    new_value->bits[i] = value.bits[i];
  }
  if (with_scale) new_value->bits[6] = value.bits[3];
}

void decimal_subtraction(int i, decimal_ex value_1, decimal_ex value_2,
                 decimal_ex *result, int borrow) {
  uint64_t v1 = 0, v2 = 0;
  int sign_1 = (value_1.bits[6] >> 31) & 1,
      sign_2 = (value_2.bits[6] >> 31) & 1, diff = 0;

  if (sign_1 == sign_2) {
    diff =
        choose_the_sign(value_1, value_2);  // 0 больше первое, 1 больше второе

    if (!diff)
      v1 = (uint64_t)value_1.bits[i], v2 = (uint64_t)value_2.bits[i] + borrow;
    else
      v2 = (uint64_t)value_1.bits[i], v1 = (uint64_t)value_2.bits[i] + borrow;

    if (v1 < v2) {
      result->bits[i] = (uint32_t)(v1 + UINT32_MAX + 1ULL - v2);
      borrow = 1;
    } else {
      result->bits[i] = (uint32_t)(v1 - v2);
      borrow = 0;
    }

    if (i < 6) decimal_subtraction(i + 1, value_1, value_2, result, borrow);

    if (!diff)
      result->bits[6] |= ((sign_1 & 0x1U) << 31);
    else
      result->bits[6] |= ((!sign_1 & 0x1U) << 31);

    result->bits[6] &= ~(0xFF << 16);
    result->bits[6] |= value_2.bits[6] & (0xFF << 16);

  } else {
    value_1.bits[6] &= ~(1u << 31);
    value_2.bits[6] &= ~(1u << 31);
    decimal_addition(i, value_1, value_2, result);
    result->bits[6] |= ((sign_1 & 0x1U) << 31);
  }
}

int choose_the_sign(decimal_ex value_1, decimal_ex value_2) {
  int status = 0, check = 0;
  for (int i = 5; i >= 0; i--) {
    if (value_1.bits[i] > value_2.bits[i] && !check) {
      status = 0;
      check = 1;
    } else if (value_1.bits[i] < value_2.bits[i] && !check) {
      status = 1;
      check = 1;
    }
  }
  return status;
}

int reduce_scale_bank_decimal_rounding_manual(decimal_ex *value) {
  int remainder = 0, end = 0, test2 = 1, scale = (value->bits[6] >> 16) & 0xFF;
  uint64_t test1 = 0, temp = 0;

  if (!scale || (value->bits[3] || value->bits[4] || value->bits[5])) end = 1;

  while ((scale > 0 && end) || scale > 28) {
    remainder = 0;

    for (int i = 5; i >= 0; i--) {
      test1 = (uint64_t)remainder * (UINT32_MAX + 1ULL) + value->bits[i];
      value->bits[i] = (uint32_t)(test1 / 10ULL);
      remainder = (int)(test1 % 10ULL);
    }

    if (remainder > 5 || (remainder == 5 && (value->bits[0] % 2 == 1))) {
      test2 = 1;
      for (int i = 0; i < 6; i++) {
        temp = (uint64_t)value->bits[i] + test2;
        value->bits[i] = (uint32_t)temp;
        test2 = temp >> 32;
      }
    }

    scale--;

    if (value->bits[3] || value->bits[4] || value->bits[5] || scale > 28) {
      end = (((value->bits[6] >> 31) & 1) == 1) ? 2 : 1;
    } else
      end = 0;
  }

  value->bits[6] &= ~(0xFF << 16);
  value->bits[6] |= ((scale & 0xFF) << 16);

  return end;
}

void change_end(int *end, decimal_ex new_result, decimal *result) {
  if (*end) {
    if ((new_result.bits[6] >> 31) & 1) *end = 2;
    for (int i = 0; i < 3; i++) result->bits[i] = 0;
  } else {
    for (int i = 0; i < 3; i++) result->bits[i] = new_result.bits[i];
    result->bits[3] = new_result.bits[6];
  }
}

// общие для decimal_floor и decimal_round функции

int pre_decimal_round(int *status, decimal *value) {
  int remainder = 0, scale = (value->bits[3] >> 16) & 0xFF,
      sign = (value->bits[3] >> 31) & 0x01;
  uint64_t test1 = 0;

  if (scale < 0) *status = 1;  // проверка аргументов

  while (scale > 0 && ((value->bits[0] != 0) || (value->bits[1] != 0) ||
                       (value->bits[2] != 0))) {
    remainder = 0;
    test1 = 0;
    for (int i = 2; i >= 0; i--) {
      test1 =
          (uint64_t)value->bits[i] + (uint64_t)remainder * (UINT32_MAX + 1ULL);
      remainder = (int)test1 % 10;
      value->bits[i] = (uint32_t)(test1 / 10ULL);
    }
    scale--;
  }

  value->bits[3] = 0;
  value->bits[3] |= ((scale & 0xFF) << 16);
  value->bits[3] |= ((sign & 0x01) << 31);

  return remainder;
}

void decimal_round_to_bigger_value(int *status, decimal value,
                           decimal *result) {
  uint64_t temp = 0;
  for (int i = 0; i < 3; i++) {
    temp = (uint64_t)value.bits[i];
    if (i == 0) temp++;
    result->bits[i] = (uint32_t)temp;
    if (temp > UINT32_MAX) {
      result->bits[i] = 0;
      if (i + 1 < 3)
        result->bits[i + 1]++;
      else
        *status = 1;  // переполнение
    }
  }
}


// не моя часть

void decimal_multiplication(decimal_ex value_1, decimal_ex value_2,
                    decimal_ex *result) {
  decimal_ex inter_result = {{0}};
  decimal_ex test = {{0}};

  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 32; j++) {
      if ((value_1.bits[i] >> j) & 1) {
        test = value_2;
        bits_shift_left(i, j, &test);

        inter_result = *result;
        clear_decimal_ex(result);

        decimal_addition(0, inter_result, test, result);
      }
    }
  }
}

void decimal_division(int *scale, decimal_ex value_1, decimal_ex value_2,
              decimal_ex *result) {
  decimal_ex remainder = value_1;
  decimal_ex sign = {{0}};
  decimal_ex inter_result = {{0}};  // промежуточные вычисления

  decimal_subtraction(0, remainder, value_2, &sign, 0);

  while (!(sign.bits[6] >> 31 & 1)) {
    int i = 0, j = 0, x = -1, y = -1;
    decimal_ex found_integer = {{0}};  // максимальное, меньшее value_1
    decimal_ex test = value_2;

    while (!(sign.bits[6] >> 31 & 1)) {
      x = i, y = j;
      found_integer = test;

      i = j == 31 ? i + 1 : i;
      j = j == 31 ? 0 : j + 1;

      test = value_2;
      bits_shift_left(i, j, &test);
      clear_decimal_ex(&sign);
      decimal_subtraction(0, remainder, test, &sign, 0);
    }

    inter_result = *result;
    clear_decimal_ex(result);

    decimal_ex offset = {{1, 0, 0, 0, 0, 0, 0}};
    bits_shift_left(x, y, &offset);
    decimal_addition(0, inter_result, offset, result);

    inter_result = remainder;
    clear_decimal_ex(&remainder);
    clear_decimal_ex(&sign);
    decimal_subtraction(0, inter_result, found_integer, &remainder, 0);
    decimal_subtraction(0, remainder, value_2, &sign, 0);
  }

  // работа с остатком
  if ((remainder.bits[0] || remainder.bits[1] || remainder.bits[2] ||
       remainder.bits[3] || remainder.bits[4] || remainder.bits[5]) &&
      !result->bits[5]) {
    work_with_remainder(scale, remainder, value_2, result);
  }
}

void work_with_remainder(int *scale, decimal_ex remainder,
                         decimal_ex value_2, decimal_ex *result) {
  decimal_ex copy_res = *result;
  decimal_ex copy_int = remainder;
  decimal_ex ten = {{10, 0, 0, 0, 0, 0, 0}};

  clear_decimal_ex(result);
  clear_decimal_ex(&remainder);

  decimal_multiplication(copy_res, ten, result);
  decimal_multiplication(copy_int, ten, &remainder);

  (*scale)++;

  decimal_division(scale, remainder, value_2, result);
}

void bits_shift_left(int i, int j, decimal_ex *num) {
  decimal_ex copy_num = *num;
  decimal_ex next_bits = {{0}};

  for (int x = 0; x < 6; x++) {
    num->bits[x] = num->bits[x] << j;
    num->bits[x] |= next_bits.bits[x];

    if (x + 1 < 6) {
      next_bits.bits[x + 1] = copy_num.bits[x] >> (31 - j);
      next_bits.bits[x + 1] = next_bits.bits[x + 1] >> 1;
    }
  }

  for (int k = 0; k < i; k++) {
    for (int x = 5; x > 0; x--) {
      num->bits[x] = num->bits[x - 1];
    }
    num->bits[0] = 0;
  }
}

void use_scale_and_sign(int decimal_add_scale, int scale_sign, decimal value_1,
                        decimal value_2, decimal_ex *result) {
  int sign_1 = value_1.bits[3] >> 31 & 1, sign_2 = value_2.bits[3] >> 31 & 1;
  int sign_result = sign_1 ^ sign_2;

  int scale_1 = (value_1.bits[3] >> 16) & 0xFF,
      scale_2 = (value_2.bits[3] >> 16) & 0xFF;
  int scale_result = scale_sign ? scale_1 - scale_2 : scale_1 + scale_2;

  scale_result += decimal_add_scale;

  while (scale_result < 0 && scale_sign) {
    decimal_ex ten = {{10, 0, 0, 0, 0, 0, 0}};
    decimal_ex copy_res = *result;

    clear_decimal_ex(result);
    decimal_multiplication(copy_res, ten, result);

    scale_result++;
  }

  result->bits[6] |= sign_result << 31;
  result->bits[6] |= scale_result << 16;
}

void clear_decimal_ex(decimal_ex *num) {
  for (int i = 0; i < 6; i++) {
    num->bits[i] = 0;
  }
}