//На вход программе подается беззнаковое 32-битное целое число N и натуральное число K (1 ≤ K ≤ 31).
// Требуется циклически сдвинуть битычисла N вправо на K битов и вывести полученное таким образом число. 

#include "stdio.h"
#include "stdint.h"
#include <stdlib.h>

#define ERROR                       1
#define SUCCESS                     0

typedef struct{
    uint32_t input_value;
    uint8_t shift;
} params_t;

params_t *parameters;


uint32_t rotate(params_t *params){
    if(params->shift<1||params->shift>31){
        printf("Shift is out of range");
        return ERROR;
    }
    return (params->input_value >> params->shift)|(params->input_value << (32 - params->shift));
}

int main(void){
    parameters = malloc(sizeof(params_t));
   scanf("%u",&parameters->input_value);
   scanf("%u",&parameters->shift);
    printf("%u\n",rotate(parameters));
   
    return 0;
}