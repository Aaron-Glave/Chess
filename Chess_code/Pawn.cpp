#include "Pawn.h"
#include "Board.h"
#include <string.h>
#include "Move_safety.h"
#define _CRT_SECURE_NO_WARNINGS //prevents warnings for using non-Microsoft functions
#pragma warning(disable:4996)

//Aaron has helped and Andrew can still edit.

Pawn::Pawn() {
    starting_column = -1;
    be_safe(COLOR::WHITE);
}

int Pawn::get_start_column()
{
    return starting_column;
}

Pawn::Pawn(COLOR b_team, int b_row, int b_column, int b_count) {
    starting_column = b_column;
    setup(b_team, b_row, b_column, b_count, TYPE::PAWN);
    name[5] = column_name();
}

//Changes the way the piece is named visually but NOT functionally!
char Pawn::column_name() {
    if (team == COLOR::BLACK) {
        return '0' + 9 - column;
    }
    else {
        return '0' + column;
    }
}

//
bool Pawn::can_classmove(int b_row, int b_column, Board* main_board) {
    //It is possible to go up 2 spaces if in your starting position.
    //I MADE PASSANT POSSIBLE!
    if (!is_safe(this, main_board, b_row, b_column)) return false;
    int direction = 1;
    if (team == COLOR::BLACK) {
        direction = -1;
    }
    bool cango1up = !main_board->does_have_any_piece(row + direction, column);
    if (row + direction == b_row && column == b_column) {
        //Moving straight up 1.
        return cango1up && !main_board->does_have_any_piece(b_row, b_column);
    }
    if (
        ((team == COLOR::WHITE && row == 2) || (team == COLOR::BLACK && row == 7))
        && (b_column == column)
    ) {
        if (cango1up &&
            !main_board->does_have_any_piece(row + 2 * direction, column))
        {
            if (b_row == row + 2 * direction) {
                //I can be hit here thanks to the en passant rule.
                return true;
            }
        }
    }
    //Assures are moving eactly 1 space
    if (row + direction != b_row) return false;
    //Check for passant.
    if ((b_column == column - 1) || (b_column == column + 1)) {
        if (main_board->passantpawn.get_piece() != NULL) {
            if (main_board->passantpawn.get_row() == b_row && main_board->passantpawn.get_column() == b_column) {
                return true;
            }
        }
    }
    if ((b_column == column - 1) || (b_column == column + 1)) {
        return (main_board->does_have_any_piece(b_row, b_column) && main_board->no_ally_there(team, b_row, b_column));
    }
    return false;
}