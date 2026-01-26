//Описана структура данных для хранения информации об использованной памяти:
//typedef struct list {
//void *address;
//size_t size;
//char comment[64];
//struct list *next;
//} list;
//Требуется реализовать только одну функцию, которая анализирует данный список и возвращает сколько всего памяти используется. 
//Адрес хранится в поле address, поле size - соответствующий размер данного блока. Если список пустой, то функция должна возвращать 0.
//Прототип функции: size_t totalMemoryUsage(list *head)

 
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

 size_t totalMemoryUsage(list *head);

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
    printf("%llu", totalMemoryUsage(head));
    return SUCCESS;
}

 size_t totalMemoryUsage(list *head){
    if(head == NULL) return 0;
    size_t total_memory_size=0;
    list *temp = NULL;                                      
    while(head!=NULL){ 
        temp = head; 
        total_memory_size+=head->size;                                  
        head=temp->next;                                                                
    }
    return total_memory_size;
 }