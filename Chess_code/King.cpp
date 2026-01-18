#include "King.h"
#include "Piece.h"
#include "Teamname.h"
#include "Board.h"
#include "Move_safety.h"
#include <string.h>
#include <stdio.h>


King::King() {
    king_setup(COLOR::WHITE);
}
King::King(COLOR b_team)
{
    king_setup(b_team);
}

bool King::is_adjacent(int b_row, int b_column, bool test = false) {
    if ((b_row >= row - 1) && (b_row <= row + 1)) {
        if ((b_column >= column - 1) && (b_column <= column + 1)) return true;
    }
    return false;
}

bool King::can_classmove(int b_row, int b_column, Board* mainboard) {
    return can_kingmove(b_row, b_column, mainboard);
}

bool King::can_kingmove(int b_row, int b_column, Board *main_board) {
    if (!is_safe(this, main_board, b_row, b_column)) return false;
    return is_adjacent(b_row, b_column);
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
    piecetype = TYPE::KING;
    sprintf_s(name, "%c%s", b_team, chess_class);
    if (b_team == COLOR::BLACK) {
        b_row = 8;
    }
    else if (b_team == COLOR::WHITE) {
        b_row = 1;
    }
    setup(b_team, b_row, 5, 0, TYPE::KING);
}
