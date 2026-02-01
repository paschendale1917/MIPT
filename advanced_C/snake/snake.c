#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses/ncurses.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
//#include <ctype.h> // for toupper()

#define NUM_SNAKES                                      1
#define MIN_Y                                           2                                                                            // Minimum Y coordinate to avoid writing over instructions

enum {LEFT=1, UP, RIGHT, DOWN, EXIT_GAME=KEY_F(10),END_GAME};
enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=6, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10};

// Structure for control keys
struct control_buttons {
    uint32_t down;
    uint32_t up;
    uint32_t left;
    uint32_t right;
};

// Standard control keys
struct control_buttons default_controls[NUM_SNAKES] = {
    {KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT}     // arrows for first snake
    //{'s', 'w', 'a', 'd'}                         // WASD for second snake
};

// Structure for one tail segment
typedef struct tail_t {
    uint32_t x;
    uint32_t y;
} tail_t;

 struct window_size{
    uint32_t x;
    uint32_t y;
};

struct window_size *window_size;

// Snake structure
typedef struct snake_t {
    uint32_t x;                                                  // head X coordinate
    uint32_t y;                                                  // head Y coordinate
    uint32_t direction;                                          // movement direction
    size_t tsize;                                                // current tail size
    struct tail_t *tail;                                         // tail array
    struct control_buttons controls;                             // control keys
 //   uint32_t score;
 //   uint8_t is_alive;
} snake_t;

// Food structure
typedef struct food_t {
    uint32_t x;
    uint32_t y;
    char symbol;
} food_t;

void getWindowSize(struct window_size *size){
    getmaxyx(stdscr, size->y, size->x);
}

// Initialize tail (zero coordinates)
void initTail(tail_t t[], size_t size) {
    tail_t init_t = {0,0};
    for(size_t i=0; i<size; i++) t[i] = init_t;
}

// Initialize snake head
void initHead(snake_t *head, int x, int y) {
    head->x = x;
    head->y = y;
    head->direction = RIGHT;                                                                // initial direction right
   // head->score = 0;
  //  head->is_alive = 1;
}

// Initialize whole snake
void initSnake(snake_t *head, size_t size, int x, int y) {
    tail_t *tail = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head, x, y);
    head->tail = tail;
    head->tsize = size+1;                                                                   // initial tail
    head->controls = *default_controls;
}


// Generate food at random position
// void spawnFood(food_t *food, int max_x, int max_y) {
//     food->x = rand() % max_x;
//     food->y = (rand() % (max_y - MIN_Y)) + MIN_Y;
//     food->symbol = '#'; // food symbol
// }

// // Draw food on screen
// void drawFood(food_t *food) {
//     attron(COLOR_PAIR(8)); // Color for food (red)
//     mvprintw(food->y, food->x, "%c", food->symbol);
//     attroff(COLOR_PAIR(8));
// }

// // Check if snake ate food
// int checkFood(snake_t *snake, food_t *food) {
//     if(snake->x == food->x && snake->y == food->y) {
//         snake->tsize++; // increase tail
//         snake->score += 10; // add points
//         playSound(1); // Food eating sound
//         return 1;       // food eaten
//     }
//     return 0;
// }

void go(struct snake_t *head){
    char ch = '@';
    uint32_t max_x = 0, max_y = 0;
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(head->y, head->x, " "); //очищаем один символ
    switch (head->direction){
        case LEFT:
            head->x = (head->x <= 0) ? max_x - 1 : head->x - 1;
            mvprintw(head->y, --(head->x),"%c", ch);//добавить проверку выхода
            break;
        case RIGHT:
            head->x = (head->x >= max_x- 1) ? 0 : head->x + 1;
            mvprintw(head->y, ++(head->x), "%c", ch);
            break;
        case UP:
            head->y = (head->y <= MIN_Y) ? max_y - 1 : head->y - 1;
            mvprintw(--(head->y), head->x, "%c", ch);
            break;
        case DOWN:
            head->y = (head->y >= max_y - 1) ? MIN_Y : head->y + 1;
            mvprintw(++(head->y), head->x, "%c", ch);
            break;
        default:
            break;
    }
    refresh();
}

void goTail(struct snake_t *head){
    char ch = '*';
    mvprintw(head->tail[head->tsize-1].y,
    head->tail[head->tsize-1].x," ");
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



int main(){
    snake_t* snake = (snake_t*)malloc(sizeof(snake_t));
    initSnake(snake, START_TAIL_SIZE, 10, 10);
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
            //break;
        }
        changeDirection(snake, key_pressed);
        go(snake);
        goTail(snake);
        refresh();
    }
    
    free(snake->tail);
    free(snake);
    endwin();
    return 0;
}