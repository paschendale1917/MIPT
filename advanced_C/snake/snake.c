#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses/ncurses.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#define CONTROLS                                        2
#define MIN_Y                                           5                                                                           
#define MIN_X                                           5 

enum {LEFT=1, UP, RIGHT, DOWN, EXIT_GAME=KEY_F(10),END_GAME};
enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=2, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10, AMOUNT_FOOD_SPAWN=5};

// структура направлений движения
struct control_buttons {
    uint32_t down;
    uint32_t up;
    uint32_t left;
    uint32_t right;
};

// массив структур с кнопками управления
struct control_buttons default_controls[CONTROLS] = {
    {KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT},    
    {'s', 'w', 'a', 'd'}                        
};

//структура с координатами элемента хвоста
typedef struct tail_t {
    uint32_t x;
    uint32_t y;
} tail_t;

//структура для хранения размера окна терминала
 typedef struct {
    uint32_t x;
    uint32_t y;
}window_size_t;

// струтура , описывающая змею
typedef struct snake_t {
    uint32_t x;                                                                                             // координата X головы 
    uint32_t y;                                                                                             // координата Y головы 
    uint32_t direction;                                                                                     // направление движения
    size_t tsize;                                                                                           // размер хвоста
    struct tail_t *tail;                                                                                    // указатель на список хврста
    struct control_buttons controls;                                                                        // массив кнокпок для управления
} snake_t;

//структура еды
typedef struct food_t {
    uint32_t x;
    uint32_t y;
    char symbol;
    time_t enable_time;
    uint8_t enable_status;
} food_t;

food_t food[MAX_FOOD_SIZE];

void getWindowSize(window_size_t *size){
    getmaxyx(stdscr, size->y, size->x);
}

// инит хвоста
void initTail(tail_t t[], size_t size) {
    tail_t init_t={0,0};
    for(size_t i=0; i<size; i++) t[i] = init_t;
}

// инит змеиной башки
void initHead(snake_t *head, int32_t x, int32_t y) {
    if(x<MIN_X)x=MIN_X;
    if(y<MIN_Y)y=MIN_Y;
    head->x=x;
    head->y=y;
    head->direction = RIGHT;                                                                
}

// инит змеи
void initSnake(snake_t *head, size_t size, uint32_t x, uint32_t y) {
    tail_t *tail = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head, x, y);
    head->tail = tail;
    head->tsize = size+1;                                                                   
    head->controls = *default_controls;
}



void go(struct snake_t *head){
    char ch='%';
    window_size_t *window_size=((window_size_t*)malloc(sizeof(window_size_t)));
    getWindowSize(window_size);                                                                                                 //получаю размер окна
    mvprintw(head->y,head->x," ");                                                                                              //очистил предыдущее положение головы
    switch (head->direction){                                                                                                   // в свитче получаю новое положение головы
        case LEFT:
            head->x=(head->x<=MIN_X)?window_size->x-MIN_X:head->x-1;                 
            break;
        case RIGHT:
            head->x=(head->x>=window_size->x-MIN_X)?MIN_X:head->x+1;
            break;
        case UP:
            head->y=(head->y<=MIN_Y)?window_size->y-MIN_Y:head->y-1;
            break;
        case DOWN:
            head->y=(head->y>=window_size->y-MIN_Y)?MIN_Y:head->y+1;
            break;
        default:
            break;
    }
    mvprintw(head->y,head->x,"%c",ch);
    refresh();
}

void goTail(struct snake_t *head){
    char ch = '*';
    mvprintw(head->tail[head->tsize-1].y,head->tail[head->tsize-1].x," ");
    for(size_t i = head->tsize-1; i>0; i--){
        head->tail[i] = head->tail[i-1];
        if( head->tail[i].y || head->tail[i].x)
            mvprintw(head->tail[i].y, head->tail[i].x, "%c", ch);
    }
    head->tail[0].x = head->x;
    head->tail[0].y = head->y;
}

void reverseDirection(snake_t *snake, uint32_t direction){
    switch (direction){
    case LEFT:                                                                                                          //если направление движения влево
        snake->direction=DOWN;                                                                                          //разворот буквой П: сначала поворачиваю вниз(можно и вверх при желании)
        go(snake);                                                                                                      //двигаю  голову на 1 позицию
        goTail(snake);                                                                                                  //двигаю хвост на 1 позицию
        snake->direction=RIGHT;                                                                                         //далее меняю направление движа вправо                                                                  
        break;
    case RIGHT:
        snake->direction=UP;
        go(snake);
        goTail(snake);
        snake->direction=LEFT;
        break;
    case UP:
        snake->direction=RIGHT;
        go(snake);
        goTail(snake);
        snake->direction=DOWN;
        break;
    case DOWN:
        snake->direction=LEFT;
        go(snake);
        goTail(snake);
        snake->direction=UP;
        break;
    
    default:
        break;
    }
}

uint8_t isOppositeDirection(uint32_t current_dir, uint32_t new_dir) {
    return (current_dir == LEFT && new_dir == RIGHT) ||                                                             //сравниваю текущее направление движения и то, которое хочется задать
           (current_dir == RIGHT && new_dir == LEFT) ||                                                             //если они противоположны, тогда функция вернет 1
           (current_dir == UP && new_dir == DOWN) ||
           (current_dir == DOWN && new_dir == UP);
}

void changeDirection(snake_t* snake, const int32_t key){
    uint32_t current_direction = snake->direction;                                                                  //запомнил, в каком направлении двигается змея
    for(uint8_t j=0;j<CONTROLS;j++){                                                                                //проверил, какой набор клавиш для управления используется
        snake->controls=default_controls[j];                                                                        //в указатель на набор клавиш записал адрес набора клавиш, который будет юзаться
        if (key == snake->controls.down)
            snake->direction = DOWN;
            else if (key == snake->controls.up)
                snake->direction = UP;
                else if (key == snake->controls.right)
                    snake->direction = RIGHT;
                    else if (key == snake->controls.left)
                        snake->direction = LEFT;
    if(isOppositeDirection(current_direction,snake->direction)){                                                    //если выявлена смена направления на противоположное
            reverseDirection(snake,current_direction);                                                              //разворачиваю змея
    }
    }
}

uint8_t isSelfIntersection(snake_t *head){
    if(head->tsize < 3) {
        return 0;                                                                                                   // не может пересечься с собой, если длина меньше 3
    }
    for(size_t i=1;i<head->tsize;i++){
         if(head->x==head->tail[i].x&&head->y==head->tail[i].y)                                                     //возврат единицы, если координаты головы совпали с координатами одного из элементов списка хвоста
            return 1;
        }
  return 0;
}



void initFood(food_t f[], size_t size){
    food_t local_food={0,0,0,0,0};                                                                                  //всю поля еды заполнил нулями
    for(size_t i=0; i<size;i++){
        food[i]=local_food;
    }
}

//заполнение структуры, описывающей еду
void spawnFood(food_t *fd) {
    static window_size_t w_size={0,0};
    getWindowSize(&w_size);
    uint32_t min_food_x = MIN_X + 2;                                                                                 // отступ от левой границы
    uint32_t min_food_y = MIN_Y + 2;                                                                                 // отступ от верхней границы
    uint32_t max_food_x = w_size.x - MIN_X - 2;                                                                      // отступ от правой границы
    uint32_t max_food_y = w_size.y - MIN_Y - 2;                                                                      // отступ от нижней границы
    fd->x = rand() % (w_size.x) ;
    fd->y = rand() % w_size.y;
    if(fd->x>=max_food_x)fd->x=max_food_x;
    if(fd->x<=min_food_x)fd->x=min_food_x;
    if(fd->y>=max_food_y)fd->y=max_food_y;
    if(fd->y<=min_food_y)fd->y=min_food_y;
    fd->symbol= '#'; 
    fd->enable_time=time(NULL);
    fd->enable_status=1;
}

//очистка места съеденной или протухшей еды
void erasePieceOfFood(food_t *fd){
     mvprintw(fd->y, fd->x, "%c", ' ');
}

//дезинтеграция куска еды
void clearPieceOfFood(food_t *fd){
   fd->enable_status=0;
   erasePieceOfFood(fd);
}

void drawPieceOfFood(food_t *fd){
    mvprintw(fd->y, fd->x, "%c", fd->symbol); 
}

//отрисовка всей еды
void drawAllFood(food_t fd[], size_t food_amount) {
    for(size_t i=0; i<food_amount; i++){
        if(fd[i].enable_status) {                                                                                      //если еда активна
           drawPieceOfFood(&fd[i]);                                                                                    //то отрисовываю ее
        }               
    }
}

//обновление состояния еды
void refreshFood(food_t fd[], size_t food_amount){
    for(size_t i=0; i<food_amount; i++){
        if( fd[i].enable_status){                                                                                       //если еда активна
            if((time(NULL) - fd[i].enable_time) > FOOD_EXPIRE_SECONDS){                                                 //но при этом она протухла
                clearPieceOfFood(&fd[i]);                                                                               //выбрасываю тухлятину
                spawnFood(&fd[i]);                                                                                      //генерирую новую еду
                drawPieceOfFood(&fd[i]);                                                                                //отрисовываю ее
            }
        } else {                                                                                                        //если еда была съедена(!enable_status), генерим новую
            spawnFood(&fd[i]);                                                                                          //генерирую новый кусок еды
            drawPieceOfFood(&fd[i]);                                                                                    // и отрисовываю его
        }
    }
}

// проверка, схавала ли змея еду
uint8_t haveEatFood(snake_t *snake, food_t fd[]) {
    for(size_t i=0;i<MAX_FOOD_SIZE;i++){
        if(fd[i].enable_status&&(snake->x==fd[i].x&&snake->y==fd[i].y)){                                                //если координаты еды и координаты головы совпали
            erasePieceOfFood(&fd[i]);                                                                                   //стираю съеденную еду
            fd[i].enable_status=0;                                                                                      //помечаю, что она съедена
            fd[i].enable_time=0;                                                                                                                                                         
            snake->tsize++;                                                                                             // увеличиваю размер хвоста                                          
            refresh();
            return 1;      
        }
}
    return 0;
}

//проверка наличия еды(может случиться ситуация, когда вся еда протухла и при этом ни один из кусков не был съеден, что приведет при имеющейся логике к отсутвию еды на экране)
void checkPresentFood(food_t *fd){
    
}

int main(){
    snake_t* snake = (snake_t*)malloc(sizeof(snake_t));
    uint32_t key_pressed = 0;

    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    curs_set(FALSE);
    mvprintw(0, 0, " Use arrows for control. Press 'F10' for EXIT");
    timeout(100); 

    initSnake(snake, START_TAIL_SIZE, 10, 10);
    initFood(food,MAX_FOOD_SIZE);
    drawAllFood(food,AMOUNT_FOOD_SPAWN);                                                                                //рисую разрешенное количество кусочков еды
    
     while(key_pressed != EXIT_GAME) {
        key_pressed = getch(); 
        if(isSelfIntersection(snake)) {
            mvprintw(1, 0, " GAME OVER! Press F10 to exit");
            refresh();
            while(key_pressed != EXIT_GAME) {                                                                           //виснем в цикле, пока не будет нажата клавиша выхода
                key_pressed = getch();
            }
        }
        changeDirection(snake, key_pressed);
        haveEatFood(snake,food);
        go(snake);
        goTail(snake);
        refreshFood(food,AMOUNT_FOOD_SPAWN);
        
        refresh();
    }

    free(snake->tail);
    free(snake);
    endwin();
    return 0;
}