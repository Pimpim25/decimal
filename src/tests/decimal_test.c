#include "../decimal.h"

#include <check.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// для меня когда я не с федоры
// gcc -std=c11 -Wall -Werror -Wextra -pedantic *.c ./tests/decimal_test.c $(pkg-config --cflags --libs check) -o ./tests/TEST

START_TEST(test_decimal_add_1) {
  // 5 + 7 = 12
  decimal a = {{5, 0, 0, 0}};
  decimal b = {{7, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 12);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_2) {
  // 1.2 + 0.03 = 1.23
  decimal a = {{12, 0, 0, 1 << 16}};  // 1.2 (scale = 1)
  decimal b = {{3, 0, 0, 2 << 16}};   // 0.03 (scale = 2)
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 123);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 2);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_3) {
  // 5 + (-3) = 2
  decimal a = {{5, 0, 0, 0}};
  decimal b = {{3, 0, 0, 1u << 31}};  // знак минус
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 2);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_4) {
  // 1.25 + 3.75 = 5.00
  decimal a = {{125, 0, 0, 2 << 16}};  // scale = 2
  decimal b = {{375, 0, 0, 2 << 16}};  // scale = 2
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 500);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 2);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_5) {
  // 0 + 123456 = 123456
  decimal a = {{0, 0, 0, 0}};
  decimal b = {{123456, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 123456);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_6) {
  // (-2) + (-4) = -6
  decimal a = {{2, 0, 0, 1u << 31}};
  decimal b = {{4, 0, 0, 1u << 31}};

  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 6);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 1);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_7) {
  // MAX + 1 = ошибка (overflow)
  decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  decimal b = {{1, 0, 0, 0}};

  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 0);
  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_decimal_add_8) {
  // 0.0000000000000000000000000001 + 0.0000000000000000000000000001 =
  // 0.0000000000000000000000000002
  decimal a = {{1, 0, 0, 28 << 16}};
  decimal b = {{1, 0, 0, 28 << 16}};
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 2);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 28);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_9) {
  // 5.00 + (-0.05) = 4.95
  decimal a = {{500, 0, 0, 2 << 16}};
  decimal b = {{5, 0, 0, (2 << 16) | (1u << 31)}};
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 495);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 2);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_10) {
  // Проверка точности округления
  decimal a = {{1, 0, 0, 1 << 16}};  // 0.1
  decimal b = {{2, 0, 0, 1 << 16}};  // 0.2
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 3);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 1);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_11) {
  // (-100.50) + 200.25 = 99.75
  decimal a = {{10050, 0, 0, (2 << 16) | (1u << 31)}};  // -100.50
  decimal b = {{20025, 0, 0, 2 << 16}};                 // 200.25
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 9975);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 2);
  ck_assert_int_eq((result.bits[3] >> 31) & 1, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

// START_TEST(test_decimal_add_12) {
//   // 1.0000000000000000000000000001 + 2 = 3.0000000000000000000000000001
//   decimal a = {{1, 0, 0, 28 << 16}};
//   decimal b = {{2, 0, 0, 0}};
//   decimal result = {{0}};
//   int status = decimal_add(a, b, &result);

//   ck_assert_int_eq(result.bits[0], 30000000000000000000000000001);
//   ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 28);
//   ck_assert_int_eq((result.bits[3] >> 31) & 1, 0);
//   ck_assert_int_eq(status, 0);
// }
// END_TEST

START_TEST(test_decimal_add_13) {
  // 79228162514264337593543950335 + 1 = overflow
  decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  decimal b = {{1, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_decimal_add_14) {
  // -0.0000000000000000000000000001 + 0.0000000000000000000000000001 = 0
  decimal a = {{1, 0, 0, (28 << 16) | (1u << 31)}};
  decimal b = {{1, 0, 0, 28 << 16}};
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 28);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_15) {
  // -123456789 + 123456789 = 0
  decimal a = {{123456789, 0, 0, 1u << 31}};
  decimal b = {{123456789, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_16) {  // проверка округления
  // 1.999999999999999999999999999 + 0.000000000000000000000000001 = 2.0
  decimal a = {{
      0xF0BDC21F,  // bits[0]
      0xB1A2BC2E,  // bits[1]
      0x036B3D0E,  // bits[2]
      27 << 16     // scale = 27, sign = 0
  }};
  decimal b = {{
      1,  // bits[0]
      0, 0,
      27 << 16  // scale = 27, sign = 0
  }};
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  // Ожидаем результат = 2.0 → все мантиссы нули, scale = 1 или 0
  ck_assert_int_eq(status, 0);
  ck_assert_uint_eq(result.bits[0], 0xF0BDC220);
  ck_assert_uint_eq(result.bits[1], 0xB1A2BC2E);
  ck_assert_uint_eq(result.bits[2], 0x036B3D0E);
  // Допускаем scale = 27, 1 или 0 в зависимости от округления
  ck_assert_int_le((result.bits[3] >> 16) & 0xFF, 27);
}
END_TEST

START_TEST(test_decimal_add_17) {
  decimal a = {.bits = {0xFFFFFFFF, 0x00000000, 0x00000000, 0}};

  decimal b = {.bits = {0x00000001, 0x00000000, 0x00000000, 0}};
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 1);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_18) {
  decimal a = {.bits = {0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0}};

  decimal b = {.bits = {0x55555555, 0x55555555, 0x55555555, 0}};
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[2], 0xFFFFFFFF);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_19) {
  decimal a = {{
      0xF0BDC21F,  // bits[0]
      0xB1A2BC2E,  // bits[1]
      0x036B3D0E,  // bits[2]
      28 << 16     // scale = 28, sign = 0
  }};

  decimal b = {{
      1u,  // bits[0] = 1 (это 1e-28)
      0, 0,
      28 << 16  // scale = 28, sign = 0
  }};

  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(result.bits[0], 0xF0BDC220);
  ck_assert_int_eq(result.bits[1], 0xB1A2BC2E);
  ck_assert_int_eq(result.bits[2], 0x036B3D0E);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 28);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_add_20) {
  // 8790 + (-123456) = -114666
  decimal a = {{8790, 0, 0, 0}};
  decimal b = {{123456, 0, 0, (1u << 31)}};  // -123456, знак = 1, scale = 0
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(result.bits[0], 114666);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 0);  // scale = 0
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 1);
}
END_TEST

START_TEST(test_decimal_add_21) {
  decimal a = {{3, 0, 0, 30 << 16}};  // 3e-30
  decimal b = {{0, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_add(a, b, &result);

  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 28);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq(result.bits[0], 0);
}
END_TEST

START_TEST(test_decimal_sub_1) {
  // 10 - 5 = 5
  decimal a = {{10, 0, 0, 0}};
  decimal b = {{5, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(result.bits[0], 5);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_sub_2) {
  // 5 - 10 = -5
  decimal a = {{5, 0, 0, 0}};
  decimal b = {{10, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(result.bits[0], 5);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 1);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_sub_3) {
  // 1.50 - 0.25 = 1.25
  decimal a = {{150, 0, 0, 2 << 16}};
  decimal b = {{25, 0, 0, 2 << 16}};
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(result.bits[0], 125);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 2);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_sub_4) {
  // 100 - 0.5 = 99.5
  decimal a = {{100, 0, 0, 0}};
  decimal b = {{5, 0, 0, 1 << 16}};
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(result.bits[0], 995);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 1);
  ck_assert_int_eq(status, 0);
}
END_TEST
START_TEST(test_decimal_sub_5) {
  // 3 - (-2) = 5
  decimal a = {{3, 0, 0, 0}};
  decimal b = {{2, 0, 0, 1u << 31}};
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(result.bits[0], 5);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_sub_6) {
  // (-5) - 2 = -7
  decimal a = {{5, 0, 0, 1u << 31}};
  decimal b = {{2, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(result.bits[0], 7);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 1);
  ck_assert_int_eq(status, 0);
}
END_TEST
START_TEST(test_decimal_sub_7) {
  // 0.0000000000000000000000000002 - 0.0000000000000000000000000001 =
  // 0.0000000000000000000000000001
  decimal a = {{2, 0, 0, 28 << 16}};
  decimal b = {{1, 0, 0, 28 << 16}};
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(result.bits[0], 1);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 28);
  ck_assert_int_eq(status, 0);
}
END_TEST
START_TEST(test_decimal_sub_8) {
  // MIN - 1 = ошибка (переполнение)
  decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1u << 31}};
  decimal b = {{1, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(status, 2);  // допустим, код 2 = underflow/overflow
}
END_TEST

START_TEST(test_decimal_sub_9) {
  // 0 - 0.01 = -0.01
  decimal a = {{0, 0, 0, 0}};
  decimal b = {{1, 0, 0, 2 << 16}};
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[0], 1);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 1);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 2);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_sub_10) {
  // 0 - 0.01 = -0.01
  decimal a = {{5, 0, 0, (15 << 16) | (1u << 31)}};
  decimal b = {{5, 0, 0, (14 << 16) | (1u << 31)}};
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(result.bits[0], 45);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 15);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_sub_11) {
  decimal a = {{3, 0, 0, 30 << 16}};  // 3e-30
  decimal b = {{0, 0, 0, 0}};         // 0
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 28);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq(result.bits[0], 0);
}
END_TEST

START_TEST(test_decimal_sub_12) {
  decimal a = {{0, 1, 0, 0}};
  decimal b = {{1, 0, 0, 0}};  // 0
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(result.bits[0], 4294967295);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(result.bits[0], 4294967295);
}
END_TEST

START_TEST(test_decimal_sub_13) {
  // 10 - 5 = 5
  decimal a = {{UINT32_MAX, 0, 0, 0}};
  decimal b = {{2147483648U, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_sub(a, b, &result);

  ck_assert_int_eq(result.bits[0], 2147483647);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_1) {
  // 5 * 10 = 50
  decimal a = {{5, 0, 0, 0}};
  decimal b = {{10, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(result.bits[0], 50);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_2) {
  // 123456789 * 987654321 = 121932631112635269
  decimal a = {{123456789, 0, 0, 0}};
  decimal b = {{987654321, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(result.bits[0], 4227814277);
  ck_assert_int_eq(result.bits[1], 28389652);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_3) {
  // 0.12345 * 0.6789 = 0.083810205
  decimal a = {{12345, 0, 0, 5 << 16}};
  decimal b = {{6789, 0, 0, 4 << 16}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(result.bits[0], 83810205);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 9);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_4) {
  // 100.01 * -0.001 = -0.10001
  decimal a = {{10001, 0, 0, 2 << 16}};
  decimal b = {{1, 0, 0, (3 << 16) | (1u << 31)}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(result.bits[0], 10001);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 5);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 1);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_5) {
  decimal a = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 1u << 31}};
  decimal b = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(status, 2);  // переполнение/слишком большое число
}
END_TEST

START_TEST(test_decimal_mul_6) {
  decimal a = {{1, 0, 0, 28 << 16}};
  decimal b = {{2, 0, 0, 28 << 16}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 28);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_7) {
  // -99999.999 * -0.0001 = 9.9999999
  decimal a = {{99999999, 0, 0, (3 << 16) | (1u << 31)}};
  decimal b = {{1, 0, 0, (4 << 16) | (1u << 31)}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(result.bits[0], 99999999);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 7);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_8) {
  decimal a = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0}};
  decimal b = {{1, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(result.bits[0], UINT32_MAX);
  ck_assert_int_eq(result.bits[1], UINT32_MAX);
  ck_assert_int_eq(result.bits[2], UINT32_MAX);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_9) {
  decimal a = {{1000, 0, 0, 0}};
  decimal b = {{0, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_10) {
  // 314159265358.9793 * 271828.1828459 = 85397342226734.24306179318987
  decimal a = {{170224929, 731459, 0, 4 << 16}};
  decimal b = {{3862497387, 632, 0, 7 << 16}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(result.bits[0], 1533453515);
  ck_assert_int_eq(result.bits[1], 3580142383);
  ck_assert_int_eq(result.bits[2], 462939919);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 11);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_11) {
  // 1.35829124765736721 * 0.000000000000001 = 0.0000000000000013582912476573
  // 6721
  decimal a = {{2345688849, 31625182, 0, 17 << 16}};
  decimal b = {{1, 0, 0, 15 << 16}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);  // 0.0000000000000013582912476573 6721

  ck_assert_int_eq(result.bits[0], 2225886622);
  ck_assert_int_eq(result.bits[1], 3162);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 28);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_12) {
  // 1000 * 2000.456 = 2000456
  decimal a = {{1000, 0, 0, 0}};
  decimal b = {{2000456, 0, 0, 3 << 16}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(result.bits[0], 2000456000);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 3);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_mul_13) {
  // 2.5 * 3.125 = 7.8125
  decimal a = {{25, 0, 0, 1 << 16}};
  decimal b = {{3125, 0, 0, 3 << 16}};
  decimal result = {{0}};
  int status = decimal_mul(a, b, &result);

  ck_assert_int_eq(result.bits[0], 78125);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 4);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_1) {
  // 50 / 5 = 10
  decimal a = {{50, 0, 0, 0}};
  decimal b = {{5, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 10);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_2) {
  // 0.75 / 0.25 = 3
  decimal a = {{75, 0, 0, 2 << 16}};
  decimal b = {{25, 0, 0, 2 << 16}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 3);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_3) {
  decimal a = {{1000, 0, 0, 0}};
  decimal b = {{0, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(status, 3);  // деление на 0
}
END_TEST

START_TEST(test_decimal_div_4) {
  decimal a = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0}};
  decimal b = {{UINT32_MAX, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 1);
  ck_assert_int_eq(result.bits[1], 1);
  ck_assert_int_eq(result.bits[2], 1);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_5) {
  // -0.00005 / 0.2 = -0.00025
  decimal a = {{5, 0, 0, (1u << 31) | (5 << 16)}};
  decimal b = {{2, 0, 0, 1 << 16}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 25);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 5);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 1);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_6) {
  decimal a = {{15, 0, 0, 15 << 16}};
  decimal b = {{1, 0, 0, 17 << 16}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 1500);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_7) {
  decimal a = {{50, 0, 0, (1u << 31) | (3 << 16)}};
  decimal b = {{10, 0, 0, (1u << 31) | (3 << 16)}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 5);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_8) {
  decimal a = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0}};
  decimal b = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 1u << 31}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 1);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 31) & 0x1, 1);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_9) {
  decimal a = {{0, 1, 0, 0}};
  decimal b = {{10, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 1);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 1);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_10) {
  decimal a = {{0, UINT32_MAX, 0, 0}};
  decimal b = {{UINT32_MAX, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 1);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_11) {
  // 75 / 2 = 37.5
  decimal a = {{75, 0, 0, 0}};
  decimal b = {{2, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 375);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 1);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_12) {
  // 4294967295 / 2 = 2147483647.5
  decimal a = {{UINT32_MAX, 0, 0, 0}};
  decimal b = {{2, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 4294967291);
  ck_assert_int_eq(result.bits[1], 4);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 1);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_div_13) {
  // 0.3651389474969235244364647869 / 2 = 0.1825694737484617622182323934
  decimal a = {{661441981, 290415008, 197942220, 28 << 16}};
  decimal b = {{2, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_div(a, b, &result);

  ck_assert_int_eq(result.bits[0], 330720990);
  ck_assert_int_eq(result.bits[1], 145207504);
  ck_assert_int_eq(result.bits[2], 98971110);
  ck_assert_int_eq((result.bits[3] >> 16) & 0xFF, 28);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_floor_1) {
  // 0 - 0.01 = -0.01
  decimal a = {{7, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_floor(a, &result);

  ck_assert_int_eq(result.bits[0], 7);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_floor_2) {
  decimal a = {{157, 0, 0, 2147549184}};
  decimal result = {{0}};

  int status = decimal_floor(a, &result);

  ck_assert_int_eq(result.bits[0], 16);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 2147483648);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_floor_3) {
  decimal a = {{159, 0, 0, 65536}};
  decimal result = {{0}};
  int status = decimal_floor(a, &result);

  ck_assert_int_eq(result.bits[0], 15);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_floor_4) {
  decimal a = {{0, 0, 0, 0}};
  decimal result = {{12345, 0, 0, 0}};  // начальное мусорное значение
  int status = decimal_floor(a, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_floor_5) {
  decimal a = {{999999999, 0, 0, 2147549184}};
  decimal result = {{0}};
  int status = decimal_floor(a, &result);

  ck_assert_int_eq(result.bits[0], 100000000);
  ck_assert_int_eq(result.bits[3], 2147483648);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_floor_6) {
  decimal a = {{123456789, 0, 0, 1835008}};
  decimal result = {{0}};
  int status = decimal_floor(a, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_floor_7) {
  decimal a = {{101, 0, 0, 2147549184}};
  decimal result = {{0}};
  int status = decimal_floor(a, &result);

  ck_assert_int_eq(result.bits[0], 11);  // -11
  ck_assert_int_eq(result.bits[3], 2147483648);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_floor_8) {
  decimal a = {{123456789, 0, 0, 1835008}};
  decimal result = {{0}};
  int status = decimal_floor(a, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_floor_9) {
  decimal a = {{101, 0, 0, 2147549184}};
  decimal result = {{0}};
  int status = decimal_floor(a, &result);

  ck_assert_int_eq(result.bits[0], 11);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 2147483648);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_round_1) {
  decimal value = {{0}};
  decimal result = {{0}};
  int status = 0;

  value.bits[0] = 5;
  decimal_round_to_bigger_value(&status, value, &result);

  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(result.bits[0], 6);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
}
END_TEST

START_TEST(test_decimal_round_2) {
  decimal value = {{0}};
  decimal result = {{0}};
  int status = 0;

  value.bits[0] = UINT32_MAX;
  decimal_round_to_bigger_value(&status, value, &result);

  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
}
END_TEST

START_TEST(test_decimal_round_3) {
  decimal value = {{0}};
  decimal result = {{0}};
  int status = 0;

  value.bits[0] = UINT32_MAX;
  value.bits[1] = UINT32_MAX;
  value.bits[2] = UINT32_MAX;

  decimal_round_to_bigger_value(&status, value, &result);

  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], UINT32_MAX);
  ck_assert_int_eq(result.bits[2], UINT32_MAX);
}
END_TEST

START_TEST(test_decimal_round_4) {
  decimal value = {{0}};
  decimal result = {{0}};
  int status = 0;

  value.bits[0] = 1234567890;
  value.bits[1] = 987654321;
  decimal_round_to_bigger_value(&status, value, &result);

  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(result.bits[0], 1234567891);
  ck_assert_int_eq(result.bits[1], 987654321);
  ck_assert_int_eq(result.bits[2], 0);
}
END_TEST

START_TEST(test_equal_1) {
  decimal a = {{12345, 0, 0, 0}};
  decimal b = {{12345, 0, 0, 0}};
  ck_assert_int_eq(is_equal(a, b), 1);
}
END_TEST

START_TEST(test_equal_2) {
  decimal a = {{12345, 0, 0, 0}};
  decimal b = {{54321, 0, 0, 0}};
  ck_assert_int_eq(is_equal(a, b), 0);
}
END_TEST

START_TEST(test_equal_3) {
  decimal a = {{1000, 0, 0, 65536}};    // 100.0
  decimal b = {{10000, 0, 0, 131072}};  // 100.00
  ck_assert_int_eq(is_equal(a, b), 1);
}
END_TEST

START_TEST(test_equal_4) {
  decimal a = {{500, 0, 0, 0}};
  decimal b = {{500, 0, 0, 2147483648}};
  ck_assert_int_eq(is_equal(a, b), 0);
}
END_TEST

START_TEST(test_equal_5) {
  decimal a = {{5000, 0, 0, 0}};
  decimal b = {{500, 0, 0, 0}};
  ck_assert_int_eq(is_equal(a, b), 0);
}
END_TEST

START_TEST(test_less_1) {
  decimal a = {{100, 0, 0, 0}};
  decimal b = {{200, 0, 0, 0}};
  ck_assert_int_eq(is_less(a, b), 1);
}
END_TEST

START_TEST(test_less_2) {
  decimal a = {{12345, 0, 0, 0}};
  decimal b = {{12345, 0, 0, 0}};
  ck_assert_int_eq(is_less(a, b), 0);
}
END_TEST

START_TEST(test_less_3) {
  decimal a = {{123, 0, 0, 2147483648}};
  decimal b = {{123, 0, 0, 0}};
  ck_assert_int_eq(is_less(a, b), 1);
}
END_TEST

START_TEST(test_less_4) {
  decimal a = {{500, 0, 0, 0}};
  decimal b = {{499, 0, 0, 2147483648}};
  ck_assert_int_eq(is_less(a, b), 0);
}
END_TEST

START_TEST(test_less_or_equal_1) {
  decimal a = {{1000, 0, 0, 2147483648}};
  decimal b = {{1000, 0, 0, 2147483648}};
  ck_assert_int_eq(is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_less_or_equal_2) {
  decimal a = {{1111, 0, 0, 0}};
  decimal b = {{2222, 0, 0, 0}};
  ck_assert_int_eq(is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_less_or_equal_3) {
  decimal a = {{2222, 0, 0, 0}};
  decimal b = {{1111, 0, 0, 0}};
  ck_assert_int_eq(is_less_or_equal(a, b), 0);
}
END_TEST

START_TEST(test_less_or_equal_4) {
  decimal a = {{0}}, b = {{0}};

  a.bits[0] = 123;
  b.bits[0] = 123;
  ck_assert_int_eq(is_less_or_equal(a, b), 1);

  a.bits[0] = 100;
  b.bits[0] = 200;
  ck_assert_int_eq(is_less_or_equal(a, b), 1);

  a.bits[0] = 300;
  b.bits[0] = 200;
  ck_assert_int_eq(is_less_or_equal(a, b), 0);

  a.bits[0] = 100;
  a.bits[3] |= (1u << 31);
  b.bits[0] = 200;
  b.bits[3] = 0;
  ck_assert_int_eq(is_less_or_equal(a, b), 1);

  a.bits[0] = 200;
  a.bits[3] = 0;
  b.bits[0] = 100;
  b.bits[3] |= (1u << 31);
  ck_assert_int_eq(is_less_or_equal(a, b), 0);

  a.bits[0] = 200;
  a.bits[3] |= (1u << 31);
  b.bits[0] = 100;
  b.bits[3] |= (1u << 31);
  ck_assert_int_eq(is_less_or_equal(a, b), 1);

  a.bits[0] = 100;
  a.bits[3] |= (1u << 31);
  b.bits[0] = 200;
  b.bits[3] |= (1u << 31);
  ck_assert_int_eq(is_less_or_equal(a, b), 0);
}
END_TEST

START_TEST(test_not_equal_1) {
  decimal a = {{12345, 0, 0, 0}};
  decimal b = {{12344, 0, 0, 0}};
  ck_assert_int_eq(is_not_equal(a, b), 1);
}
END_TEST

START_TEST(test_not_equal_2) {
  decimal a = {{1000, 0, 0, 2147483648}};
  decimal b = {{1000, 0, 0, 2147483648}};
  ck_assert_int_eq(is_not_equal(a, b), 0);
}
END_TEST

START_TEST(test_not_equal_3) {
  decimal a = {{1000, 0, 0, 1 << 16}};
  decimal b = {{100, 0, 0, 0}};
  ck_assert_int_eq(is_not_equal(a, b), 0);
}
END_TEST

START_TEST(test_not_equal_4) {
  decimal a = {{56789, 0, 0, 2147483648}};
  decimal b = {{56789, 0, 0, 0}};
  ck_assert_int_eq(is_not_equal(a, b), 1);
}
END_TEST

START_TEST(test_greater_1) {
  decimal a = {{12345, 0, 0, 0}};
  decimal b = {{12344, 0, 0, 0}};
  ck_assert_int_eq(is_greater(a, b), 1);
}
END_TEST

START_TEST(test_greater_2) {
  decimal a = {{1111, 0, 0, 0}};
  decimal b = {{2222, 0, 0, 0}};
  ck_assert_int_eq(is_greater(a, b), 0);
}
END_TEST

START_TEST(test_greater_3) {
  decimal a = {{2, 0, 0, 2147483648}};
  decimal b = {{4, 0, 0, 2147483648}};
  ck_assert_int_eq(is_greater(a, b), 1);
}
END_TEST

START_TEST(test_greater_4) {
  decimal a = {{22222, 0, 0, 2147483648}};
  decimal b = {{22222, 0, 0, 2147483648}};
  ck_assert_int_eq(is_greater(a, b), 0);
}
END_TEST

START_TEST(test_greater_5) {
  decimal a = {{22222, 0, 0, 0}};
  decimal b = {{22222, 0, 0, 0}};
  ck_assert_int_eq(is_greater(a, b), 0);
}
END_TEST

START_TEST(test_greater_6) {
  decimal a = {{22222, 0, 0, 0}};
  decimal b = {{11111, 0, 0, 2147483648}};
  ck_assert_int_eq(is_greater(a, b), 1);
}
END_TEST

START_TEST(test_greater_7) {
  decimal a = {{12345, 0, 0, 2147483648}};
  decimal b = {{12345, 0, 0, 0}};
  ck_assert_int_eq(is_greater(a, b), 0);
}
END_TEST

START_TEST(test_greater_or_equal_1) {
  decimal a = {{12345, 0, 0, 0}};
  decimal b = {{12345, 0, 0, 0}};
  ck_assert_int_eq(is_greater_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_greater_or_equal_2) {
  decimal a = {{11111, 0, 0, 2147483648}};
  decimal b = {{22222, 0, 0, 2147483648}};
  ck_assert_int_eq(is_greater_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_greater_or_equal_3) {
  decimal a = {{56789, 0, 0, 2147483648}};
  decimal b = {{56789, 0, 0, 2147483648}};
  ck_assert_int_eq(is_greater_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_greater_or_equal_4) {
  decimal a = {{22221, 0, 0, 0}};
  decimal b = {{22222, 0, 0, 0}};
  ck_assert_int_eq(is_greater_or_equal(a, b), 0);
}
END_TEST

START_TEST(test_greater_or_equal_5) {
  decimal a = {{12345, 0, 0, 2147483648}};
  decimal b = {{12345, 0, 0, 0}};
  ck_assert_int_eq(is_greater_or_equal(a, b), 0);
}
END_TEST

START_TEST(test_greater_or_equal_6) {
  decimal a = {{12345, 0, 0, 0}};
  decimal b = {{22222, 0, 0, 2147483648}};
  ck_assert_int_eq(is_greater_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_decimal_negate_0) {
  decimal a = {{1000, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_negate(a, &result);

  ck_assert_int_eq(result.bits[0], 1000);
  ck_assert_int_eq(result.bits[3], 2147483648);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_negate_1) {
  decimal a = {{5000, 0, 0, 2147483648}};
  decimal result = {{0}};
  int status = decimal_negate(a, &result);

  ck_assert_int_eq(result.bits[0], 5000);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_negate_2) {
  decimal a = {{0, 0, 0, 0}};
  decimal result = {{123, 456, 789, 10}};
  int status = decimal_negate(a, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 2147483648);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_negate_3) {
  decimal a = {{123, 0, 0, 0}};
  int status = decimal_negate(a, NULL);

  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_decimal_truncate_1) {
  decimal a = {{12345, 0, 0, 65536}};  // 1234.5
  decimal result = {{0}};
  int status = decimal_truncate(a, &result);

  ck_assert_int_eq(result.bits[0], 1234);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_truncate_2) {
  decimal a = {{6789, 0, 0, 2147549184}};  // -678.9
  decimal result = {{0}};
  int status = decimal_truncate(a, &result);

  ck_assert_int_eq(result.bits[0], 678);
  ck_assert_int_eq(result.bits[3], 2147483648);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_truncate_3) {
  decimal a = {{777, 0, 0, 0}};
  decimal result = {{0}};
  int status = decimal_truncate(a, &result);

  ck_assert_int_eq(result.bits[0], 777);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_truncate_4) {
  decimal a = {{0, 0, 0, 131072}};  // 0.00
  decimal result = {{1, 2, 3, 4}};
  int status = decimal_truncate(a, &result);

  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
  ck_assert_int_eq(status, 0);
}
END_TEST

START_TEST(test_decimal_truncate_5) {
  decimal a = {{12345, 0, 0, 65536}};  // 1234.5
  int status = decimal_truncate(a, NULL);

  ck_assert_int_eq(status, 1);
}
END_TEST

START_TEST(test_decimal_round) {
  decimal a = {{0}}, res = {{0}};

  a.bits[0] = 12345;
  a.bits[3] = 3 << 16;
  ck_assert_int_eq(decimal_round(a, &res), 0);
  ck_assert_int_eq(res.bits[0], 12);
  ck_assert_int_eq(res.bits[1], 0);
  ck_assert_int_eq(res.bits[2], 0);
  ck_assert_int_eq(res.bits[3], 0);

  a.bits[0] = 12345;
  a.bits[3] = (3 << 16) | (1u << 31);
  ck_assert_int_eq(decimal_round(a, &res), 0);
  ck_assert_int_eq(res.bits[0], 12);
  ck_assert_int_eq(res.bits[3] >> 31, 1);

  a.bits[0] = 15;
  a.bits[3] = 1 << 16;
  ck_assert_int_eq(decimal_round(a, &res), 0);
  ck_assert_int_eq(res.bits[0], 2);

  a.bits[0] = 25;
  a.bits[3] = 1 << 16;
  ck_assert_int_eq(decimal_round(a, &res), 0);
  ck_assert_int_eq(res.bits[0], 3);

  a.bits[0] = 4294967295u;
  a.bits[1] = 4294967295u;
  a.bits[2] = 4294967295u;
  a.bits[3] = 10 << 16;
  ck_assert_int_eq(decimal_round(a, &res), 0);
}
END_TEST

Suite *decimal_suite(void) {
  Suite *s = suite_create("Decimal");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_decimal_add_1);
  tcase_add_test(tc_core, test_decimal_add_2);
  tcase_add_test(tc_core, test_decimal_add_3);
  tcase_add_test(tc_core, test_decimal_add_4);
  tcase_add_test(tc_core, test_decimal_add_5);
  tcase_add_test(tc_core, test_decimal_add_6);
  tcase_add_test(tc_core, test_decimal_add_7);
  tcase_add_test(tc_core, test_decimal_add_8);
  tcase_add_test(tc_core, test_decimal_add_9);
  tcase_add_test(tc_core, test_decimal_add_10);
  tcase_add_test(tc_core, test_decimal_add_11);
  tcase_add_test(tc_core, test_decimal_add_13);
  tcase_add_test(tc_core, test_decimal_add_14);
  tcase_add_test(tc_core, test_decimal_add_15);
  tcase_add_test(tc_core, test_decimal_add_16);
  tcase_add_test(tc_core, test_decimal_add_17);
  tcase_add_test(tc_core, test_decimal_add_18);
  tcase_add_test(tc_core, test_decimal_add_19);
  tcase_add_test(tc_core, test_decimal_add_20);
  tcase_add_test(tc_core, test_decimal_add_21);
  tcase_add_test(tc_core, test_decimal_sub_1);
  tcase_add_test(tc_core, test_decimal_sub_2);
  tcase_add_test(tc_core, test_decimal_sub_3);
  tcase_add_test(tc_core, test_decimal_sub_4);
  tcase_add_test(tc_core, test_decimal_sub_5);
  tcase_add_test(tc_core, test_decimal_sub_6);
  tcase_add_test(tc_core, test_decimal_sub_7);
  tcase_add_test(tc_core, test_decimal_sub_8);
  tcase_add_test(tc_core, test_decimal_sub_9);
  tcase_add_test(tc_core, test_decimal_sub_10);
  tcase_add_test(tc_core, test_decimal_sub_11);
  tcase_add_test(tc_core, test_decimal_sub_12);
  tcase_add_test(tc_core, test_decimal_sub_13);
  tcase_add_test(tc_core, test_decimal_mul_1);
  tcase_add_test(tc_core, test_decimal_mul_2);
  tcase_add_test(tc_core, test_decimal_mul_3);
  tcase_add_test(tc_core, test_decimal_mul_4);
  tcase_add_test(tc_core, test_decimal_mul_5);
  tcase_add_test(tc_core, test_decimal_mul_6);
  tcase_add_test(tc_core, test_decimal_mul_7);
  tcase_add_test(tc_core, test_decimal_mul_8);
  tcase_add_test(tc_core, test_decimal_mul_9);
  tcase_add_test(tc_core, test_decimal_mul_10);
  tcase_add_test(tc_core, test_decimal_mul_11);
  tcase_add_test(tc_core, test_decimal_mul_12);
  tcase_add_test(tc_core, test_decimal_mul_13);
  tcase_add_test(tc_core, test_decimal_div_1);
  tcase_add_test(tc_core, test_decimal_div_2);
  tcase_add_test(tc_core, test_decimal_div_3);
  tcase_add_test(tc_core, test_decimal_div_4);
  tcase_add_test(tc_core, test_decimal_div_5);
  tcase_add_test(tc_core, test_decimal_div_6);
  tcase_add_test(tc_core, test_decimal_div_7);
  tcase_add_test(tc_core, test_decimal_div_8);
  tcase_add_test(tc_core, test_decimal_div_9);
  tcase_add_test(tc_core, test_decimal_div_10);
  tcase_add_test(tc_core, test_decimal_div_11);
  tcase_add_test(tc_core, test_decimal_div_12);
  tcase_add_test(tc_core, test_decimal_div_13);
  tcase_add_test(tc_core, test_decimal_floor_1);
  tcase_add_test(tc_core, test_decimal_floor_2);
  tcase_add_test(tc_core, test_decimal_floor_3);
  tcase_add_test(tc_core, test_decimal_floor_4);
  tcase_add_test(tc_core, test_decimal_floor_5);
  tcase_add_test(tc_core, test_decimal_floor_6);
  tcase_add_test(tc_core, test_decimal_floor_7);
  tcase_add_test(tc_core, test_decimal_floor_8);
  tcase_add_test(tc_core, test_decimal_floor_9);
  tcase_add_test(tc_core, test_decimal_round_1);
  tcase_add_test(tc_core, test_decimal_round_2);
  tcase_add_test(tc_core, test_decimal_round_3);
  tcase_add_test(tc_core, test_decimal_round_4);
  tcase_add_test(tc_core, test_equal_1);
  tcase_add_test(tc_core, test_equal_2);
  tcase_add_test(tc_core, test_equal_3);
  tcase_add_test(tc_core, test_equal_4);
  tcase_add_test(tc_core, test_equal_5);
  tcase_add_test(tc_core, test_less_1);
  tcase_add_test(tc_core, test_less_2);
  tcase_add_test(tc_core, test_less_3);
  tcase_add_test(tc_core, test_less_4);
  tcase_add_test(tc_core, test_less_or_equal_1);
  tcase_add_test(tc_core, test_less_or_equal_2);
  tcase_add_test(tc_core, test_less_or_equal_3);
  tcase_add_test(tc_core, test_less_or_equal_4);
  tcase_add_test(tc_core, test_not_equal_1);
  tcase_add_test(tc_core, test_not_equal_2);
  tcase_add_test(tc_core, test_not_equal_3);
  tcase_add_test(tc_core, test_not_equal_4);
  tcase_add_test(tc_core, test_greater_1);
  tcase_add_test(tc_core, test_greater_2);
  tcase_add_test(tc_core, test_greater_3);
  tcase_add_test(tc_core, test_greater_4);
  tcase_add_test(tc_core, test_greater_5);
  tcase_add_test(tc_core, test_greater_6);
  tcase_add_test(tc_core, test_greater_7);
  tcase_add_test(tc_core, test_greater_or_equal_1);
  tcase_add_test(tc_core, test_greater_or_equal_2);
  tcase_add_test(tc_core, test_greater_or_equal_3);
  tcase_add_test(tc_core, test_greater_or_equal_4);
  tcase_add_test(tc_core, test_greater_or_equal_5);
  tcase_add_test(tc_core, test_greater_or_equal_6);
  tcase_add_test(tc_core, test_decimal_negate_0);
  tcase_add_test(tc_core, test_decimal_negate_1);
  tcase_add_test(tc_core, test_decimal_negate_2);
  tcase_add_test(tc_core, test_decimal_negate_3);
  tcase_add_test(tc_core, test_decimal_truncate_1);
  tcase_add_test(tc_core, test_decimal_truncate_2);
  tcase_add_test(tc_core, test_decimal_truncate_3);
  tcase_add_test(tc_core, test_decimal_truncate_4);
  tcase_add_test(tc_core, test_decimal_truncate_5);
  tcase_add_test(tc_core, test_decimal_round);

  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  Suite *s = decimal_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
