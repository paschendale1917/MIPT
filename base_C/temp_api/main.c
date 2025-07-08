#include "temp_api.h"
#include <stdint.h>
#include <stdio.h>

char *version = "v1.3";
char filename[] = "";
uint8_t u = 0;

uint8_t get_filename(char *sourse, char *dest_name) {
  char *pp = sourse;
  uint8_t t = 0;
  while (*pp != '\000' /*  && *pp != ' ' */) {
    *(dest_name + (t++)) = *(pp++);
  };
  return t;
}

void print_help(void) {
  printf("Supported arguments:\n");
  printf("-h                                 -- help\n");
  printf("-f <filename.csv>                  -- .csv file to load. show statistics for all months by the year\n");
  printf("-f <filename.csv> -m <month>       -- show statistics for target <month> \n");
  printf("-v                                 -- version\n");
}

void print_about(void) {
  printf("\n======================================================\n");
  printf("\nThis is a simple temperature statistic program (%s).\n", version);
  printf("To get help use the key -h.\n");
  printf("\n======================================================\n\n");
}

void args(int32_t argc, char *argv[]) {
  for (uint8_t i = 0; i < argc; i++) {
    char *p = argv[i];
    int8_t res=0;
    uint8_t f_name=0;
    if (*p == '-') {
      p++;
      switch (*p) {
      case 'h':
        print_help();
        break;
      case 'v':
        printf("%s\n", version);
        break;
      case 'm':
        p += 2;
        if (res!=ERROR) {
          (char2num(p, '\000')>=1&&char2num(p, '\000')<=12)?read_data(&full_data, filename):*p;//читаем данные только если введен корректный номер месяца
          print_month_info(&full_data, &m_data, char2num(p, '\000'));
        }
        break;
      case 'f':
        p += 2;
        f_name = get_filename(p, filename);          // считал наименование входного файла и подсчитал кол-во символов
        if (/* res!=ERROR&& */*(p + f_name + 1) != '-') {    
         res=read_data(&full_data, filename);                // если после наименования файла нет '-', то вывожу данные за год
          print_yearstat_info(&full_data,&m_data);     // если есть, то происходит выход из свитча по брейку
        }                                                                 // и функция прыгает  в if
        break;
      default:
        printf("Unknown option: %s\n", argv[i]);
        break;
      }
    } else if (argc < 2) {
      print_about();
    }
  }
}

int main(int32_t argc, char *argv[]) {
  args(argc, argv);
  return 0;
}