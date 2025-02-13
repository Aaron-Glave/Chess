/*
Reminder: The black perspective is facing the other way, so their first rook
from their perspective is on the far RIGHT.
/*/ 
#include "Knight.h"
#include "King.h"
#include "Team.h"
#include "Teamname.h"
#include "Piece.h"
#include "Board.h"
#include "Rook.h"
#include "Check_vs_Checkmate.h"
#include "Pawn_Upgrader.h"
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS //prevents warnings for using non-Microsoft functions
#pragma warning(disable:4996)
//public
//variables


Team::Team() {
	enemy_team = nullptr;
	empty_spaces();
}
Team::Team(COLOR team_color, Board *the_board_shared) :the_king(WHITE)
{
	enemy_team = nullptr;
	empty_spaces();
	color = team_color;
	current_status = NEUTRAL;
	if (team_color == WHITE) {
		namefirstletter = 'w';
		strcpy(full_name, "White");
		empty_spaces();
		knight1 = Knight(team_color, 1, 2, 1);
		knight2 = Knight(team_color, 1, 7, 2);
		rook1 = Rook(team_color, 1, 1, 1);
		rook2 = Rook(team_color, 1, 8, 2);
		bishop1 = Bishop(team_color, 1, 3, 1);
		bishop2 = Bishop(team_color, 1, 6, 2);
		//TODO: ASSING PAWNS DIRECTLY TO THE PIECES ARRAY HERE.
		for (int i = 8; i < 16; i++) {
			pawns[i - 8] = Pawn(team_color, 2, i, i);
		}
	}
	else //team_color == BLACK
	{
		namefirstletter = 'b';
		strcpy(full_name, "Black");
		empty_spaces();
		knight1 = Knight(team_color, 8, 2, 2);
		knight2 = Knight(team_color, 8, 7, 1);
		rook1 = Rook(team_color, 8, 1, 2);
		rook2 = Rook(team_color, 8, 8, 1);
		bishop1 = Bishop(team_color, 8, 3, 2);
		bishop2 = Bishop(team_color, 8, 6, 1);
		//TODO: ASSING PAWNS DIRECTLY TO THE PIECES ARRAY HERE. AND HERE
		for (int i = 1; i <= 8; i++) {
			pawns[i - 8] = Pawn(team_color, 7, i, i);
		}
	}
	pieces[0] = &rook1;
	pieces[1] = &knight1;
	pieces[2] = &bishop1;
	//Queen goes here.
	the_king = King(team_color);
	pieces[4] = &the_king;
	pieces[5] = &bishop2;
	pieces[6] = &knight2;
	pieces[7] = &rook2;
	for (int i = 0; i < 8; i++) {
		pieces[i + 8] = &pawns[i];
	}

	// Now put the pieces on our team on the board.
	put_pieces_on_board(the_board_shared);
}
void Team::empty_spaces() {
	for (int i = 0; i < 16; i++) {
		pieces[i] = 0;
	}
}

//TODO: Function to change a piece's class.
void Team::put_pieces_on_board(Board* the_board_shared) {
	for (int i = 0; i < 16; i++) {
		if (pieces[i] != 0) {
			the_board_shared->spaces[pieces[i]->row - 1][pieces[i]->column - 1] = pieces[i];
		}
	}
}