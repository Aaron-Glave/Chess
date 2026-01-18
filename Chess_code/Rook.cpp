#include "Rook.h"
#include "Board.h"
#include "Move_horizontally.h"
#include "Piece.h"
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS //prevents warnings for using non-Microsoft functions
#pragma warning(disable:4996)

Rook::Rook() {
    be_safe(COLOR::WHITE);
}

Rook::Rook(COLOR b_team, int b_row, int b_column, int b_count) {
    setup(b_team, b_row, b_column, b_count, TYPE::ROOK);
}

//The letter b means I'm gonna BE here.
bool Rook::can_classmove(int b_row, int b_column, Board* board_to_move_on) {
    return move_horizontally(this, b_row, b_column, board_to_move_on);
}