//Преподаватели Geek Brains создали набор, содержащий N (1 ≤ N ≤ 1 000 000) натуральных чисел, не превосходящих 1 000 000.
//Известно, что ровно одно число в этом наборе встречается один раз, а остальные — ровно по два раза. Помогите им найти это число. 
//Входныеданные: в первой входной строке вводится число N, затем по одному числу в строке вводятся N натуральных чисел, не превосходящих 2000 000 000. 
//Выходные данные: ваша программа должна определить число, встречающееся один раз, и вывести его на экран. Указание: использовать массивы запрещается.

#include "stdio.h"
#include "stdint.h"
#include <stdlib.h>

#define ERROR                       1
#define SUCCESS                     0

uint32_t num_input_value=0;
int64_t input_value=0;
int64_t current_value=0;
uint8_t value_cntr=1;

int find_odd_element(int32_t arr[], size_t n) {
int32_t res = 0;
for (size_t i = 0; i < n; i++)
res ^= arr[i];
return res;
}

int main(void){
    scanf("%u",&num_input_value);
    scanf("%lld",&current_value);
    for(uint32_t i=0;i<(num_input_value-1);i++){
        scanf("%lld",&input_value);
        current_value^=input_value;
    }

    printf("%llu",current_value);
    return SUCCESS;
}