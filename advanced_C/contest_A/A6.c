//На вход программе подается беззнаковое 32-битное целое число N. 
//Требуется изменить значения всех битов старшего байта числа на противоположные и вывести полученное таким образом число.

#include "stdio.h"
#include "stdint.h"

#define ERROR                       1
#define SUCCESS                     0
#define BITS_IN_UINT32              32

uint32_t input_value=0;

uint32_t change_msbits(uint32_t value){
    uint32_t mask=0xFF000000;
    return value^(mask);    
}

int main(void){
    scanf("%u",&input_value);
    printf("%u\n",change_msbits(input_value));
    return SUCCESS;
}