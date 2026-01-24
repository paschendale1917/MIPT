//На стандартном потоке ввода задается целое неотрицательное число N и последовательность допустимых символов в кодировке ASCII, оканчивающаяся точкой. 
//Допустимые символы - латинские буквы 'a' ... 'z', 'A' ... 'Z' и пробел. Требуется закодировать латинские буквы ('a' ... 'z', 'A' ... 'Z') шифром Цезаря,
//пробелы вывести без изменения. Число N задает сдвиг в шифре. Шифр Цезаря заключается в следующем преобразовании. 
//Пусть буквы алфавита пронумерованы от 0 до K - 1, где K - число символов в алфавите. Тогда символ с номером n кодируется символом с номером p = (n + N) mod K (mod - операция взятия остатка).
//На стандартном потоке вывода напечатать зашифрованное сообщение, оканчивающееся точкой. Преобразование требуется выполнять независимо для заглавных и строчных латинских букв.
//Указание: использовать массивы запрещается 

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ALF_NUM_LETTERS             26
#define LOWERCASE                   0
#define UPPERCASE                   1
#define ERROR                       1
#define SUCCESS                     0

typedef enum { UP, DOWN } InsertPosition;

typedef struct list{
  char letter;                                                              //символ
  struct list *next;                                                        //указатель на следующий элемент
  struct list *prev;                                                        //указатель на предыдущий элемент
}list;

list *orig_head=NULL;

uint32_t caesar_shift=0;                                                    //сдвиг Цезаря
char current_char=0;
char modded_char=0;
uint32_t num_chars=0;

char caesar_encoder(uint32_t shift, char letter);
uint8_t push_data(list **head, char data);
void reverse_list(list **head);
char pop_data(list **head);

int main(void) {
    scanf("%u", &caesar_shift);                                             // считал сдвиг
    while(current_char!='.'){
        scanf("%c", &current_char);
        push_data(&orig_head,current_char);
        num_chars++;
    }
    reverse_list(&orig_head);
    for(uint32_t i=0;i<num_chars;i++){
        current_char=pop_data(&orig_head);
        modded_char=caesar_encoder(caesar_shift,current_char);
        printf("%c", modded_char); 
    }

    return 0;
}

uint8_t is_uppercase(char letter) {
    return (letter>= 'A' && letter <= 'Z');
}

uint8_t is_lowercase(char letter) {
    return (letter>= 'a' && letter <= 'z');
}

char caesar_encoder(uint32_t shift, char letter){
    char local_letter=0;
    if(letter!=' '&&letter!='.'){
        uint8_t letter_state=(is_uppercase(letter));                        //проверка,входит ли в диапазон заглавных букв алфавита введенный символ
        if(!letter_state){                                
            if(!(letter_state=is_lowercase(letter)))                        //если не входит, тогда проверяем, входит ли в диапазон строчных букв
        return ERROR;                                                       //если не входит и в этот диапазон, тогда вываливаемся с ошибкой
        }
        letter_state=is_uppercase(letter);                                  //если все проверки пройдены, тогда опредeляем,в какой диапазон входит символ: в диапазон заглавных или строчных букв
        switch(letter_state){
            case UPPERCASE:
                local_letter='A'+(letter-'A'+shift)%ALF_NUM_LETTERS;
                break;
            case LOWERCASE:
                local_letter='a'+(letter-'a'+shift)%ALF_NUM_LETTERS;
                break;
        }
     return local_letter;
    } else if(letter=='.')return '.';
    else return ' ';
}

uint8_t is_list_empty(list *head) {
  return (head == NULL);                                                    // возвращаем 1, если список пуст, иначе 0
}

uint8_t push_data(list **head, char data) {
  list *next_pointer = (list *)malloc(sizeof(list));
  if(next_pointer == NULL) {                                                //если malloc вернул NULL, значит память не выделена
    printf("Memory allocation failed for new list.\n");
    return ERROR;
  }
    next_pointer->letter=data;                                              //занесение символа в next_pointer
    next_pointer->next=*head;                                               //head уехал на позицию вперед, поэтому для next_pointer это будет следующий элемент
    next_pointer->prev=NULL;                                                //при этом предыдущего элемента у next_pointer нет, посему пишем NULL
    if(!is_list_empty(*head)) {                                             //если список не пустой
        (*head)->prev=next_pointer;                                         //head уехал на позицию вперед, поэтому для него next_pointer это предыдущий элемент
    }
    if(next_pointer->prev==NULL&&next_pointer->next==NULL&&data==' '){      //если добавляемый символ пробел и это первый элемент списка, то уменьшаем счетчик символов и выходим из функции
        num_chars-=1;
        return ERROR;               
    }
    *head = next_pointer;                                                   //после делаем началом списка next_pointer
  return SUCCESS;
}

void reverse_list(list **head) {
    list *prev = NULL;
    list *current = *head;
    list *next = NULL;
    
    while(current != NULL) {
        next = current->next;
        current->next = prev;
        current->prev = next;  
        prev = current;
        current = next;
    }
    
    *head = prev;
}

 char pop_data(list **head) {
    if (is_list_empty(*head)) {
      printf("List is empty, nothing to pop.\n");
      return (char){0};                                                     // возвращаем нулевой символ, если список пуст
    }
    char data;
    list *temp = *head;               
    data = temp->letter;                                                    // копируем данные из первого элемента
    *head = temp->next;                                                     // сдвигаем голову списка на следующий элемент
    if (*head != NULL) {
        (*head)->prev = NULL;                                               // если список не пуст, обновляем указатель prev нового головного элемента
    }
    free(temp);                                                             // освобождаем память первого элемента
    return data;
 }




// char pop_data(list **head) {
//   if (is_list_empty(*head)) {
//     printf("List is empty, nothing to pop.\n");
//     return (char){0};                                                    // возвращаем нулевой символ, если список пуст
//   }
//   char data;
//   list *temp = *head;               
//   while(temp->next!=NULL){                                               //идем по адресам элементов списка, пока не дойдем до последнего(он равен NULL)
//     temp=temp->next;                                                     //после всех этих дел в temp будет указатель на последний элемент списка
//   }                                                                      // сохраняем указатель на первый элемент
//   data = temp->letter;                                                   // копируем данные из первого элемента
//     if (temp->prev == NULL) {                                            // eсли это был единственный элемент в списке                 
//         *head = NULL;                                                    // то адрес списка обнуляем
//     } else {                                                             //иначе делаем предпоследний элемент новым последним 
//         temp->prev->next = NULL;
//     }
//   free(temp);                                                              // освобождаем память первого элемента
//   return data;                                                             // возвращаем данные первого элемента
// }