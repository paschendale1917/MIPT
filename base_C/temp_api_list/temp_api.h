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
#define DATA_MONTH_AV      2

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

typedef struct list{
  int32_t meas; // номер измерения
  datatype measure; //струтура для записи данных 
  struct list *next;  //указатель на следующий элемент
}list;


extern list* m_data_head;
extern list* full_data_head; // указатель на голову списка full_data

int32_t char2num(char *p, char stop_symb);
int8_t read_full_data(list **dt, const char *csv_name) ;
int8_t read_month_data(list **dt, const char *csv_name, uint8_t num_month, uint32_t *err);
uint8_t print_month_info(const char *csv_name, uint8_t num_month);
void print_yearstat_info(const char *csv_name);
#endif