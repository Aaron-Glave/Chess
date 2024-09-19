#include "Piece.h"
#include <iostream>
#include <string.h>

using namespace std;
//*
Piece::Piece() {
    be_safe(NONE);
}
// */

Piece::Piece(COLOR color) {
    be_safe(color);
}

Piece::Piece(Piece* piece_to_clone) {
    count = piece_to_clone->count;
    row = piece_to_clone->row;
    column = piece_to_clone->column;
}



void Piece::be_safe(COLOR color) {
    count = 0;
    row = -1;
    column = -1;
    team = color;
    piecetype = EMPTY;
    alive = true;
    /*
    if (false) {
        for (int i = 0; i < 7; i++) {
            chess_class[i] = ' ';
            name[i] = ' ';
        }
    }
    */
    chess_class[7] = '\0';
    name[9] = '\0';
}

void Piece::setup(char* typeofpiece, COLOR b_team, int b_row, int b_column, int b_count, TYPE b_piecetype) {
    be_safe(b_team);
    team = b_team;
    name[0] = b_team;
    alive = true;
    piecetype = b_piecetype;
    setspace(b_team, b_row, b_column, b_count);
    strcpy_s(chess_class, typeofpiece);
    set_up_full_name(typeofpiece);
    
}

bool Piece::same_team(char piece_team) {
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

char Piece::team_character() {
    if (team == WHITE) {
        return 'w';
    }
    if (team == BLACK) {
        return 'b';
    }
    return '?';
}

void Piece::know_i_change_position(int b_row, int b_column) {
    row = b_row;
    column = b_column;
}

bool Piece::can_classmove(int b_row, int b_column, Board *board_to_move_on) {
    return false;
}
void Piece::sayspace() {
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
        sprintf_s(name, "%c%s", team, nameofpiece);
    }
    else {
        sprintf_s(name, "%c%s%d", team, nameofpiece, count);
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

bool Piece::do_team_match(Piece* team_there) {
    if (team_there == NULL)
    {
        return false;
    }
    if (team_there->team != team) {
        return false;
    }
    
    return true;
}