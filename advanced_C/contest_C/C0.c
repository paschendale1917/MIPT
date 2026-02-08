//Сколько различных трехзначных чисел можно получить из заданного натурального N, вычеркивая цифры из его десятичной записи? 

//в последней версии вместо поиска уникальных чисел в огромном массиве трехзначных чисел сделана проверка по битовой маске сразу после изъятия этого числа из входного массива

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define ERROR_SUBJ                                              255
#define ERROR_STR                                               254
#define ERROR_ARR                                               253
#define ERROR_RES                                               252
#define BINOM_K                                                 3               //в задании трехзначное число
#define MAX_INPUT                                               512             //программа может принять не более MAX_INPUT символов 
#define MAX_OUTPUT                                              900             //максимальное количество трехзначных чисел

typedef struct {
    uint8_t     arr_str_lenth;                                                  //длина исследуемой строки
    char        *array_string;                                                  //указатель на входную строку
    uint8_t     *input_arr;                                                     //указатель на  входной массив(после его получения путем преобразования входной строки)
    uint32_t    *research_arr;                                                  //указатель на исследуемый массив с  числами заданной длины(конкретно здесь трехзначными)
}array_info_t;

array_info_t *subj_array;                                                       //исследуемый массив
uint8_t combination_num=0;
uint32_t current_num=0;

uint8_t string2array(array_info_t *dt);                                         //строку в исследуемый массив
uint32_t factorial(uint8_t number);
uint32_t all_iterations(array_info_t *dt);
uint32_t get_num_diff_items(uint32_t *arr, uint32_t arr_size);


int main(void) {
    subj_array=malloc(sizeof(array_info_t));                                    //выделение памяти под структуру
    if(subj_array==NULL)return ERROR_SUBJ;

    subj_array->array_string=malloc(MAX_INPUT*sizeof(char));                    //выделение памяти под строку
    if(subj_array->array_string==NULL)return ERROR_STR;
    scanf("%s",subj_array->array_string);                                       //снюхал входные данные в строку
    subj_array->arr_str_lenth=strlen(subj_array->array_string);                 //получение количества считанных символов
    subj_array->arr_str_lenth>MAX_INPUT?subj_array->arr_str_lenth=MAX_INPUT:0;  //на всякий проверяем, не получилась ли строка больше, чем максимальное входное количество симовлов
    subj_array->input_arr=malloc(subj_array->arr_str_lenth*sizeof(uint8_t));    //выделение памяти под входной массив
    if(subj_array->input_arr==NULL)return ERROR_ARR;
    string2array(subj_array);                                                   //строку переделал в массив
    uint32_t total_numbers=all_iterations(subj_array);
    //uint8_t diff=get_num_diff_items(subj_array->research_arr,total_numbers);
    printf("%u",total_numbers);
    free(subj_array->input_arr);
    free(subj_array->array_string);
    free(subj_array->research_arr);
    free(subj_array);

    return 0; 
}

uint32_t binomial_coefficient(uint8_t n, uint8_t k) {
    if (k > n) return 0;
    uint32_t result=1;
    for (uint8_t i=1; i<=k; i++) {
        result=result*(n-k+i)/i;
    }
    return result;
}

uint8_t string2array(array_info_t *dt){
    for(uint8_t i=0;i<dt->arr_str_lenth;i++){
        *(dt->input_arr+i)=*(dt->array_string+i)-0x30;
    }
    return 0;
}

uint8_t bitmask_check(uint16_t num, uint8_t bitmask_arr[]){                    //битовая маска, где каждый бит каждого из элементов массива будет обозначать число(1000бит/8бит=125 байт)
    uint16_t byte_index=num/8;                                                 //номер байта в массиве,в котором будет храниться бит исследуемого числа
    uint8_t bit_index=num%8;                                                    
    if(!(bitmask_arr[byte_index]&(1<<bit_index))){
        bitmask_arr[byte_index]|=1<<bit_index;
        return 1;
    }else
    return 0;
}

uint32_t all_iterations(array_info_t *dt) {
    uint32_t total_count=0;
    uint32_t current_len=dt->arr_str_lenth;
    uint32_t max_total=0;
    uint8_t bitmask[125]={0,};                                                  //битовая маска, где каждый бит каждого из элементов массива будет обозначать число(1000бит/8бит=125 байт)
    for(uint32_t len=current_len; len >= 3; len--) {                            // подсчет максимального количества комбинаций(для выделения соответствующего количества памяти)
        max_total+=binomial_coefficient(len,3);
    }
    dt->research_arr=malloc(max_total*sizeof(uint32_t));                        //выделил память для всех комбинаций //тут можно подзапариться и сделать динамическое выделение памяти каждый раз как появляется уникальное трехзначное число
    if(dt->research_arr == NULL) return 0;
    while(current_len >= 3) {                                                   // пока длина остатка входной строки больше трех
        for(uint32_t i=0; i<current_len-2; i++) {                               // произвожу сборку числа для текущей длины остатка входной строки(i,j,k для сотен, десятков и единиц соответственно)
            if(*(dt->input_arr+i)==0) continue;                                
            for(uint32_t j=i+1; j<current_len-1; j++) {                 
                for(uint32_t k=j+1; k<current_len; k++) {
                    uint16_t local_res=*(dt->input_arr+i)*100 + 
                        *(dt->input_arr+j)*10 + 
                        *(dt->input_arr+k);
                    if(bitmask_check(local_res,bitmask)){
                         dt->research_arr[total_count]=local_res;
                         total_count++;                                         //подсчет количества уникальных трехзначных элементов 
                    }
                                                                  
                }
            }
        }
        current_len--;                                                          //вычеркнул один элемент из входного массива
    }
    return total_count;
}

uint32_t get_num_diff_items(uint32_t *arr, uint32_t arr_size){
    uint32_t different_count = arr_size;
    for(uint32_t i=0;i<arr_size;i++){
        for(uint32_t x=i+1;x<arr_size;x++){
            if (*(arr+i)==*(arr+x)) {
                different_count--; 
                break; 
            }
        }
    }
    return different_count;
}