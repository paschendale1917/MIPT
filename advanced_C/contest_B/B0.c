//описана структура
//Требуется реализовать только одну функцию, которая в данном списке находит адрес блока памяти занимающий больше всего места.
//Адрес хранится в поле address, поле size - соответствующий размер данного блока. Если список пустой, то функция должна возвращать 0. Если есть несколько таких блоков, то вернуть адрес любого из них.
//Прототип функции: uint64_t findMaxBlock(list *head)
 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ERROR                       1
#define SUCCESS                     0

typedef struct list {
    uint64_t address;
    size_t size;
    char comment[64];
    struct list *next;
} list;

uint32_t sequence_size=0;
list *head=NULL;

 uint64_t findMaxBlock(list *head);

int main(void) {

   scanf("%u", &sequence_size);                        
    for(uint32_t i=sequence_size;i>0;i--){
        list *next_list=malloc(sizeof(list));
        if(next_list == NULL) {                                                
            printf("Memory allocation failed for new list.\n");
            return ERROR;
        }
        scanf("%llu",&next_list->address);
        scanf("%llu",&next_list->size);
        next_list->next=head;
        head=next_list;
    }
    printf("%llu", findMaxBlock(head));
    return 0;
}

 uint64_t findMaxBlock(list *head){
    if(head == NULL) return 0;
    size_t local_size=0;
    uint64_t local_address=0;
    list *temp = NULL;                                      //переписал адрес первого элемента списка во временную переменную для того, чтобы в дальнейшем освободить этот участок памяти
    while(head!=NULL){ 
        temp = head;                                   
        if(head->size>local_size){               //внес в поля струтуры с ответом интересующие поля первого элемента исследуемого списка
            local_size=head->size;
           local_address=head->address;
        }
        head=temp->next;                                   //после сохранения инрересующих данных в указатель записываю адрес следующего элемента списка                                 
        //free(temp);                                      //предыдущий элемент мне уже не интересен, освобождаю память
    }
    return local_address;
 }