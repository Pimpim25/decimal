#pragma once

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t bits[4];
} decimal;

typedef struct {
  uint32_t bits[7];
} decimal_ex;

int decimal_add(decimal value_1, decimal value_2, decimal *result);
int decimal_sub(decimal value_1, decimal value_2, decimal *result);
int decimal_mul(decimal value_1, decimal value_2, decimal *result);
int decimal_div(decimal value_1, decimal value_2, decimal *result);
void decimal_addition(int i, decimal_ex value_1, decimal_ex value_2,
              decimal_ex *result);
void use_scale(decimal value, decimal_ex **new_value, int scale,
               int small_scale);
void preuse_scale(int with_scale, decimal value, decimal_ex *new_value);
void clear_decimal_ex(decimal_ex *num);
void decimal_subtraction(int i, decimal_ex value_1, decimal_ex value_2,
                 decimal_ex *result, int borrow);
void decimal_multiplication(decimal_ex value_1, decimal_ex value_2,
                    decimal_ex *result);
void decimal_division(int *scale, decimal_ex value_1, decimal_ex value_2,
              decimal_ex *result);
void work_with_remainder(int *scale, decimal_ex remainder,
                         decimal_ex value_2, decimal_ex *result);
void bits_shift_left(int i, int j, decimal_ex *num);
void use_scale_and_sign(int decimal_add_scale, int scale_sign, decimal value_1,
                        decimal value_2, decimal_ex *result);
int reduce_scale_bank_decimal_rounding_manual(decimal_ex *value);
void decimal_to_string(decimal_ex dec, char *out, int out_size);
void change_end(int *end, decimal_ex new_result, decimal *result);
void prepare_values(decimal value_1, decimal value_2,
                    decimal_ex *new_value_1, decimal_ex *new_value_2);
int choose_the_sign(decimal_ex value_1, decimal_ex value_2);

int is_less(decimal value_1, decimal value_2);
int is_less_or_equal(decimal value_1, decimal value_2);
int is_equal(decimal value_1, decimal value_2);
int is_not_equal(decimal value_1, decimal value_2);
int is_greater(decimal value_1, decimal value_2);
int is_greater_or_equal(decimal value_1, decimal value_2);

int decimal_floor(decimal value, decimal *result);
int decimal_round(decimal value, decimal *result);
int pre_decimal_round(int *status, decimal *value);
void decimal_round_to_bigger_value(int *status, decimal value, decimal *result);

int decimal_truncate(decimal value, decimal *result);
int decimal_negate(decimal value, decimal *result);
