#include "temp_api.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const char csvfile_name[] = "temperature_small.csv";
const char csvbigfile_name[] = "temperature_big.csv";

const char stat_month[] = "STATISTICS BY MONTH";
const char stat_month_by_year[] = "STATISTICS BY MONTH FOR THE YEAR";
const char stat_year[] = "STATISTICS FOR THE YEAR";

data full_data = {0};
data_month m_data = {0};

const char month_name[12][10] = {
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"};

void swap(record *dt, uint32_t i, uint32_t j) {
  record temp = dt[i];
  dt[i] = dt[j];
  dt[j] = temp;
}

void quick_sortByTemp(data_month *dt, uint32_t left, uint32_t right) {
  if (!dt->meas_month || left >= right) return;
  uint32_t i = left, j = right;
  record pivot = dt->measure_month[(left + right) / 2];
  while (i <= j) {
    while (dt->measure_month[i].temp < pivot.temp) i++;
    while (dt->measure_month[j].temp > pivot.temp) j--;
    if (i <= j) {
      swap(dt->measure_month, i, j);
      i++;
      if (j > 0) j--;
      else break;
    }
  }
  if (left < j) quick_sortByTemp(dt, left, j);
  if (i < right) quick_sortByTemp(dt, i, right);
}

void sortByTemp(data_month *dt) {
  if (!dt->meas_month) return;
  for (uint32_t i = 0; i < dt->meas_month; ++i)
    for (uint32_t j = i + 1; j < dt->meas_month; ++j)
      if (dt->measure_month[i].temp > dt->measure_month[j].temp)
        swap(dt->measure_month, i, j);
}

uint64_t encode_date(record *dt) {
  return ((uint64_t)dt->year << 32) | ((uint32_t)dt->month << 24) |
         ((uint32_t)dt->day << 16) | ((uint32_t)dt->hour << 8) | dt->min;
}

void sortByDate(data_month *dt) {
  for (uint32_t i = 0; i < dt->meas_month; ++i)
    for (uint32_t j = i + 1; j < dt->meas_month; ++j)
      if (encode_date(&dt->measure_month[i]) > encode_date(&dt->measure_month[j]))
        swap(dt->measure_month, i, j);
}

void add_record(record *dt, uint32_t num_meas, uint16_t year, uint8_t month,
                uint8_t day, uint8_t hour, uint8_t min, int8_t temp) {
  dt[num_meas].year = year;
  dt[num_meas].month = month;
  dt[num_meas].day = day;
  dt[num_meas].hour = hour;
  dt[num_meas].min = min;
  dt[num_meas].temp = temp;
}

void clear_record(record *dt, uint32_t num_meas) {
  dt[num_meas].year = 0;
  dt[num_meas].month = 0;
  dt[num_meas].day = 0;
  dt[num_meas].hour = 0;
  dt[num_meas].min = 0;
  dt[num_meas].temp = 0;
}

void clear_month_data(data_month *dt) {
  for (uint16_t i = 0; i < dt->meas_month; i++) {
    clear_record(dt->measure_month, i);
  }
  dt->meas_month = 0;
}

void get_month_data(data *dt_source, data_month *dt_dest, uint8_t month_number) {
  uint32_t num_records = 0;
  for (uint32_t i = 0; i <= dt_source->meas_amount; i++) {
    if (dt_source->measure[i].month == month_number) {
      if (num_records < NUM_MEAS_PER_MONTH)
        dt_dest->measure_month[num_records++] = dt_source->measure[i];
    }
  }
  dt_dest->meas_month = num_records;
}

float month_average_temp(data_month *dt) {
  int32_t result = 0;
  if (!dt->meas_month) return 0;
  for (uint16_t i = 0; i < dt->meas_month; i++) {
    result += dt->measure_month[i].temp;
  }
  return (float)result / dt->meas_month;
}

int8_t month_min_temp(data_month *dt) {
  if (!dt->meas_month) return 0;
  quick_sortByTemp(dt, 0, dt->meas_month - 1);
  return dt->measure_month[0].temp;
}

int8_t month_max_temp(data_month *dt) {
  if (!dt->meas_month) return 0;
  quick_sortByTemp(dt, 0, dt->meas_month - 1);
  return dt->measure_month[dt->meas_month - 1].temp;
}

float year_average_temp(data *dt) {
  float result = 0;
  if (!dt->meas_amount) return 0;
  for (int32_t i = 0; i <= dt->meas_amount; i++) {
    result += dt->measure[i].temp;
  }
  return result / (dt->meas_amount + 1);
}

void print_title(void) {
  printf("%-8s %-12s %-21s %-10s %13s\n", "year", "month", "average_temp", "min_temp", "max_temp");
  printf("-----   ----------    ------------         ----------      ----------\n");
}

void print_title_name(const char *title_name) {
  printf("\n====================%s====================\n\n", title_name);
}

uint8_t print_month_data(data_month *dt_dest, uint8_t num_month, float average) {
  if (!dt_dest->meas_month) {
    printf("There is no valid data for %s\n", month_name[num_month - 1]);
    return ERROR;
  } else {
    printf("%-8d %-15s %0.1f%-14s %4d%-12s %4d%s\n",
           dt_dest->measure_month[0].year, month_name[num_month - 1],
           average, "°C", dt_dest->measure_month[0].temp,
           "°C", dt_dest->measure_month[dt_dest->meas_month - 1].temp,
           "°C");
  }
  return SUCCESS;
}

void print_year_data(float avg, int8_t max, int8_t min, uint8_t num_month) {
  printf("%-15s %-10s %s\n", "average_temp", "min_temp", "max_temp");
  printf("-------------  ---------  --------\n");
  if (num_month)
    printf("%s %0.1f%s%s %d%s%s %d%s\n\n", "   ", avg / num_month, "°C", "       ",
           min, "°C", "      ", max, "°C");
  else
    printf("There is no data by month\n\n");
}

uint8_t print_month_info(data *dt_source, data_month *dt_dest, uint8_t num_month) {
  float av = 0;
  // if (num_month > 12 || num_month < 1) {
  //   printf("\nPlease enter the correct month\n\n");
  //   return DATA_ERROR;
  // }
  if (!dt_source->meas_amount) {
    printf("\nThere is no valid data for %s\n\n", month_name[num_month - 1]);
    return ERROR;
  }
  get_month_data(dt_source, dt_dest, num_month);
  av = month_average_temp(dt_dest);
  quick_sortByTemp(dt_dest, 0, dt_dest->meas_month - 1);
  print_title_name(stat_month);
  print_title();
  print_month_data(dt_dest, num_month, av);
  printf("\n");
  return SUCCESS;
}

void print_yearstat_info(data *dt_source, data_month *dt_dest) {
  int8_t year_max = -127, year_min = 127;
  uint8_t valid_months = 0;
  float year_avg = 0;
  print_title_name(stat_month_by_year);
  print_title();
  for (uint8_t j = 1; j <= 12; j++) {
    float av = 0;
    get_month_data(dt_source, dt_dest, j);
    quick_sortByTemp(dt_dest, 0, dt_dest->meas_month - 1);
    av = month_average_temp(dt_dest);
    if (dt_dest->meas_month) {
      valid_months++;
      year_min = year_min > dt_dest->measure_month[0].temp ? dt_dest->measure_month[0].temp : year_min;
      year_max = year_max < dt_dest->measure_month[dt_dest->meas_month - 1].temp ? dt_dest->measure_month[dt_dest->meas_month - 1].temp : year_max;
      year_avg += av;
    }
    print_month_data(dt_dest, j, av);
    clear_month_data(dt_dest);
  }
  print_title_name(stat_year);
  print_year_data(year_avg, year_max, year_min, valid_months);
}

int32_t char2num(char *p, char stop_symb) {
  int32_t num = 0;
  uint8_t negative = 0;
  while (*p != stop_symb && *p != '\n' && *p != '\r' && *p != '\0') {
    if (*p == ' ') {
      p++;
      continue;
    }
    if (*p == '-') {
      negative = 1;
      p++;
    } else if (*p >= '0' && *p <= '9') {
      num = num * 10 + (*p - '0');
      p++;
    } else {
      return DATA_ERROR;
    }
  }
  return negative ? -num : num;
}

void clear_string(char *string) {
  for (uint8_t i = 0; i < STRING_LENGTH; i++) {
    string[i] = 0;
  }
}

int8_t read_data(data *dt, char *csv_name) {
  uint32_t meas_cntr = 0;
  int year = 0, month = 0, day = 0, hour = 0, min = 0, temp = 0;
  uint32_t string_cntr = 0;
  FILE *rd = fopen(csv_name, "rb");
  if (rd == NULL) {
    //printf("\nWarning: read error or unknown option!\n\n");
    return ERROR;
  }
  while (1) {
    int n_scan = fscanf(rd, "%d;%d;%d;%d;%d;%d", &year, &month, &day, &hour, &min, &temp);
    if (n_scan == EOF) break;
    if (n_scan < 6) {
      char s[STRING_LENGTH];
      if (n_scan == -1) {
        fclose(rd);
        return DATA_ERROR;
      }
      fscanf(rd, "%[^\n]", s);
      printf("\nString №%u data format not supported. Skipping.\n", ++string_cntr);
      continue;
    }
    string_cntr++;
    if (meas_cntr < NUM_MEAS) {
      add_record(dt->measure, meas_cntr, year, month, day, hour, min, temp);
      dt->meas_amount = meas_cntr;
      meas_cntr++;
    } else {
      printf("\nWarning: measurement array overflow, skipping data.\n");
      break;
    }
  }
  fclose(rd);
  return SUCCESS;
}