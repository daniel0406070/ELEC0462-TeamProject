#include "display.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

extern player players[MAX_PLAYER];
extern int player_count;
extern char word[BUFSIZ];

void displayInGame() {
	int r, c;
	char* line;
	
	//set row col
	r = TOP_ROW;
	c = LEFT_EDGE;
	
	//clear screen
	clear();
	
	//display players
	for(int i = 0; i < player_count; i++) {
		line = toStringPlayer(players[i]);

		move(r, c);
		addstr(line);

		r++;
	}
	//get spliter like "---- ... ----" form
	line = getSpliter('-', LEFT_EDGE, RIGHT_EDGE);
	
	//move next line and display spliter
	move(r, c);
	addstr(line);
	
	//move down 2 row and print target word
	r += 2;
	move(r, c);
	addstr(word);
	
	//move to bot_row - 5 and display spliter
	r = BOT_ROW -5;
	move(r, c);
	addstr(line);
	
	//move next line
	r++;
}

char* toStringPlayer(player p) {
	char* ret = malloc(MAXLEN_STR);

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
