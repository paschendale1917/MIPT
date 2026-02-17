#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define BUFFERSIZE      20

struct list {
char word[BUFFERSIZE];
struct list *next;
};

struct list *start_item_pointer=NULL;
char buffer[20];
uint8_t word_counter=0;
uint8_t letter_counter=0;

void get_new_list_item(struct list **head, char *buffer, uint8_t buffersize);
void clear_buffer(char *buffer, uint8_t buffersize);
void printListIteration(struct list *item);
void organize_struct(struct list *list_head);

int main(void) {
    char c;
    while ((c = getchar()) != '\n') {
        switch(c){
            case ' ':
                get_new_list_item(&start_item_pointer,buffer,sizeof(buffer));
                letter_counter=0;
                clear_buffer(buffer, sizeof(buffer));
                break;
            case '.':
                get_new_list_item(&start_item_pointer,buffer,sizeof(buffer));
                //start_item_pointer->next=NULL;
                break;
            default:
                buffer[letter_counter++]=c;
                break;
        }
}  
    organize_struct(start_item_pointer);
    printListIteration(start_item_pointer);
    return 0; 
}

void get_new_list_item(struct list **head, char *buffer, uint8_t buffersize){
    struct list *new_item=calloc(1,sizeof(struct list));
    strcpy(new_item->word, buffer); 
     if(*head==NULL){
        *head=new_item;
     }else{
        struct list *p=*head;
        while(p->next!=NULL){
            p=p->next;
        }
        p->next=new_item;
    }
}


void clear_buffer(char *buffer, uint8_t buffersize){
    for(uint8_t i=0;i<buffersize;i++){
        buffer[i]=0x00;
    }
}

void printListIteration(struct list *item){
    while(item){
        struct list *local=NULL;
        printf("%s ",item->word);
        local=item;
        item = item->next;
        free(local);
    }
    printf("\n");
}

void change_buf(char *buf1, char *buf2, uint8_t buffersize){
    char buf[buffersize];
    strcpy(buf,buf1); 
    strcpy(buf1, buf2);
    strcpy(buf2, buf);
}

void organize_struct(struct list *list_head){
    struct list *local;
    uint8_t  swapped=0;
    struct list *last = NULL;
    do{
        local=list_head;                                                                            //в локальный указатель адрес головы списка
        swapped=0;                                                                                  //сброс флажка
        while(local->next!=last){                                                                   //идем до last==NULL                            
            if(strcmp(local->word,local->next->word)>0){
                change_buf(local->word,local->next->word,BUFFERSIZE);                               //флажок подниматся только тогда , когда зайдем в эту часть функции для перемещения
                swapped=1;
            }
        local=local->next;
        }                                                                                           //после того, как в первый раз прошли до NULL(то бишь до элемента списка с полем next,указывающим на NULL)
    last=local;                                                                                     //то пишем указатель на данный элемент списка и возвращаемся вверх, записывая в локал адрес головы списка и пройдем мы уже не до нулл во второй раз, а до предпоследнего
    }
    while(swapped);

}