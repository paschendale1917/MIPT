//На вход программе подается беззнаковое 32-битное целое число N. Требуется найти количество единичных битов в двоичном представлении данного числа.

#include "stdio.h"
#include "stdint.h"
#include <stdlib.h>

#define ERROR                       1
#define SUCCESS                     0
#define BITS_IN_UINT32              32

uint32_t input_value=0;

uint32_t get_num_bits(uint32_t value){
    uint8_t bit_cntr=0;
    uint32_t is_bit_set=0;
    uint32_t mask=0x01;
    for(uint8_t i=0;i<BITS_IN_UINT32;i++){
        is_bit_set=value&(mask<<i);
       if(is_bit_set) bit_cntr++;
        
    }
    return bit_cntr;
}

int main(void){
    scanf("%u",&input_value);
    printf("%u\n",get_num_bits(input_value));
    return SUCCESS;
}