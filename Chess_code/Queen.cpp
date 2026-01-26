#pragma once
#define _CRT_SECURE_NO_WARNINGS //prevents warnings for using non-Microsoft functions
#pragma warning(disable:4996)
#include "Queen.h"
#include "move_diagnolly.h"
#include "move_horizontally.h"
//Reminder: Always say the row BEFORE the column.
bool Queen::can_classmove(int b_row, int b_column, Board* board_to_move_on) {
    bool can_move = false;
    DiagMove diagmove = get_direction(this, b_row, b_column);
    can_move |= can_move_diagnolly(this, diagmove.direction, diagmove.distance, board_to_move_on);
    can_move |= move_horizontally(this, b_row, b_column, board_to_move_on);
    return can_move;
}

Queen::Queen() {
    be_safe(COLOR::WHITE);
}

Queen::Queen(COLOR b_team, int b_row, int b_column, int b_count) {
    setup(b_team, b_row, b_column, 0, TYPE::QUEEN);
}