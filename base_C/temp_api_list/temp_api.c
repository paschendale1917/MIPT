#include "temp_api.h"
#include <stdint.h>
#include <string.h>


const char stat_month[] = "STATISTICS BY MONTH";
const char stat_month_by_year[] = "STATISTICS BY MONTH FOR THE YEAR";
const char stat_year[] = "STATISTICS FOR THE YEAR";

#ifdef DEBUG
const char csvfile_name[] = "temperature_small.csv";
const char csvbigfile_name[] = "temperature_big.csv";
#endif


typedef record datatype;
list *full_data_head = NULL;
list* m_data_head = NULL;

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

void free_list(list *head) {
  list *current = head;
  while (current != NULL) {
    list *next = current->next;
    free(current);
    current = next;
  }
}

uint8_t is_list_empty(list *head) {
  return (head == NULL); // возвращаем 1, если список пуст, иначе 0
}

int8_t push_data(list **head, datatype data) {
  list *next_pointer = (list *)malloc(sizeof(list));
  if(next_pointer == NULL) {//если malloc вернул NULL, значит память не выделена
    printf("Memory allocation failed for new list.\n");
    return DATA_ERROR;
  }
  //next_pointer->num_meas = meas_amount; // пишем количество измерений
 
  next_pointer->next = *head; //  в новый указатель пишем адрес текущего начала списка
   next_pointer->measure = data; // копируем данные в новый элемент списка
  *head = next_pointer; // обновляем указатель на голову списка
  return SUCCESS;
}

  
datatype pop_data(list **head) {
  if (is_list_empty(*head)) {
    printf("List is empty, nothing to pop.\n");
    return (datatype){0}; // возвращаем пустую структуру
  }
  datatype data;
  list *temp = *head; // сохраняем указатель на первый элемент
  *head = (*head)->next; // обновляем голову списка
  data = temp->measure; // копируем данные из первого элемента
  free(temp); // освобождаем память первого элемента
  return data; // возвращаем данные первого элемента
}



int8_t read_full_data(list **dt, const char *csv_name) {
  int32_t year = 0, month = 0, day = 0, hour = 0, min = 0, temp = 0;
  uint32_t string_cntr =0; // счетчик строк для вывода информации о некорректных данных
  uint32_t errors[65535]={0};
  uint32_t error_cntr = 0;
  uint32_t n_scan = 0;
 // *dt = NULL; // указатель на начало списка
  FILE *rd = fopen(csv_name, "rb");
  if (rd == NULL) {
    // printf("\nWarning: read error or unknown option!\n\n");
    return ERROR;
  }
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
    push_data(dt, add); // добавляем данные в стек
    (*dt)->meas = string_cntr; // сохраняем количество измерений в списке
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

int8_t read_month_data(list **dt, const char *csv_name, uint8_t num_month, uint32_t *err) {
  int32_t year = 0, month = 0, day = 0, hour = 0, min = 0, temp = 0;
  uint32_t string_cntr = 0; // счетчик строк для вывода информации о некорректных данных
  uint32_t n_scan = 0;
  uint32_t error_cntr = 0;
  int8_t mem_alloc=0;
  *dt = NULL; // временная структура для хранения данных
  FILE *rd = fopen(csv_name, "rb");
  if (rd == NULL) {
    // printf("\nWarning: read error or unknown option!\n\n");
    return ERROR;
  }
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
     //(*dt)->meas = string_cntr;
      continue;
    }
    string_cntr++;
    if (month == num_month) { // если месяц совпадает с указанным
      push_data(dt, ((datatype) { year, month, day, hour, min,  temp})); // добавляем данные в стек
      mem_alloc=DATA_MONTH_AV;
      (*dt)->meas = string_cntr; // сохраняем количество измерений в списке
    }
    
  }
 
  fclose(rd);
  return mem_alloc;
}

int8_t get_max_temp(list *dt) {
  if (is_list_empty(dt)) {
    printf("List is empty, no max temperature.\n");
    return DATA_ERROR;
  }
  int8_t max_temp = dt->measure.temp;
  list *current = dt;
  while (current) {
    if (current->measure.temp > max_temp) {
      max_temp = current->measure.temp; 
    }
    current = current->next; // переходим к следующему элементу списка
  }
  return max_temp;
}

int8_t get_min_temp(list*dt){
  if(is_list_empty(dt)){
    printf("List is empty, no min temperature.\n");
    return DATA_ERROR;
  }
  int8_t min_temp = dt->measure.temp;
  list *current = dt;
  while(current){
    if(min_temp>current->measure.temp){
      min_temp = current->measure.temp; // обновляем минимальную температуру
    }
    current = current->next;
  }
  return min_temp;
}

float get_average_temp(list *dt) {
  if (is_list_empty(dt)) {
    printf("List is empty, no average temperature.\n");
    return DATA_ERROR;
  }
  float sum = 0;
  uint32_t count = 0;
  list *current = dt;
  while (current) {
    sum += current->measure.temp; // суммируем температуру
    count++; // увеличиваем счетчик
    current = current->next; // переходим к следующему элементу списка
  }
  return sum / count; // возвращаем среднюю температуру
}


// uint64_t encode_date(record *dt) {
//   return ((uint64_t)dt->year << 32) | ((uint32_t)dt->month << 24) |
//          ((uint32_t)dt->day << 16) | ((uint32_t)dt->hour << 8) | dt->min;
// }






void print_title(void) {
  printf("%-8s %-12s %-21s %-10s %13s\n", "year", "month", "average_temp",
"min_temp", "max_temp");
printf("-----   ----------    ------------         ----------      ----------\n");
}

void print_title_name(const char *title_name) {
  printf("\n====================%s====================\n\n", title_name);
}

uint8_t print_month_data(list *dt_dest, uint8_t num_month, float average){
   if (!dt_dest->meas) {
     printf("There is no valid data for %s\n",month_name[num_month - 1]);
    return ERROR;
   } else {
    int8_t max=get_max_temp( dt_dest);
    int8_t min=get_min_temp(dt_dest);
     printf("%-8d %-15s %0.1f%-16s%4d%-12s %4d%s\n", dt_dest->measure.year, month_name[num_month - 1],
           average, "°C", max,
           "°C", min,
           "°C");
  }
  return SUCCESS;
}


uint8_t print_month_info(const char *csv_name, uint8_t num_month) {
  list *m_data = NULL;
  uint32_t errors[65535] = {0};
  int8_t mem=read_month_data(&m_data, csv_name, num_month, errors);
   if (mem!=DATA_MONTH_AV) {
    printf("There is no valid data for %s\n", month_name[num_month - 1]);
    return ERROR;
  }
  float av = get_average_temp(m_data);
  print_title_name(stat_month);
  print_title();
  print_month_data(m_data, num_month, av);
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
  list *m_data = NULL;
  uint32_t errors[65535] = {0};
  int32_t mem=0;
  print_title_name(stat_month_by_year);
  print_title();
  for (uint8_t j = 1; j <= 12; j++) {
    mem=read_month_data(&m_data, csv_name, j, errors);
    if (mem!=DATA_MONTH_AV) {
      printf("There is no valid data for %s\n", month_name[j - 1]);
    continue;
    }
    float av = get_average_temp(m_data);
    if (m_data->meas) {
      valid_months++;
      year_min = year_min > m_data->measure.temp ? m_data->measure.temp : year_min;
      year_max = year_max < m_data->measure.temp ? m_data->measure.temp : year_max;
      year_avg += av;
    }
    print_month_data(m_data, j, av);
  }
  printf("\n");
  print_title_name(stat_year);
  print_year_data(year_avg, year_max, year_min, valid_months);
}


