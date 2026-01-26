#pragma once
#include "Piece.h"

class Pawn : public Piece
{
public:
    int get_start_column();
    Pawn(COLOR b_team, int b_row, int b_column, int b_count);
    char relative_column_name();
    Pawn();
    bool can_classmove(int b_row, int b_column, Board* mainboard);
    static int index_to_column(COLOR pawn_team, int index);
    static int column_to_index(int column);
    //  virtual void setup(COLOR b_team, int b_row, int b_column, int b_count);
};
