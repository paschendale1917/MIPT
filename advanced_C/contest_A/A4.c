
//На вход программе подается беззнаковое 32-битное целое число N и натуральное число K (1 ≤ K ≤ 31).
// Требуется взять K подряд идущих битов числа N так, чтобы полученное число было максимальным. Программа должна вывести полученное число.

#include "stdio.h"
#include "stdint.h"
#include <stdlib.h>

#define ERROR                       1
#define SUCCESS                     0
#define BITS_IN_UINT32              32


typedef struct{
    uint32_t input_value;
    uint8_t bits_to_check;
} params_t;

params_t *parameters;

uint32_t get_some_bits(params_t *params){
    uint32_t max_value=0;
    uint32_t current_value=0;
    uint32_t mask=0xffffffff;
    mask=(mask<<(BITS_IN_UINT32-params->bits_to_check))>>(BITS_IN_UINT32-params->bits_to_check); 
    for(uint8_t i=0;i<=BITS_IN_UINT32-params->bits_to_check;i++){
        current_value=(params->input_value&(mask<<i))>>i;
        current_value>max_value?max_value=current_value:max_value;
    }
    return max_value;
}

int main(void){
    parameters = malloc(sizeof(params_t));

   scanf("%u",&parameters->input_value);
   scanf("%u",&parameters->bits_to_check);
    printf("%u\n",get_some_bits(parameters));
    return SUCCESS;
}