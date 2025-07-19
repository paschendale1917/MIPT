#include "temp_api.h"
#include <stdint.h>
#include <stdio.h>

#define FILENAME_LENGTH 256

const char *version = "v1.5.1";
char filename[FILENAME_LENGTH] = {0};
uint32_t er[200]={0}; // массив для хранения ошибок чтения данных

uint8_t get_filename(const char *source, char *dest_name) {
  uint16_t t = 0;
  while (*source && *source != ' ' && t < FILENAME_LENGTH - 1) {
    dest_name[t++] = *source++;
  }
  dest_name[t] = '\0';
  return t;
}

void print_help(void) {
  printf("Supported arguments:\n");
  printf("-h                                 -- help\n");
  printf("-f <filename.csv>                  -- .csv file to load. it will show statistics for all months by the year\n");
  printf("-f <filename.csv> -m<month>        -- show statistics for target <month> \n");
  printf("-v                                 -- version\n");
}

void print_about(void) {
  printf("\n======================================================\n");
  printf("\nThis is a simple temperature statistic program (%s).\n", version);
  printf("To get help use the key -h.\n");
  printf("\n======================================================\n\n");
}

void args(int32_t argc, char *argv[]) {
  if (argc < 2) {
    print_about();
    return;
  }
  int8_t res =5;
  uint8_t month = 0;
  for (uint8_t i = 1; i < argc; i++) {
    char *p = argv[i];
    if (*p == '-') {
      p++;
      switch (*p) {
      case 'h':
        print_help();
        break;
      case 'v':
        printf("%s\n", version);
        break;
      case 'f': {
        p+=2;
        while (*p == ' ') p++; // пропуск пробелов
        if (*p) {
          get_filename(p, filename);
          res = read_full_data(&full_data_head, filename);
          if (res==ERROR) {
            printf("Error reading file.\n");
            return;
          }
        } else {
          printf("No file name specified after -f\n");
          return;
        }
        break;
      }
      case 'm': {
        p++;
        month = (uint8_t)char2num(p, '\0');
        if (month < 1 || month > 12) {
          printf("Invalid month number: %d\n\n", month);
          return;
        }
        if (res==5) {
          printf("\nPlease specify a file using -f <filename.csv>\n\n");
          return;
        }
        print_month_info(filename,month);
        break;
      }
      default:
        printf("Unknown option: %s\n", argv[i]);
        break;
      }
    }
  }
  // Если был только -f, выводим статистику по году
  if (res==SUCCESS && !month) {
    print_yearstat_info(filename);
  }
}

int main(int32_t argc, char *argv[]) {
  args(argc, argv);
  return 0;
}