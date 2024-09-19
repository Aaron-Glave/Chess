#include "Pawn.h"
#include "Board.h"
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS //prevents warnings for using non-Microsoft functions
#pragma warning(disable:4996)

Pawn::Pawn() {
	be_safe(WHITE);
}

Pawn::Pawn(COLOR b_team, int b_row, int b_column, int b_count) {
	strcpy(chess_class, "Pawn");
	setup(chess_class, b_team, b_row, b_column, b_count, PAWN);
}

bool Pawn::can_classmove(int b_row, int b_column, Board* mainboard) {
	int direction = 1;
	if (team == BLACK) {
		direction = -1;
	}
	if (row + direction != b_row) return false;
	bool can_move_diagonaly = false;
	//TODO figure this out later.
	return false;
}