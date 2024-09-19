#pragma once
#include "Piece.h"
//Reminder: Always say the row BEFORE the column.
class Rook :
    public Piece
{
public:
    Rook(COLOR b_team, int b_row, int b_column, int b_count);
    //Problem: This construcur is not defined
    Rook();
    virtual void setup(COLOR b_team, int b_row, int b_column, int b_count);
    bool can_classmove(int b_row, int b_column, Board *board_to_move_on);
};
