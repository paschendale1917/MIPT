//На вход программе подается беззнаковое 32-битное целое число N и натуральное число K (1 ≤ K ≤ 31).
// Требуется взять K младших битов числа N и вывести полученное таким образом число.

#include "stdio.h"
#include "stdint.h"
#include <stdlib.h>

#define ERROR                       1
#define SUCCESS                     0

typedef struct{
    uint32_t input_value;
    uint8_t num_lsbits;
} params_t;

params_t *parameters;


uint32_t get_lsbits(params_t *params){
    if(params->num_lsbits<1||params->num_lsbits>31){
        printf("Num_lsbits is out of range");
        return ERROR;
    }
    uint32_t mask=0xFFFFFFFF;
    mask&=~(mask>>(params->num_lsbits)<<params->num_lsbits);
    return params->input_value&mask;
}

int main(void){
    parameters = malloc(sizeof(params_t));

   scanf("%u",&parameters->input_value);
   scanf("%u",&parameters->num_lsbits);
    printf("%u\n",get_lsbits(parameters));
   
    return 0;
}