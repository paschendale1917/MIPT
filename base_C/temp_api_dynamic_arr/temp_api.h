#ifndef temp_api_h
#define temp_api_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define STRING_LENGTH       22
#define DATA_ERROR          -127
#define ERROR               1
#define SUCCESS             0

#define DEBUG

#ifdef  DEBUG 
  extern const char csvfile_name[];
  extern const char csvbigfile_name[];
#endif

enum {
  january = 1,
  february,
  march,
  april,
  may,
  june,
  july,
  august,
  september,
  october,
  november,
  december
};

typedef struct {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  int8_t temp;
} record;

typedef record datatype;

typedef struct stack {
  int32_t capacity;  //длина стека(вместимость стека)
  uint32_t size;      //текущее количество элементов в стеке
  datatype *meas;       //указатель на массив элементов стека
} stack;

typedef struct errors {
  int32_t capacity;  //длина стека(вместимость стека)
  uint32_t size;      //текущее количество элементов в стеке
  uint32_t *err;       //указатель на массив элементов стека
} errors;


extern stack full_data;
extern stack m_data;
extern errors error_stack;


int32_t char2num(char *p, char stop_symb);
int8_t read_full_data(stack *dt, const char *csv_name) ;
uint8_t print_month_info(const char *csv_name, uint8_t num_month);
void print_yearstat_info(const char *csv_name);
#endif