#include "Bishop.h"
#include "Board.h"
#include "Move_diagnolly.h"
#include "Piece.h"
#include <string.h>

#define _CRT_SECURE_NO_WARNINGS //prevents warnings for using non-Microsoft functions
#pragma warning(disable:4996)
Bishop::Bishop() {
    be_safe(COLOR::WHITE);
}

Bishop::Bishop(COLOR b_team, int b_row, int b_column, int b_count) {
    setup(b_team, b_row, b_column, b_count, TYPE::BISHOP);
}

bool Bishop::can_classmove(int b_row, int b_column, Board* board_to_move_on) {
    DiagMove diagmove = get_direction(this, b_row, b_column);
    return can_move_diagnolly(this, diagmove.direction, diagmove.distance, board_to_move_on);
}