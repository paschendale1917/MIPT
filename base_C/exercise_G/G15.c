// В файле input.txt дано предложение. Необходимо заменить все имена «Cao» на
// «Ling» и записать результат в файл output.txt.

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_CHAR 2000

FILE *f_in, *f_out;
char *file_in = "input.txt";
char *file_out = "output.txt";

char output[MAX_CHAR] = "";
char string[MAX_CHAR] = "";


uint16_t cao_counter(char *str);
void exchange(char *str_in, char *str_out);

int main(void) {
  f_in = fopen(file_in, "r");
  f_out = fopen(file_out, "w");
  fscanf(f_in, "%[^\n]s", string);
  exchange(string, output);
  fprintf(f_out, "%s", output);
  fclose(f_in);
  fclose(f_out);
  return 0;
}

// uint16_t cao_counter(char *str) {
//   uint16_t cntr = 0;
//   uint16_t cao_cntr = 0;
//   while (*(str + cntr) != '\000') {
//     if (*(str + cntr) == 'C') {
//       if (*(str + cntr + 1) == 'a') {
//         *(str + cntr + 2) == 'o' ? cao_cntr++ : cao_cntr;
//       }
//     }
//     cntr++;
//   }
//   return cao_cntr;
// }

uint8_t payload_finder(char *str) {
  uint16_t cao = 0;
  if (*str == 'C') {
    if (*(str + 1) == 'a') {
      *(str + 2) == 'o' ? cao++ : cao;
    }
  }
  return cao ? 1 : 0;
}

void exchange(char *str_in, char *str_out) {
  uint16_t char_in = 0;
  uint16_t char_out = 0;
  char *payload_out = "Ling";
  while (*(str_in + char_in) != '\000') {
    if (payload_finder(str_in + char_in)) {
      for (uint8_t u = 0; u < 4; u++) {
        *(str_out + u + char_out) = *(payload_out + u);
      }
      char_out += 4;
      char_in += 3;
    } else {
      *(str_out + char_out) = *(str_in + char_in);
      char_in++;
      char_out++;
    }
  }
}
