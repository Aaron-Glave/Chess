#include "King.h"
#include "Piece.h"
#include "Teamname.h"
#include "Board.h"
#include <string.h>
#include <stdio.h>


King::King() {
	king_setup(WHITE);
}
King::King(COLOR b_team)
{
	king_setup(b_team);
		
}

bool King::is_adjacent(int b_row, int b_column, bool test = false) {
	if ((b_row >= row - 1) && (b_row <= row + 1)) {
		if (test) {
			printf("Test works at least.\n");
		}
		if ((b_column >= column - 1) && (b_column <= column + 1)) return true;
	}
	return false;
}

bool King::can_kingmove(int b_row, int b_column, Board *mainboard, bool *landed_on_king) {
	if (!is_on_board(b_row, b_column)) return false;
	Piece* piece_in_that_square = mainboard->spaces[b_row - 1][b_column - 1];
	if (!is_on_board(b_row, b_column)) {
		return false;
	}
	if (!is_adjacent(b_row, b_column)) return false;
	
	//notdone
	if (piece_in_that_square != NULL) {
		if (piece_in_that_square->piecetype == KING && landed_on_king != NULL) {
			*landed_on_king = true;
			return false;
		}
		return (piece_in_that_square->team != team);
	};
	return true;
}

bool King::am_in_check(Board *main_board) {
	//here
	Piece* piece_to_check;
	for (int row = 1; row <= 8; row++) {
		for (int column = 1; column <= 8; column++) {
			piece_to_check = main_board->spaces[row - 1][column - 1];
			if (piece_to_check != NULL) {
				if (piece_to_check->team != this->team) {
					if (piece_to_check->can_classmove(column, row, main_board)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

void King::king_setup(COLOR b_team) {
	be_safe(b_team);
	count = 0;
	int b_row = 1;
	strcpy_s(chess_class, "King");
	piecetype = KING;
	sprintf_s(name, "%c%s", b_team, chess_class);
	if (b_team == BLACK) {
		b_row = 8;
	}
	else if (b_team == WHITE) {
		b_row = 1;
	}
	column = 5;
	setup(chess_class, b_team, b_row, 5, 0, KING);
	set_up_full_name(chess_class);
}