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
    int32_t x;
    int32_t y;
}window_size_t;

// струтура , описывающая змею
typedef struct snake_t {
    uint32_t x;                                                  // координата X головы 
    uint32_t y;                                                  // координата Y головы 
    uint32_t direction;                                          // направление движения
    size_t tsize;                                                // размер хвоста
    struct tail_t *tail;                                         // указатель на список хврста
    struct control_buttons controls;                             // массив кнокпок для управления
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
    tail_t init_t = {0,0};
    for(size_t i=0; i<size; i++) t[i] = init_t;
}

// инит змеиной башки
void initHead(snake_t *head, int32_t x, int32_t y) {
    if(x<MIN_X)x=MIN_X;
    if(y<MIN_Y)y=MIN_Y;
    head->x = x;
    head->y = y;
    head->direction = RIGHT;                                                                
}

// инит змеи
void initSnake(snake_t *head, size_t size, int x, int y) {
    tail_t *tail = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head, x, y);
    head->tail = tail;
    head->tsize = size+1;                                                                   
    head->controls = *default_controls;
}



void go(struct snake_t *head){
    char ch = '%';
    window_size_t *window_size=((window_size_t*)malloc(sizeof(window_size_t)));
    getWindowSize(window_size);
    mvprintw(head->y, head->x, " ");                                    //очищаем один символ
    switch (head->direction){
        case LEFT:
            head->x = (head->x <= MIN_X) ? window_size->x - MIN_X : head->x - 1;
            mvprintw(head->y, --(head->x),"%c", ch);                    
            break;
        case RIGHT:
            head->x = (head->x >= window_size->x- MIN_X) ? MIN_X : head->x + 1;
            mvprintw(head->y, ++(head->x), "%c", ch);
            break;
        case UP:
            head->y = (head->y <= MIN_Y) ? window_size->y - MIN_Y : head->y - 1;
            mvprintw(--(head->y), head->x, "%c", ch);
            break;
        case DOWN:
            head->y = (head->y >= window_size->y - MIN_Y) ? MIN_Y : head->y + 1;
            mvprintw(++(head->y), head->x, "%c", ch);
            break;
        default:
            break;
    }
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
    case LEFT:
        snake->direction=DOWN;
        go(snake);
        goTail(snake);
        snake->direction=RIGHT;
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
    return (current_dir == LEFT && new_dir == RIGHT) ||
           (current_dir == RIGHT && new_dir == LEFT) ||
           (current_dir == UP && new_dir == DOWN) ||
           (current_dir == DOWN && new_dir == UP);
}

void changeDirection(snake_t* snake, const int32_t key){
    uint32_t current_direction = snake->direction;                  //запоминаем, в каком направлении двигается змея
    for(uint8_t j=0;j<CONTROLS;j++){                                //проверяем, какой набор клавиш для управления используется
        snake->controls=default_controls[j];
        if (key == snake->controls.down)
            snake->direction = DOWN;
            else if (key == snake->controls.up)
                snake->direction = UP;
                else if (key == snake->controls.right)
                    snake->direction = RIGHT;
                    else if (key == snake->controls.left)
                        snake->direction = LEFT;
    if(isOppositeDirection(current_direction,snake->direction)){  //если выявлена смена направления на противоположное
            reverseDirection(snake,current_direction);             //разворачиваю змея
    }
    }
}

uint8_t isSelfIntersection(snake_t *head){
    if(head->tsize < 3) {
        return 0;                                                   // не может пересечься с собой, если длина меньше 3
    }
    for(size_t i=1;i<head->tsize;i++){
         if(head->x==head->tail[i].x&&head->y==head->tail[i].y)
            return 1;
        }
  return 0;
}

void initFood(food_t f[], size_t size){
    food_t local_food={MIN_X,MIN_Y,0,0,0};
    for(size_t i=0; i<size;i++){
        food[i]=local_food;
    }
}

//заполнение структуры, описывающей еду
void spawnFood(food_t *fd) {
    static window_size_t w_size={0,0};
    getWindowSize(&w_size);
    if((fd->x = rand() % w_size.x)<MIN_X)fd->x=MIN_X+2; //+2 для того, чтобы отодвинуть от края рабочего поля, где может ползать змея
    if((fd->y = rand() % w_size.y)<MIN_Y)fd->y=MIN_Y+2;
    fd->symbol = '#'; 
    fd->enable_time=time(NULL);
    fd->enable_status=1;
    mvprintw(fd->y,fd->x,"%c",fd->symbol);
}

//отрисовка еды
void drawFood(food_t fd[], size_t food_amount) {
    for(size_t i=0; i<food_amount; i++){
        spawnFood(fd);
    }
}

void refreshFood(food_t fd[], size_t food_amount){
    for(size_t i=0; i<food_amount; i++){
        if( fd[i].enable_time ){
            if( !fd[i].enable_status || (time(NULL) - fd[i].enable_time) > FOOD_EXPIRE_SECONDS ){
                spawnFood(&fd[i]);
            }
        }
    }
}

// проверка, схавала ли змея еду
uint8_t checkFood(snake_t *snake, food_t fd[]) {
    for(size_t i=0;i<MAX_FOOD_SIZE;i++){
        if(fd[i].enable_status&&(snake->x==fd[i].x&&snake->y==fd[i].y)){
            snake->tsize++;                                                 // увеличиваем размер хвоста
          //  snake->score += 10;                                            
            fd[i].enable_status=0;
            fd[i].enable_time=0;
            refresh();
            return 1;      
        }
}
    return 0;
}

int main(){
    snake_t* snake = (snake_t*)malloc(sizeof(snake_t));
    initSnake(snake, START_TAIL_SIZE, 10, 10);
    initFood(food,MAX_FOOD_SIZE);
    drawFood(food,AMOUNT_FOOD_SPAWN);
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();
    curs_set(FALSE);
    mvprintw(0, 0, " Use arrows for control. Press 'F10' for EXIT");
    timeout(100);  
     uint32_t key_pressed = 0;
     while(key_pressed != EXIT_GAME) {
        key_pressed = getch(); 
        if(isSelfIntersection(snake)) {
            mvprintw(1, 0, "GAME OVER! Press F10 to exit");
            refresh();
            while(key_pressed != EXIT_GAME) {                //виснем в цикле, пока не будет нажата клавиша выхода
                key_pressed = getch();
            }
        }
        changeDirection(snake, key_pressed);
        checkFood(snake,food);
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