#include <stdio.h>
#include <curses.h>
#include "typeracer.h"

extern char* word;
extern int player_count;
extern player players[]; 

char last_word[BUF_SIZE]; //마지막으로 인식한 word 저장용 array

int left_edge = 5;
int right_edge = 35;
int top_row = 5;
int bot_row = 55;

char* toStringPlayer(palyer);
char* getSpliter(char, int, int);

void *displayScreen(){
	//init screen
	initscr();
	
	int r, c;
	char* line;
	while(1) {
		//if word is not updated -> don't update the screen
		if(strcmp(word, last_word) == 0)
			continue;

		//set row col
		r = top_row;
		c = left_edge;

		//clear screen
		clear();
		
		//display palyers
		for(int i = 0; i < player_count; i++) {
			//get player to string in "<player name>: <score>" form
			line = toStringPlayer(players[i]);

			//move cursor and add string	
			move(r, c);
			addstr(line);
			
			//1 row down
			r++;
		}
		//get spliter "--------- ... --------" form
		line = getSpliter('-', left_edge, right_edge);

		//move next line and display spliter
		move(r, c);
		addstr(line);
		
		//move down 2 row and print target word
		r += 2;
		move(r, c);
		addstr(word);
		
		//move to bot_row - 5
		r = bot_row - 5;
		move(r, c);
		
		//display spliter
		addstr(line);
		
		//move nextline
		r++;
		move(r, c);

		//update word
		strcpy(last_word, word);
	}
}

char* toStringPlayer(player p) {
	char result[BUF_SIZE];

	sprintf(result, " %20s: %d", p.name, p.score);

	return result;
}

char* getSpliter(char elem, int left, int right) {
	char result[BUF_SIZE];
	int length = right - left;
	
	int i = 0;
	for(; i < length; i++)
		result[i] = elem;
	result[i] = '\0';

	return result;
}
