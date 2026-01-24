//На стандартном потоке ввода задаётся натуральное число N (N > 0), после которого следует последовательность из N целых чисел.
//На стандартный поток вывода напечатайте, сколько раз в этой последовательности встречается максимум. Указание: использовать массивы запрещается. 

#include <stdio.h>
#include <stdint.h>

uint32_t sequence_size=0;
int32_t current_num=0;
int32_t maximum=0;
volatile uint32_t cntr=1;

int main(void) {

   scanf("%u", &sequence_size);                         // считал количество обрабатывамых чисел
   scanf("%d", &current_num);                           //считал первое число
   maximum=current_num;                                 //перво считанное число теперь максимум
   for(uint32_t i = 0; i < sequence_size-1; i++) {      
    scanf("%d", &current_num);
    if(current_num>maximum){                           
        maximum=current_num;
        cntr=1;
    }else
     if(current_num==maximum)cntr++;
    }
     printf("%u", cntr);
    return 0;
}