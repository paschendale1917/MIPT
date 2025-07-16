#ifndef temp_api_h
#define temp_api_h

#include <stdint.h>
#include <stdio.h>

#define NUM_MEAS_PER_MONTH  (31*24*60)
#define STRING_LENGTH        22
#define DATA_ERROR          -127
#define ERROR               1
#define SUCCESS             0

#ifdef  DEBUG 
  #define NUM_MEAS            100
#else
  #define NUM_MEAS            (365*NUM_MEAS_PER_MONTH)
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

typedef struct {
  int32_t meas_amount;
  record measure[(uint32_t)NUM_MEAS];
} data;

typedef struct {
  int32_t meas_month;
  record measure_month[(uint32_t)NUM_MEAS_PER_MONTH];
} data_month;

extern data full_data;
extern data_month m_data;

extern const char csvfile_name[];
extern const char csvbigfile_name[];

int32_t char2num(char *p, char stop_symb);
int8_t read_data(data *dt, char *csv_name);
void get_month_data(data *dt_source, data_month *dt_dest, uint8_t month_number);
float month_average_temp(data_month *dt);
int8_t month_min_temp(data_month *dt);
int8_t month_max_temp(data_month *dt);
uint8_t print_month_info(data *dt, data_month *dt_dest, uint8_t num_month);
void print_yearstat_info(data *dt_source, data_month *dt_dest);
uint64_t encode_date(record *dt);
void sortByTemp(data_month *dt);

#endif