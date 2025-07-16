#include "temp_api.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const char stat_month[] = "STATISTICS BY MONTH";
const char stat_month_by_year[] = "STATISTICS BY MONTH FOR THE YEAR";
const char stat_year[] = "STATISTICS FOR THE YEAR";

#ifdef DEBUG
const char csvfile_name[] = "temperature_small.csv";
const char csvbigfile_name[] = "temperature_big.csv";
#endif

/* data full_data = {0};
data_month m_data = {0}; */

typedef record datatype;
stack full_data = {0};
stack m_data = {0};

const char month_name[12][10] = {
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"};

void swap(record *dt, uint32_t i, uint32_t j) {
  record temp = dt[i];
  dt[i] = dt[j];
  dt[j] = temp;
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

void init_stack(stack *stack, uint32_t capacity) {
  stack->capacity =
      capacity; // заносим количество элементов типа record в стеке
  if (capacity < 1) {
    printf("Stack capacity must be at least 1.\n");
    return;
  }
  stack->size = 0; // заносим текущее количество элементов в стеке
  stack->meas = (datatype *)malloc(
      capacity * sizeof(datatype)); // в указатель стека заносим адрес памяти,
                                    // выделенной под массив record
  if (stack->meas ==
      NULL) { // если malloc вернул NULL, значит память не выделена
    printf("Memory allocation failed for stack.\n");
    return;
  }
}

void free_stack(stack *stack) {
  if (stack->meas) {
    free(stack->meas); // освобождаем память, выделенную под массив record
    stack->meas = NULL; // обнуляем указатель
  }
  stack->capacity = 0; // обнуляем размер стека
  stack->size = 0; // обнуляем текущее количество элементов в стеке
}

void push_data(stack *stack, datatype *data) {
  if (stack->size ==
      stack->capacity - 1) { // если размер стека равен его вместимости,
                             // увеличиваем размер стека
    stack->capacity *= 2; // увеличиваем размер стека в 2 раза
    stack->meas = (datatype *)realloc(
        stack->meas, stack->capacity *
                         sizeof(datatype)); // получаем указатель на увеличенный
                                            // в 2 раза участок памяти
    if (stack->meas ==NULL) { // если realloc вернул NULL, значит память не выделена
      printf("Memory reallocation failed for stack.\n");
      return;
    }
  }
  stack->meas[stack->size++] = *data; // добавляем элемент в стек и увеличиваем размер стека
}

uint8_t is_stack_empty(stack *st) {
  return (st->size == 0); // возвращаем 1, если стек пуст, иначе 0
}

void pop_data(stack *st, datatype *data) {
  if (!is_stack_empty(st)) { // если в стеке есть элементы
    *data = st->meas[--st->size]; // уменьшаем размер стека и возвращаем
                                  // последний элемент
  } else {
    printf("Stack is empty, cannot pop data.\n");
    return;
  }
}

int8_t read_full_data(stack *dt, const char *csv_name) {
  int32_t year = 0, month = 0, day = 0, hour = 0, min = 0, temp = 0;
  uint32_t string_cntr =0; // счетчик строк для вывода информации о некорректных данных
  uint32_t errors[65535]={0};
  uint32_t error_cntr = 0;
  uint32_t n_scan = 0;
  FILE *rd = fopen(csv_name, "rb");
  if (rd == NULL) {
    // printf("\nWarning: read error or unknown option!\n\n");
    return ERROR;
  }
  init_stack(dt, 1); // инициализируем стек
  while ((n_scan = fscanf(rd, "%d;%d;%d;%d;%d;%d", &year, &month, &day, &hour, &min, &temp)) != EOF) {
    if (n_scan < 6) {
      char s[STRING_LENGTH];
      if (n_scan == -1) {
        fclose(rd);
        return DATA_ERROR;
      }
       errors[error_cntr++] = ++string_cntr;
      fscanf(rd, "%[^\n]", s); // считываем строку без дальнейшего разбора для продвижения по файлу
      continue;
    }
    datatype add = {year, month, day, hour,
                    min,  temp}; // вносим в струтуру record считанные данные
    string_cntr++;
    push_data(dt, &add); // добавляем данные в стек
  }
 if(error_cntr){
    printf("\nStrings № ");
    for(uint32_t i = 0; i < error_cntr; i++) {
      printf("%u ", errors[i]);
    }
    printf("data format not supported. Data was skipped.\n");
  }
  fclose(rd);
  return SUCCESS;
}

int8_t read_month_data(stack *dt, const char *csv_name, uint8_t num_month, uint32_t *err) {
  int32_t year = 0, month = 0, day = 0, hour = 0, min = 0, temp = 0;
  uint32_t string_cntr = 0; // счетчик строк для вывода информации о некорректных данных
  uint32_t n_scan = 0;
  uint32_t error_cntr = 0;
  FILE *rd = fopen(csv_name, "rb");
  if (rd == NULL) {
    // printf("\nWarning: read error or unknown option!\n\n");
    return ERROR;
  }
  init_stack(dt, 1); // инициализируем стек
  while ((n_scan = fscanf(rd, "%d;%d;%d;%d;%d;%d", &year, &month, &day, &hour,
                          &min, &temp)) != EOF) {
    if (n_scan < 6) {
      char s[STRING_LENGTH];
      if (n_scan == -1) {
        fclose(rd);
        return DATA_ERROR;
      }
      err[error_cntr++] = ++string_cntr;
      fscanf(rd, "%[^\n]", s); // считываем строку без дальнейшего разбора для продвижения по файлу
     // printf("\nString №%u data format not supported. Skipping data.\n", ++string_cntr);
      continue;
    }
    string_cntr++;
    if (month == num_month) { // если месяц совпадает с указанным
      datatype add = {year, month, day, hour,
                      min,  temp}; // вносим в структуру record считанные данные
      push_data(dt, &add); // добавляем данные в стек
    }
  }
 
  fclose(rd);
  return SUCCESS;
}

void quick_sortByTemp(stack *dt, uint32_t left, uint32_t right) {
  if (!dt->meas->temp || left >= right)
    return;
  uint32_t i = left, j = right;
  record pivot = dt->meas[(left + right) / 2];
  while (i <= j) {
    while (dt->meas[i].temp < pivot.temp)
      i++;
    while (dt->meas[j].temp > pivot.temp)
      j--;
    if (i <= j) {
      swap(dt->meas, i, j);
      i++;
      if (j > 0)
        j--;
      else
        break;
    }
  }
  if (left < j)
    quick_sortByTemp(dt, left, j);
  if (i < right)
    quick_sortByTemp(dt, i, right);
}

uint64_t encode_date(record *dt) {
  return ((uint64_t)dt->year << 32) | ((uint32_t)dt->month << 24) |
         ((uint32_t)dt->day << 16) | ((uint32_t)dt->hour << 8) | dt->min;
}

float year_average_temp(stack *dt) {
  float result = 0;
  if (!dt->size)
    return 0;
  for (int32_t i = 0; i < dt->size; i++) {
    result += dt->meas[i].temp;
  }
  return result / dt->size;
}

float month_average_temp(stack *dt) {
  float result = 0;
  if (!dt->size)
    return 0;
  for (uint16_t i = 0; i < dt->size; i++) {
    result += dt->meas[i].temp;
  }
  return result / dt->size;
}




void print_title(void) {
  printf("%-8s %-12s %-21s %-10s %13s\n", "year", "month", "average_temp",
"min_temp", "max_temp");
printf("-----   ----------    ------------         ----------      ----------\n");
}

void print_title_name(const char *title_name) {
  printf("\n====================%s====================\n\n", title_name);
}

uint8_t print_month_data(stack *dt_dest, uint8_t num_month, float average){
   if (!dt_dest->size) {
     printf("There is no valid data for %s\n",month_name[num_month - 1]);
    return ERROR;
   } else { printf("%-8d %-15s %0.1f%-16s%4d%-12s %4d%s\n", dt_dest->meas[0].year, month_name[num_month - 1],
           average, "°C", dt_dest->meas[0].temp,
           "°C", dt_dest->meas[dt_dest->size - 1].temp,
           "°C");
  }
  return SUCCESS;
}


uint8_t print_month_info(const char *csv_name, uint8_t num_month) {
  float av = 0;
  stack m_data = {0};
  uint32_t errors[65535] = {0};
  read_month_data(&m_data, csv_name, num_month, errors);
   if (!m_data.size) {
    printf("There is no valid data for %s\n", month_name[num_month - 1]);
    return ERROR;
  }
  av = month_average_temp(&m_data);
  quick_sortByTemp(&m_data, 0, m_data.size - 1);
  print_title_name(stat_month);
  print_title();
  print_month_data(&m_data, num_month, av);
  //printf("\n");
 /*   if(errors[0]){
    printf("\nStrings № ");
    for(uint32_t i = 0; i < sizeof(errors)/sizeof(errors[0]); i++) {
      if(errors[i]){
        if (i > 0 && errors[i] == errors[i - 1]) continue; // пропускаем дубликаты
      printf("%u ", errors[i]);
    }
  }
  printf("data format not supported. Data was skipped.\n");
  } */
  return SUCCESS;
}


void print_year_data(float avg, int8_t max, int8_t min, uint8_t num_month) {
  printf("%-15s %-10s %s\n", "average_temp", "min_temp", "max_temp");
  printf("-------------  ---------  --------\n");
  if (num_month)
    printf(" %0.1f%-12s %d%-10s %d%s\n\n",  avg / num_month, "°C",
           min, "°C",  max, "°C");
  else
    printf("There is no data by month\n\n");
}

void print_yearstat_info(const char *csv_name) {
  int8_t year_max = -127, year_min = 127;
  uint8_t valid_months = 0;
  float year_avg = 0;
  stack m_data = {0};
  float av = 0;
  uint32_t errors[65535] = {0};
  print_title_name(stat_month_by_year);
  print_title();
  for (uint8_t j = 1; j <= 12; j++) {
     init_stack(&m_data,1);
    read_month_data(&m_data, csv_name, j, errors);
    if (!m_data.size) {
      printf("There is no valid data for %s\n", month_name[j - 1]);
    continue;
    }
    av = month_average_temp(&m_data);
    quick_sortByTemp(&m_data, 0, m_data.size - 1);
    if (m_data.size) {
      valid_months++;
      year_min = year_min > m_data.meas[0].temp ? m_data.meas[0].temp : year_min; 
      year_max = year_max < m_data.meas[m_data.size - 1].temp ? m_data.meas[m_data.size - 1].temp : year_max; year_avg += av;
    }
    print_month_data(&m_data, j, av);
    free_stack(&m_data);
  }
  printf("\n");
  print_title_name(stat_year);
  print_year_data(year_avg, year_max, year_min, valid_months);
    if(errors[0]){
    printf("\nStrings № ");
    for(uint32_t i = 0; i < sizeof(errors)/sizeof(errors[0]); i++) {
      if(errors[i]){
        if (!errors[i] && errors[i] == errors[i - 1]) continue; // пропускаем дубликаты и нули
      printf("%u ", errors[i]);
    }
  }
  printf("data format not supported. Data was skipped.\n");
  }

}


