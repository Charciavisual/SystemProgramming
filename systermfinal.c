#include <stdio.h>

#include <signal.h>

#include <string.h>

#include <termios.h>

#include <sys/time.h>

#include <stdlib.h>

#include <time.h>

#include <unistd.h>

#include <curses.h>

 

#define MESSAGE_SIZE 13

 

typedef struct message{

        char word[MESSAGE_SIZE];

        int row;

        int col;

}message;

 

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

 

int full=0;

int hp=5;

char hp_message[MESSAGE_SIZE];

message message_list[15]={0,};

int idx=0;

int firstIdx=0;

char word_list[][MESSAGE_SIZE]={"apple","banana","little","phone","car","train","computer","bag","love","star","queen","clock","dog","cat","monkey"};

 

 

void main()

{

        srand(time(NULL));

 

        start();

        endwin();

}

 

void start()

{

        int c;

        int i;

        int input_idx=0;

        int input_row,input_col;

        char input[MESSAGE_SIZE]={0,};

        void move_msg(int);

 

        initscr();

        cr_mode();

        no_echo();

        clear();

 

        input_row=18;

        input_col=50;

	print_map();

 

        refresh();

        set_ticker(1000);

 

        signal(SIGALRM,move_msg);

 

        while(hp>0)

        {

                if(input_idx>=MESSAGE_SIZE)

                {

                        move(input_row,input_col);

                        addstr("             ");

                        move(input_row,input_col);

                        addstr("Over input!  ");

                        break;

                }

 

                c=getch();

 

                if(c=='\n')

                {

                        input[input_idx]='\0';

                }

                else if(c==127)

                {

                        if(idx>0)

                        {

                                input[input_idx--]='\0';

                                move(input_row,input_col);

                                addstr("             ");

                                move(input_row,input_col);

                                addstr(input);

                        }

                        else

                        {

                                move(input_row,input_col);

                                addstr("             ");

                                input_col--;

                        }

                }

                else

                {

                        input[input_idx++]=c;

                        move(input_row,input_col++);

                        addstr(input);

                }

                refresh();

                alarm(1);

                signal(SIGALRM,move_msg);

        }

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

 

void move_msg(int signum)

{

        add_word(word_list[idx],1,rand()%15+43);

        print_word();

        refresh();

}

 

void print_word()

{

        int i;

//      print_map();

        for(i=0;(full != 1 && i<idx) || (full == 1 && i < 15);i++)

        {

                move(message_list[i].row,41);

                addstr("                            ");

                move(message_list[i].row,message_list[i].col);

                addstr(message_list[i].word);

                if(message_list[i].row < 16)

                {

                        message_list[i].row++;

}

                else

                {

                }

        }

        print_message(15,39,"*|~~~~~~~~~~~~dead~~~~~~~~~~~~|*");

        print_message(16,40,"|                            |*");

        move(18,50);

        refresh();

}

 

void add_word(char *word,int row,int col)

{

        int i;

 

        if(idx < 15)

        {

                strcpy(message_list[idx].word,word);

                message_list[idx].row=row;

                message_list[idx++].col=col;

        }

        else

        {

                move(0,0);

                endwin();

        }

}

 

int integer_to_string(int n,char *str)

{

        int temp;

 

        if(n<=0)

        {

                strcpy(str,"0");

                return 0;

        }

 

        temp=integer_to_string(n/10,str);

 

        *(str+temp)=48+(n%10);

 

        return temp+1;

}

 

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

        move(19,50);

	integer_to_string(hp,hp_message);

        addstr(hp_message);

        print_message(20,39,"*==============================*");

 

        print_message(21,39,"********************************");

        move(18,50);

}