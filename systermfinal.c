#include <stdio.h>

#include <signal.h>

#include <string.h>

#include <termios.h>

#include <sys/time.h>

#include <stdlib.h>

#include <time.h>

#include <unistd.h>

#include <curses.h>
#include <fcntl.h>
#include <pthread.h>

 

#define MESSAGE_SIZE 20
#define BLANK "                                                  "
#define BUF_SIZE 20
#define INPUT_ROW 26
#define INPUT_COL 20
#define DEAD_LINE 23
#define LEFT_EDGE 10

void start();
void no_echo();
void cr_mode();
int set_ticker(int);
void print_message(int,int,char*);
void print_word();
void add_word(char*,int,int);
int integer_to_string(int,char*);
void print_map();
void game_end();
void init_stage();
void move_msg(int signum);
void compare_word(char word[]);
void input_word();
void enable_kbd_signals();
void game_over();
void sigio_handler(int signum);
void *thread_1(void *none);
void clear_check();

 

int stage = 1;
char hp_message[5];
int score = 0;
int dip_cnt = 0;
int rmv_cnt = 0;
int dir=1;
int delay = 1500;
int tic_cnt = 0;
int num_word[3] = {0, 20, 30}; //num of words for each stage
int hp[3] = {0, 10, 20}; // index 0 -> use(x), HP for stage1 = 10, stage2 = 20
int fd[2];
int exit_program = 1;



char read_word[20]; 
 
typedef struct word_list{
	
	char word[20];
	int row;
	int col;
	int visible;
	
}WL;

WL display_word[30];

char word_list[30][MESSAGE_SIZE] = {"apple","banana","little","phone","car"
					      ,"train","computer","bag","love","star"
					      ,"queen","clock","dog","cat","monkey"
					      ,"system","programming","project","linux","google"
					      ,"train","computer","bag","love","star"
					      ,"queen","clock","dog","cat","monkey"};

void main()

{
	pthread_t t1;
	void sigint_handler(int signum);
	
	signal(SIGINT, sigint_handler);
	srand(time(NULL));
	
    initscr();

    cr_mode();

    no_echo();

    clear();

	pthread_create(&t1, NULL, thread_1, NULL);
	
	while(exit_program) 
		input_word();

	pthread_join(t1, NULL);

       endwin();

}
void sigint_handler(int signum) {
	
	endwin();
	exit(0);
}

void sigio_handler(int signum) {
	
	input_word();
}

void init_stage() {

	int i = 0;
	char start_msg[20];
	
	dip_cnt = 0;
	rmv_cnt = 0;
	tic_cnt = 0;
	delay = 1000;
	
	hp[1] = 10;
	hp[2] = 20;
	
	clear();
	print_map();
	sprintf(start_msg, "Stage : %d", stage);
	move(DEAD_LINE+8,LEFT_EDGE+1);
	addstr(BLANK);
	move(DEAD_LINE+9,LEFT_EDGE+1);
	addstr(BLANK);
	move(DEAD_LINE+8,(LEFT_EDGE+50)/2);
	addstr(start_msg);
	
	
	
	
	refresh();
	
	
}

void *thread_1(void *none){
	
	start();
	
	while(exit_program)
		pause();
}

void input_word() {
	
	
	char c;
	int i=0;
	move(INPUT_ROW, INPUT_COL);

	while(1) {

		c = getch();

		if(c=='\n') {
			read_word[i] = '\0';
			move(INPUT_ROW, INPUT_COL);
			addstr("                                          ");
			move(INPUT_ROW, INPUT_COL);
			break;
		}

		else if(c == '\b') { // input backspace

			if(i > 0){
				i--;
				read_word[i] = '\0';
				move(INPUT_ROW, INPUT_COL);
				addstr("                   ");
				addstr(read_word);
				i--;
			}
			
			else {
				move(INPUT_ROW, INPUT_COL);
				addstr("                   ");
				i--;
			}
		}
		
		else
			read_word[i] = c;
		
			
		
		i++;
	}
	compare_word(read_word);
	move(INPUT_ROW, INPUT_COL);
	
}

void compare_word(char word[]){
	
	int i=0;
	
	for(i=0; i<dip_cnt; i++)
		if(strcmp(display_word[i].word, word)==0)
			if(display_word[i].visible == 1){
		
				mvaddstr(display_word[i].row, LEFT_EDGE+2, BLANK);	
				display_word[i].visible = 0;
				score += 10;
				rmv_cnt++;
				sprintf(hp_message, "%d", score);
				mvaddstr(DEAD_LINE+5,INPUT_COL,hp_message);
				clear_check();
				break;
			}
		
		for(i=0; i<MESSAGE_SIZE; i++)
			read_word[i] = '\0';
}

void clear_check() {
	
	if(hp[stage] == 0) {
		
		
		signal(SIGALRM, SIG_IGN);
		game_over();
	}
	
	
	else if(rmv_cnt == num_word[stage] && hp[stage] > 0 && stage == 1) {
		signal(SIGALRM, SIG_IGN);
		stage++;
		move(DEAD_LINE+8,11);
		addstr(BLANK);
		move(DEAD_LINE+8,(LEFT_EDGE+50)/2);
		addstr("Stage Clear!!");
		move(DEAD_LINE+9,((LEFT_EDGE+50)/2)-8);
		addstr("Wait for start next stage.");
		refresh();
		sleep(3);
		start();
	}
	
	else if(rmv_cnt == num_word[stage] && hp[stage] > 0 && stage == 2) {
		signal(SIGALRM, SIG_IGN);
		move(DEAD_LINE+8,11);
		addstr(BLANK);
		move(DEAD_LINE+8,(LEFT_EDGE+50)/2);
		addstr("Final Stage Clear!!");
		move(DEAD_LINE+9,((LEFT_EDGE+50)/2));
		addstr("Congraturations!!!");
		refresh();
		exit_program = 0;
		
	}
}

void enable_kbd_signals() {

	int fd_flags;
	fd_flags = fcntl(0, F_GETFL);
	fcntl(0, F_SETFL, (fd_flags|O_ASYNC));
}



void start()

{
		signal(SIGALRM,move_msg);
        void move_msg(int);	
		init_stage();
        set_ticker(delay);

}

 

void print_message(int row,int col,char *msg)

{

        move(row,col);
		
        addstr("                                                  "); 

        move(row,col);

        addstr(msg);

		refresh();

}

 

void cr_mode()

{

        struct termios ttystate;

 

        tcgetattr(0,&ttystate);

        ttystate.c_lflag &= ~ICANON;

        ttystate.c_cc[VMIN] = 1;

        tcsetattr(0,TCSANOW,&ttystate);

}

 

void no_echo()

{

        struct termios ttystate;

 

        tcgetattr(0,&ttystate);

        ttystate.c_lflag &= ~ICANON;

        ttystate.c_lflag &= ~ECHO;;

        ttystate.c_cc[VMIN] = 1;

        tcsetattr(0,TCSANOW,&ttystate);

}

 

int set_ticker(int n_msecs)

{

        struct itimerval new_timeset;

        long n_sec,n_usecs;

 

        n_sec=n_msecs/1000;

        n_usecs=(n_msecs%1000)*1000L;

 

        new_timeset.it_interval.tv_sec  = n_sec;

        new_timeset.it_interval.tv_usec = n_usecs;

        new_timeset.it_value.tv_sec     = n_sec;

        new_timeset.it_value.tv_usec    = n_usecs;

 

        return setitimer(ITIMER_REAL,&new_timeset,NULL);

}

 

void move_msg(int signum){

	int i=0;

	tic_cnt++;
	if(tic_cnt%3 == 0){
		if(delay >300) {
			delay = delay - 50;
			set_ticker(delay);
		}
	}
	if(dip_cnt < num_word[stage])
        add_word(word_list[dip_cnt],0,rand()%42+12); //랜덤위치에 단어 출력
	
	
	for(i=0; i<dip_cnt; i++) {

			if(display_word[i].row < DEAD_LINE && display_word[i].visible == 1) {
		
					mvaddstr(display_word[i].row, LEFT_EDGE+2, BLANK);
					display_word[i].row += dir;
					if(display_word[i].row != DEAD_LINE)
						mvaddstr(display_word[i].row, display_word[i].col, display_word[i].word);
				
			}
			
			if(display_word[i].row == DEAD_LINE && display_word[i].visible == 1) {
			
				display_word[i].visible = 0;
				hp[stage] = hp[stage] - 1;
				rmv_cnt++;		
				sprintf(hp_message, "%d", hp[stage]);
				mvaddstr(DEAD_LINE+4,INPUT_COL ,"          ");
				mvaddstr(DEAD_LINE+4,INPUT_COL ,hp_message);
				
				if(hp[stage] == 0)
					signal(SIGALRM, SIG_IGN);
			
			}
			
			move(INPUT_ROW, INPUT_COL);
			addstr("                                          ");
			move(INPUT_ROW, INPUT_COL);
			addstr(read_word);
			
		    refresh();
			clear_check();
		}
	}


void game_over() {
	
	char c;
			
			score = 0;
			stage = 1;
			move(DEAD_LINE+8,(LEFT_EDGE+50)/2);
			addstr(BLANK);
			move(DEAD_LINE+8,(LEFT_EDGE+50)/2);
			addstr("GAME OVER!!");
			move(DEAD_LINE+9,(LEFT_EDGE+50)/2);
			addstr("Retry?(y/n)");
			refresh();
	
	
				while(1) {
				c=getchar();
	
				if(c == 'y') {
					fflush(stdin);
					start();
					break;
				}
	
				else if(c == 'n') {
					fflush(stdin);
					endwin();
					exit(0);
					break;
				}
	
				}
}

void add_word(char *word,int row,int col)

{

        int i;
        strcpy(display_word[dip_cnt].word,word);
		display_word[dip_cnt].row=row;
		display_word[dip_cnt].col=col;		
		display_word[dip_cnt++].visible = 1;
}


void print_map()

{

        int i;

 

        for(i=1;i<DEAD_LINE+7;i++)
        print_message(i, LEFT_EDGE,"*|                                                  |*");

 

        print_message(DEAD_LINE,LEFT_EDGE+1,"|~~~~~~~~~~~~~~~~~~~~~~~dead~~~~~~~~~~~~~~~~~~~~~~~|");

      print_message(DEAD_LINE+1,LEFT_EDGE+1 ,"|                                                  |");

         print_message(DEAD_LINE+2,LEFT_EDGE+1,"|==================================================|");

         print_message(DEAD_LINE+3,LEFT_EDGE+1,"|Input :                                           |");

        print_message(DEAD_LINE+4,LEFT_EDGE+1,"|Hp    :                                           |");
		print_message(DEAD_LINE+5,LEFT_EDGE+1,"|Score :                                           |");


		sprintf(hp_message, "%d", hp[stage]);
		mvaddstr(DEAD_LINE+4,INPUT_COL,hp_message);
		sprintf(hp_message, "%d", score);
		mvaddstr(DEAD_LINE+5,INPUT_COL,hp_message);

       print_message(DEAD_LINE+6,LEFT_EDGE+1,"|==================================================|");
         print_message(DEAD_LINE+7,LEFT_EDGE+1,"***************************************************");

        move(INPUT_ROW,INPUT_COL);

}