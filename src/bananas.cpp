/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

/*
 * This file is part of the code written for the Jupiter Experiment 2017 at Sapienza University.
 * Every tooth in a man's head is more valuable than a diamond.
 * You don't have to brush your teeth — just the ones you want to keep.
 * 
 * 
 */

#include <ncurses.h> 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


enum Direction {
    UP, DOWN, LEFT, RIGHT
};

enum Status {
    SUCCESS, FAILURE
};

struct PointList {
    int x;
    int y;
    struct PointList* next;
};


enum Direction get_next_move(enum Direction previous);
void display_points(PointList* snake, const chtype symbol);
 
void ananas();


typedef struct PointList PointList;

typedef struct {
    PointList* snake;
    PointList* foods;
    int xmax;
    int ymax;
} Board;


bool is_same_place(PointList* cell1, PointList* cell2);
enum Status move_snake(Board* board, enum Direction dir);
PointList* next_move(Board* board, enum Direction dir);
PointList* create_cell(int x, int y);
PointList* create_random_cell(int xmax, int ymax);
PointList* create_snake();
Board* create_board(PointList* foods, PointList* snake, int xmax, int ymax);
bool list_contains(PointList* cell, PointList* list);
bool remove_from_list(PointList* elt, PointList** list);
void add_new_food(Board* board);
 





void display_points(PointList* snake, const chtype symbol) {
    while (snake) {
        mvaddch(snake->y, snake->x, symbol);
        snake = snake->next;
    }
}

enum Direction get_next_move(enum Direction previous) {
    int ch = getch();
    switch (ch) {
        case KEY_LEFT:
            if (previous != RIGHT) return LEFT;
        case KEY_RIGHT:
            if (previous != LEFT) return RIGHT;
        case KEY_DOWN:
            if (previous != UP) return DOWN;
        case KEY_UP:
            if (previous != DOWN) return UP;
        default:
            return previous;
    }
}

 
 int REQUIRED;
 int ACQUIRED;

bool ananas(int required) {
 
    
    if (required == 0) return 1;
    REQUIRED = required;
    ACQUIRED = 0;
    
     srand48(time(0));
    
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE); // make keys work
    curs_set(0); // hide cursor
    
       printw("DATA ANALYSIS WILL BEGIN SOON!\n");
    printw("I'm hungry. Please feed me with some Feroci meat. \n");
    printw("If you succeed, next time I am less hungry.\n");
    printw("If you don't, next time I am more.\n\n");
    printw("And remember, I don't like to pay for cream\n but I don't mind to pay for meat.\n\n");
    printw("Today I need %d steaks\n",required);
    refresh();
    sleep(1);
    
    timeout(100);

    int xmax;
    int ymax;
    getmaxyx(stdscr, ymax, xmax);
    enum Direction dir = RIGHT;

    Board* board = create_board(create_snake(), NULL, xmax, ymax);
    int i;
    for (i = 0; i < 6; i++) {
        add_new_food(board);
    }

    while (true) {
        clear();
        display_points(board->snake, ACS_BLOCK);
        display_points(board->foods, ACS_DIAMOND);
        refresh();
        if (REQUIRED == ACQUIRED){
            endwin();
            return 1;
        }
        dir = get_next_move(dir);
        enum Status status = move_snake(board, dir);
        if (status == FAILURE) break;
    }
    endwin();

    return 0;
     
}

/*
 * Removes from the list or returns false
 */
bool remove_from_list(PointList* elt, PointList** list) {
    PointList *currP, *prevP;
    prevP = NULL;

    for (currP = *list;
            currP != NULL;
            prevP = currP, currP = currP->next) {
        if (is_same_place(currP, elt)) {
            if (prevP == NULL) {
                *list = currP->next;
            } else {
                prevP->next = currP->next;
            }
            free(currP);
            return true;
        }
    }
    return false;
}

enum Status move_snake(Board* board, enum Direction dir) {
    // Create a new beginning. Check boundaries.
    PointList* beginning = next_move(board, dir);
    if (beginning == NULL) {
        return FAILURE;
    }

    // If we've gone backwards, don't do anything
    if (board->snake->next && is_same_place(beginning, board->snake->next)) {
        beginning->next = NULL;
        free(beginning);
        return SUCCESS;
    }

    // Check for collisions
    if (list_contains(beginning, board->snake)) {
        return FAILURE;
    }

    // Check for food
    if (list_contains(beginning, board->foods)) {
        // Attach the beginning to the rest of the snake;
        beginning->next = board->snake;
        board->snake = beginning;
        remove_from_list(beginning, &(board->foods));
        ACQUIRED++;
        add_new_food(board);

        return SUCCESS;
    }

    // Attach the beginning to the rest of the snake
    beginning->next = board->snake;
    board->snake = beginning;


    // Cut off the end
    PointList* end = board->snake;
    while (end->next->next) {
        end = end->next;
    }
    free(end->next);
    end->next = NULL;

    return SUCCESS;
}

bool is_same_place(PointList* cell1, PointList* cell2) {
    return cell1->x == cell2->x && cell1->y == cell2->y;
}

PointList* next_move(Board* board, enum Direction dir) {
    PointList* snake = board->snake;
    int new_x = snake->x;
    int new_y = snake->y;
    switch (dir) {
        case UP:
            new_y = snake->y - 1;
            break;
        case DOWN:
            new_y = snake->y + 1;
            break;
        case LEFT:
            new_x = snake->x - 1;
            break;
        case RIGHT:
            new_x = snake->x + 1;
            break;
    }
    if (new_x < 0 || new_y < 0 || new_x >= board->xmax || new_y >= board->ymax) {
        return NULL;
    } else {
        return create_cell(new_x, new_y);
    }
}

PointList* create_random_cell(int xmax, int ymax) {
    return create_cell(lrand48() % xmax, lrand48() % ymax);
}

void add_new_food(Board* board) {
    PointList* new_food;
    do {
        new_food = create_random_cell(board->xmax, board->ymax);
    } while (list_contains(new_food, board->foods) || list_contains(new_food, board->snake));
    new_food->next = board->foods;
    board->foods = new_food;
}

bool list_contains(PointList* cell, PointList* list) {
    PointList* s = list;
    while (s) {
        if (is_same_place(s, cell)) {
            return true;
        }
        s = s->next;
    }
    return false;
}

PointList* create_cell(int x, int y) {
    PointList* cell = (PointList*) malloc(sizeof (*cell));
    cell->x = x;
    cell->y = y;
    cell->next = NULL;
    return cell;
}

Board* create_board(PointList* snake, PointList* foods, int xmax, int ymax) {
    Board* board = (Board*) malloc(sizeof (*board));
    board->foods = foods;
    board->snake = snake;
    board->xmax = xmax;
    board->ymax = ymax;
    return board;
}

PointList* create_snake() {
    PointList* a = create_cell(2, 3);
    PointList* b = create_cell(2, 2);
    a->next = b;
    return a;
}
 