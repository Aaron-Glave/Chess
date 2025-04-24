#pragma once
#include "Piece.h"
#include "Team.h"
#include <stdio.h>
#include "Check_vs_Checkmate.h"
#include "Move.h"
#include "PassantPawn.h"
#include <cstddef>

typedef Piece * space;
class Board
{
    //variables
private:
    int turn_number;
public:
    //Row then column
    Piece* spaces[8][8];
    COLOR team_in_check;
    
    Move previous_move;
    Piece* threatens_white;
    Piece* threatens_black;
    PassantPawn passantpawn;
    PassantPawn prevepassant;

    // int column;
    // int row;
    bool whiteturn;

    //functions
    int current_turn() const;
    void undo_move(Move* move_i_made, Team* team_that_moved = NULL);
    Move make_move(Piece* piece_that_moved, int erow, int ecolumn);
    bool human_move_piece(Move* move_to_make);
    void place(Piece* piece, int row, int column, bool revivedinsameplace = false);
    void kill_passant();
    void print_board() const;
    void print_passant(bool* testprinted = NULL);
    bool is_on_board(int b_row, int b_column);
    Board();
    Game_Status is_in_check(Team* my_team, Team* enemy_team, bool check_for_checkmate = true);
    Game_Status try_to_escape(Team* my_team, Team* enemy_team, Board* mainboard);
    bool does_have_any_piece(int row, int column) const;
    bool no_ally_there(COLOR my_team, int row, int column) const;
    int get_turn();
    void next_turn();
};

//bool do_team_dismatch(Piece* team_moving, Piece* team_there)