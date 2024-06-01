#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "typeracer.h"
#include "displayScreen.h"

#define MAXLEN 200

int left_edge = 5;
int right_edge = 35;
int top_row = 5;
int bot_row = 55;

void displayInGame() {
	int r, c;
	char* line;
	
	//set row col
	r = top_row;
	c = left_edge;
	
	//clear screen
	clear();
	
	//display players
	for(int i = 0; i < player_count; i++) {
		line = toSringPlayer(players[i]);

		move(r, c);
		addstr(line);

		r++;
	}
	//get spliter like "---- ... ----" form
	line = getSpliter('-', left_edge, right_edge);
	
	//move next line and display spliter
	move(r, c);
	addstr(line);
	
	//move down 2 row and print target word
	r += 2;
	move(r, c);
	addstr(word);
	
	//move to bot_row - 5 and display spliter
	r = bot_row -5;
	move(r, c);
	addstr(line);
	
	//move next line
	r++;
}

char* toStringPlayer(player p) {
	char* ret = malloc(MAXLEN);

	sprintf(ret, "%20s %d", p.name, p.score);

	return ret;
}

char* getSpliter(char elem, int left, int right) {
	int length = right - left;
	char* ret = malloc(length);
	
	int i = 0;
	for(; i < length; i++)
		ret[i] = elem;
	ret[i] = '\n';

	return ret;
}
