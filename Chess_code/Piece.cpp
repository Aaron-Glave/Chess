#include "Piece.h"
#include <iostream>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS //prevents warnings for using non-Microsoft functions
#pragma warning(disable:4996)
using namespace std;
//*
Piece::Piece() {
    be_safe(COLOR::NONE);
}
// */

Piece::Piece(COLOR color) {
    be_safe(color);
}

Piece::Piece(Piece* piece_to_clone) {
    first_turn = -1;
    strcpy(chess_class, piece_to_clone->chess_class);
    piecetype = piece_to_clone->piecetype;
    team = piece_to_clone->team;
    strcpy(name, piece_to_clone->name);
    alive = true;
    count = piece_to_clone->count;
    row = piece_to_clone->row;
    column = piece_to_clone->column;
}

int Piece::first_turn_i_moved() const
{
    return first_turn;
}

void Piece::know_i_moved(int turn_i_moved_on)
{
    if(first_turn == -1) first_turn = turn_i_moved_on;
    if (turn_i_moved_on == -1) first_turn = -1;
}

void Piece::be_safe(COLOR color) {
    count = 0;
    row = -1;
    column = -1;
    team = color;
    piecetype = TYPE::EMPTY;
    alive = true;
    first_turn = -1;
    chess_class[7] = '\0';
    name[9] = '\0';
}

void Piece::setup(char* typeofpiece, COLOR b_team, int b_row, int b_column, int b_count, TYPE b_piecetype) {
    be_safe(b_team);
    name[0] = char(b_team);
    alive = true;
    piecetype = b_piecetype;
    setspace(b_team, b_row, b_column, b_count);
    strcpy(chess_class, typeofpiece);
    set_up_full_name(typeofpiece);
}

bool Piece::same_team(COLOR piece_team) const {
    return team == piece_team;
}

void Piece::setspace(COLOR b_team, int b_row, int b_column, int b_count) {
    team = b_team;
    row = b_row;
    column = b_column;
    count = b_count;
}

void Piece::place(int b_row, int b_column) {
    row = b_row;
    column = b_column;
}

char Piece::team_character() const {
    if (team == COLOR::WHITE) {
        return 'w';
    }
    if (team == COLOR::BLACK) {
        return 'b';
    }
    return '?';
}

bool Piece::has_moved() const {
    return first_turn != -1;
}

void Piece::know_i_change_position(int b_row, int b_column, int on_turn) {
    row = b_row;
    column = b_column;
    if (!has_moved()) {
        first_turn = on_turn;
    }
}

bool Piece::can_classmove(int b_row, int b_column, Board *board_to_move_on) {
    return false;
}
void Piece::sayspace() const {
    cout << name;
}

bool Piece::is_on_board(int b_row, int b_column) {
    if (b_row < 1 || b_column < 1 || b_row > 8 || b_column > 8) {
        return false;
    }
    return true;
}

void Piece::set_up_full_name(const char *nameofpiece) {
    if (count == 0) {
        sprintf(name, "%c%s", team, nameofpiece);
    }
    else {
        sprintf(name, "%c%s%d", team, nameofpiece, count);
    }
    bool name_ended = false;
    for (int char_name = 0; char_name < 10; char_name++) {
        if (name[char_name] == '\0') {
            name_ended = true;
        }
        if (name_ended) {
            name[char_name] = '\0';
        }
    }
    name[9] = '\0';
    // 0;
}

bool Piece::do_team_match(Piece* team_there) const {
    if (team_there == NULL)
    {
        return false;
    }
    if (team_there->team != team) {
        return false;
    }
    
    return true;
}

void Piece::AssignSavedData(Piece* pPc)
{
    alive = pPc->alive;
    count = pPc->count;
    row = pPc->row;
    column = pPc->column;
    //UPDATE FROM Aaron: Save the piece's knowledge of when it first moved.
    first_turn = pPc->first_turn;
}