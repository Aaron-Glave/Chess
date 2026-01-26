#include "Knight.h"
#include "Piece.h"
#include "Board.h"
#include <string.h>
#include "Move_safety.h"
#define _CRT_SECURE_NO_WARNINGS //prevents warnings for using non-Microsoft functions
#pragma warning(disable:4996)
Knight::Knight() {
    be_safe(COLOR::WHITE);
}

Knight::Knight(COLOR b_team, int b_row, int b_column, int b_count) {
    setup(b_team, b_row, b_column, b_count, TYPE::KNIGHT);
}

//Row should come BEFORE column. 
bool Knight::can_classmove(int b_row, int b_column, Board *main_board) {
    if (!is_safe(this, main_board, b_row, b_column)) return false;
    if (row == b_row + 1 || row == b_row - 1) {
        if (column == b_column + 2 || column == b_column - 2) {
            return true;
        }
    }

    if (row == b_row + 2 || row == b_row - 2) {
        if (column == b_column + 1 || column == b_column - 1) {
            return true;
        }
    }
    return false;
}