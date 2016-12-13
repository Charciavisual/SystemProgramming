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

 

#define MESSAGE_SIZE 20
#define BLANK "                            "
#define BUF_SIZE 20
#define INPUT_ROW 18
#define INPUT_COL 50
#define DEAD_LINE 15

// typedef struct message{
//
//         char word[MESSAGE_SIZE];
//
//         int row;
//
//         int col;
//
// }message;

 

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
void input_word(int signum);
//void read_word(int fd);
void enable_kbd_signals();
void game_over();



 

//int full=0;

int stage = 1;



char hp_message[4];

int score = 0;

int dip_cnt = 0;
int rmv_cnt = 0;

int dir=1;

int num_word[3] = {0, 20, 30}; //num of words for each stage
int hp[3] = {0, 10, 20}; // index 0 -> use(x), HP for stage1 = 10, stage2 = 20

//message message_list[15]={0,};

//int idx=0;

//int firstIdx=0;



 
typedef struct word_list{
	
	char word[100];
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
	
	srand(time(NULL));
	
    initscr();

    cr_mode();

    no_echo();

    clear();

	signal(SIGALRM,move_msg);
	
	start();
	// if(pipe(fd)==-1) //pipe 를 통해 child와 parent의 통신 구현
// 		perror("pipe");
//
// 	if((pid=fork())==-1)
// 		perror("fork");
	
	
	
	// if(pid == 0) { //child 에서 단어를 입력받음
// 		input_word(fd[1]);
//
// 	}
//
//
//  	if(pid>0) { //parent 에서는 단어 display
//
// 		signal(SIGIO, read_word());
// 		enable_kbd_signals();
// 		start();
//
//  	}

        

       endwin();

}


void init_stage() {

	clear();
	
	dip_cnt = 0;
	rmv_cnt = 0;
	
	
	
	
}

void input_word(int signum) { //단어를 입력하여 parent로 write
	
	char read_word[20]; 
	
	move(INPUT_ROW, INPUT_COL);
	fgets(read_word, sizeof(read_word), stdin);
	read_word[strlen(read_word)-1] = '\0';
	
	//write(fd, word, BUFSIZ);
	compare_word(read_word);
	move(INPUT_ROW, INPUT_COL);
	
}

void compare_word(char word[]){
	
	int i=0;
	
	for(i=0; i<dip_cnt; i++)
		if(strcmp(display_word[i].word, word)==0){
		
			mvaddstr(display_word[i].row, display_word[i].col, BLANK);
			strcpy(display_word[i].word, BLANK);	
			display_word[i].visible = 0;
			score += 10;
			rmv_cnt++;
			break;
		}
		
		if(rmv_cnt == num_word[stage] && hp[stage] > 0 && stage == 1) {
			
			signal(SIGALRM, SIG_IGN);
			stage++;
			mvaddstr(10,55, "Stage clear\nNext Stage\n");
			init_stage();
			refresh();
			
		}
}
//
// void read_word(int fd) {
//
// 	read(fd, buffer, BUF_SIZE);
//
// 	//출력중인 단어 리스트 순회 -> strcmp == 0 이면 blank
// }

void enable_kbd_signals() {

	int fd_flags;
	fd_flags = fcntl(0, F_GETFL);
	fcntl(0, F_SETFL, (fd_flags|O_ASYNC));
}



void start()

{


	signal(SIGIO, input_word);
	enable_kbd_signals();
        void move_msg(int);
		
		print_map();
		
        refresh();

        set_ticker(1000);

 

        //signal(SIGALRM,move_msg);

 
		
		
       //  while(hp[stage]>0)
//
//         {
//
//                 if(input_idx>=MESSAGE_SIZE)
//
//                 {
//
//                         move(input_row,input_col);
//
//                         addstr("             ");
//
//                         move(input_row,input_col);
//
//                         addstr("Over input!  ");
//
//                         break;
//
//                 }
//
//
//
//                 c=getch();
//
//
//
//                 if(c=='\n')
//
//                 {
//
//                         input[input_idx]='\0';
//
//                 }
//
//                 else if(c==127)
//
//                 {
//
//                         if(idx>0)
//
//                         {
//
//                                 input[input_idx--]='\0';
//
//                                 move(input_row,input_col);
//
//                                 addstr("             ");
//
//                                 move(input_row,input_col);
//
//                                 addstr(input);
//
//                         }
//
//                         else
//
//                         {
//
//                                 move(input_row,input_col);
//
//                                 addstr("             ");
//
//                                 input_col--;
//
//                         }
//
//                 }
//
//                 else
//
//                 {
//
//                         input[input_idx++]=c;
//
//                         move(input_row,input_col++);
//
//                         addstr(input);
//
//                 }
//
//                 refresh();
//
//            	   alarm(1);
//
//            	   signal(SIGALRM,move_msg);
//	
//         }

}

 

void print_message(int row,int col,char *msg)

{

        move(row,col);

        addstr("                                       "); //30칸

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

	if(dip_cnt < num_word[stage])
        add_word(word_list[dip_cnt],0,rand()%15+43); //랜덤위치에 단어 출력
	
	
	for(i=0; i<dip_cnt; i++) {

			if(display_word[i].row < DEAD_LINE && display_word[i].visible == 1) {
		
					mvaddstr(display_word[i].row, 41, BLANK);
					display_word[i].row += dir;
					if(display_word[i].row != DEAD_LINE)
						mvaddstr(display_word[i].row, display_word[i].col, display_word[i].word);
				
			}
			
			if(display_word[i].row == DEAD_LINE && display_word[i].visible == 1) {
			
				display_word[i].visible = 0;
				hp[stage] = hp[stage] - 1;
				rmv_cnt++;		
				sprintf(hp_message, "%1d", hp[stage]);
				mvaddstr(19,50,hp_message);
				
				if(hp[stage] == 0) {
					game_over();

				signal(SIGALRM, SIG_IGN);
				}
				
			
			}
			
			move(INPUT_ROW, INPUT_COL);
		    refresh();	
		}
		
		
	}


void game_over() {
	
	
	int i=0;
	
	mvaddstr(22,50, "GAME OVER!!\n");
	refresh();
	
}

void add_word(char *word,int row,int col)

{

        int i;

 

        // if(idx < 15)
//
//         {

                strcpy(display_word[dip_cnt].word,word);

                display_word[dip_cnt].row=row;

                display_word[dip_cnt].col=col;
				
				display_word[dip_cnt++].visible = 1;
				

	       // }
       //
       //  else
       //
       //  {
       //

    //    }

}


// void print_word()
//
// {
//
//         int i;
//
// 		for(i=0; i<dip_cnt; i++) {
//
// 			if(display_word[i].visible == 1) {
//
// 				mvaddstr(display_word[i].row, display_word[i].col, BLANK);
// 				mvaddstr(display_word[i].row, display_word[i].col, display_word[i].word);
//
// 			}
//
// 		}
// //      print_map();
//
//         // for(i=0;(full != 1 && i<idx) || (full == 1 && i < 15);i++)
// //
// //         {
// //
// //                 move(message_list[i].row,41);
// //
// //                 addstr("                            ");
// //
// //                 move(message_list[i].row,message_list[i].col);
// //
// //                 addstr(message_list[i].word);
// //
// //                 if(message_list[i].row < 16)
// //
// //                 {
// //
// //                         message_list[i].row++;
// //
// // }
// //
// //                 else
// //
// //                 {
// //
// //                 }
//
// //        }
//
//         // print_message(15,39,"*|~~~~~~~~~~~~dead~~~~~~~~~~~~|*");
//  //
//  //        print_message(16,40,"|                            |*");
//
//         move(INPUT_ROW,INPUT_COL);
//
//         refresh();
//
// }

 


// int integer_to_string(int n,char *str)
//
// {
//
//         int temp;
//
//         if(n<=0)
//
//         {
//
//                 strcpy(str,"0");
//
//                 return 0;
//
//         }
//
//         temp=integer_to_string(n/10,str);
//
//         *(str+temp)=48+(n%10);
//
//         return temp+1;
// }

 

void print_map()

{

        int i;

 

        for(i=1;i<20;i++)
                print_message(i,39,"*|                            |*");

 

        print_message(15,39,"*|~~~~~~~~~~~~dead~~~~~~~~~~~~|*");

        print_message(16,40,"|                            |*");

        print_message(17,40,"|============================|*");

        print_message(18,40,"|Input :                     |*");

        print_message(19,40,"|Hp    :                     |*");

        // move(19,50);
 //
 // 		integer_to_string(hp[stage],hp_message);
 //
 //        addstr(hp_message);
		
		sprintf(hp_message, "%d", hp[stage]);
		mvaddstr(19,50,hp_message);

        print_message(20,39,"*==============================*");

 

        print_message(21,39,"********************************");

        move(18,50);

}