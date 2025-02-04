#pragma once
#ifndef TEAM_INCLUDED
#define TEAM_INCLUDED
#include "Knight.h"
#include "King.h"
#include "Teamname.h"
#include "Piece.h"
#include "Rook.h"
#include "Bishop.h"
#include "Pawn.h"
#include "Check_vs_Checkmate.h"

class Board;

class Team {
	//public
public:
	//variables
	char namefirstletter;
	char full_name[6];
	Piece* pieces[16];
	COLOR color;
	Knight knight1;
	Knight knight2;
	Rook rook1;
	Rook rook2;
	Bishop bishop1;
	Bishop bishop2;
	King the_king;
	Piece pawns[8];
	Team* enemy_team;
	Game_Status current_status;

	//functions
	//Team(COLOR team_color);
	Team(COLOR team_color, Board* the_board_shared);
	Team();
	void empty_spaces();
	void upgrade_pawn(Pawn* pawn_to_upgrade);
	void put_pieces_on_board(Board* the_board_shared);
};

#endif TEAM_INCLUDED