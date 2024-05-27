#include <stdio.h>
#include <stdlib.h> // malloc, free 함수 선언
#include <ncurses.h>
#include <time.h>

#define BLOCK_SIZE 4
#define MAP_HEIGHT 20
#define MAP_LENGTH 14 //fakjse;l

int random_select();
bool check_collision(int temp_x, int temp_y, int temp_rotate);
void block_shape();
void block_remake();
void time_block_down();
void block_rotate_move();
void delete_line(WINDOW *win);
void true_ending(WINDOW *win);
void block_print_win(WINDOW *win);
void limit_height_ending(WINDOW *win);

struct blocks {
        int shape[4][4];
};

struct blocks block[4][4];

int block_select;
int rotate = 0;
int x = 2;
int y = 0;
int end_condition = 0;
int **gamemap;

time_t start, end;

int main(void) {

        initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	curs_set(false);

        gamemap = (int **)malloc(MAP_HEIGHT * sizeof(int *));
        for (int i = 0; i < MAP_HEIGHT + 1; i++) {
                gamemap[i] = (int *)malloc(MAP_LENGTH * sizeof(int));
                for (int j =0; j < MAP_LENGTH; j++) {
                        gamemap[i][j] = 0;
                }
        }


        block_shape();

	block_select = random_select();
	time(&start);

	WINDOW *win = newwin(MAP_HEIGHT, MAP_LENGTH, 10, 20);
	box(win, 0, 0);

	while(1) {

	werase(win);
	box(win, 0, 0);

	block_print_win(win);

        delete_line(win);

	wrefresh(win);
	refresh();

        true_ending(win);
        limit_height_ending(win);

        time_block_down();
        block_rotate_move();

        napms(50);

	}

        for (int i = 0; i < MAP_HEIGHT + 1; i++) {
                free(gamemap[i]);
        }
        
        free(gamemap);

	delwin(win);
	endwin();

	return 0;

}


int random_select() {
 srand((unsigned int)time(NULL));
 return rand()%4;
}

bool check_collision(int temp_x, int temp_y, int temp_rotate) {
        for (int i = 0; i < BLOCK_SIZE; i++) {
                for (int j = 0; j < BLOCK_SIZE; j++) {
                        if (block[temp_rotate][block_select].shape[i][j] == 1) {
                                if (gamemap[temp_y + i][temp_x + j] != 0 || temp_x + j < 0 || temp_y + i >= MAP_HEIGHT -1 || temp_x + j >= MAP_LENGTH) {
                                        return true;
                                }
                        }
                }
        }

        return false;
}

void block_shape () {
        struct blocks T_block = {
                { {0,0,0,0}, {0,1,0,0}, {1,1,1,0}, {0,0,0,0} }
        };

        struct blocks i_block = {
                { {0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}, }
        };

        struct blocks square_block = {
                { {0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}, }
        };

        struct blocks L_block = {
                { {0,0,0,0}, {0,1,1,0}, {0,1,0,0}, {0,1,0,0}, }
        };

        block[0][0] = T_block;
        block[0][1] = i_block;
        block[0][2] = square_block; 
        block[0][3] = L_block;

        
        block[1][0]  = (struct blocks) { .shape = { {0,0,0,0}, {0,0,1,0}, {0,0,1,1}, {0,0,1,0}}};
        block[1][1]  = (struct blocks) { .shape = { {1,0,0,0}, {1,0,0,0}, {1,0,0,0}, {1,0,0,0}}};
        block[1][2]  = square_block;
        block[1][3]  = (struct blocks) { .shape = { {0,0,0,0}, {0,1,1,1}, {0,0,0,1}, {0,0,0,0}}};
             
        block[2][0]  = (struct blocks) { .shape = { {0,0,0,0}, {0,0,0,0}, {1,1,1,0}, {0,1,0,0}}};
        block[2][1]  = i_block;
        block[2][2]  = square_block;
        block[2][3]  = (struct blocks) { .shape = { {0,0,0,0}, {0,0,1,0}, {0,0,1,0}, {0,1,1,0}}};
        
        block[3][0]  = (struct blocks) { .shape = { {0,0,0,0}, {0,1,0,0}, {1,1,0,0}, {0,1,0,0}}};
        block[3][1]  = (struct blocks) { .shape = { {1,0,0,0}, {1,0,0,0}, {1,0,0,0}, {1,0,0,0}}};
        block[3][2]  = square_block;
        block[3][3]  = (struct blocks) { .shape = { {0,0,0,0}, {1,0,0,0}, {1,1,1,0}, {0,0,0,0}}};

}

void block_print_win(WINDOW *win) {  // 블럭을 윈도우로 출력
	for(int i=0; i < MAP_HEIGHT; i++) {
	 for(int j = 0; j < MAP_LENGTH; j++) {
	  if(gamemap[i][j] != 0) {
             mvwprintw(win, i, j, "X");
	}
       }
      }
	 for(int i = 0; i < BLOCK_SIZE; i++) {
	  for(int j = 0; j < BLOCK_SIZE; j++) {
	   if (block[rotate][block_select].shape[i][j] == 1) {
             mvwprintw(win, i + y, j+x, "X");
        }
       }
      }
}

void time_block_down() { // 1초마다 블럭의 위치 조정
        time(&end);
        int diff_time = difftime(end, start);

        if (diff_time != 0) {
                if (!check_collision(x, y + 1, rotate)) {
                        y += 1;
                }

                else {
                        block_remake();
                }
                time(&start);
        }

}

void block_rotate_move() { // 회전 , 좌우 이동
        int key_input = getch();

        if (key_input != ERR) {

                int temp_x = x;
                int temp_y = y;
                int temp_rotate = rotate;
                
                if (key_input == 'r') {
                        temp_rotate = (rotate+1) % 4; 
                }
                else if (key_input == 'a' && x > 0) {
                        temp_x = x - 1; 
                }
                else if (key_input == 'd' && x < MAP_LENGTH - BLOCK_SIZE ) {
                        temp_x = x + 1; 
                }
                else if (key_input == 's') {
                        temp_y = y + 1;
                }
        
                if (!check_collision(temp_x, temp_y, temp_rotate)) {
                        x = temp_x;
                        y = temp_y;
                        rotate = temp_rotate;
                }
        } 
}

void block_remake() {
         for (int i = 0; i < BLOCK_SIZE; i++) {
          for (int j = 0; j < BLOCK_SIZE; j++) {
            if (block[rotate][block_select].shape[i][j] == 1) {
               gamemap[y + i][x + j] = block_select + 1;
             }
            }
           }
          block_select = random_select();
          rotate = 0;
          x = 2;
          y = 0;

}

void delete_line(WINDOW *win) {
        for (int i = 0; i < MAP_HEIGHT; i++) {
                int cnt = 0;
                for (int j = 0; j < MAP_LENGTH; j++) {
                        if (gamemap[i][j] != 0) {
                                cnt += 1;
                        }
                }


                if (cnt >= MAP_LENGTH -2) {
                                for (int k = i; k > 0; k-- ) {
                                        for (int j = 0; j < MAP_LENGTH; j++) {
                                                gamemap[k][j] = gamemap[k-1][j];
                                        }
                                }       
                        for (int j = 0; j < MAP_LENGTH; j++) {
                                gamemap[0][j] = 0;
                        }

                        end_condition += 1;
                        if (end_condition >= 20) {
                                true_ending(win);
                        }
                }    
        }
}

void limit_height_ending (WINDOW *win) {
        for (int j = 0; j < MAP_LENGTH; j ++) {
                if (gamemap[5][j] != 0) {
                        mvwprintw(win, MAP_HEIGHT/2, 0, "   Gameover!\n");
                        wrefresh(win);
                        delwin(win);
                        napms(10000);
                        endwin();
                        exit(0);
                }
        }
}

void true_ending (WINDOW *win) {
        if ( end_condition >= 20 ) {
                mvwprintw(win, MAP_HEIGHT/2, 0 , "    Clear!\n");
                wrefresh(win);
                delwin(win);
                napms(10000);
                endwin();
                exit(0);
        }

}