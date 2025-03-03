#pragma once
#pragma warning(disable:4996)
#include <string.h>
#include "Piece.h"
#include "Board.h"
#include <stdio.h>
#include <stdlib.h>
#include "Team.h"
#include "Safety.h"
#include "Check_vs_Checkmate.h"
#include "Pawn_Upgrader.h"
#include "Node.h"
#include "Move.h"
//#include "Undo_move.h"
#include <windows.h>
#include <ctime>
void tellmeaspace(Piece* piecetomove) {
    if (piecetomove->alive) {
        (*piecetomove).sayspace();
    }
}

void saybadmove() {
    printf("Illegal move!\n");
}
//Column always comes before road.
void say(int column, int row, Board mainboard) {
    if (mainboard.spaces[row][column] != NULL) {
        printf("%s\n", mainboard.spaces[row][column]->name);
    }

}

/*TYPE move_pawn(Pawn* pawn_to_move, Board* mainboard, Team* current_team) {
    //TODO Upgrade the pawn if necessary.
    return PAWN;
}*/

COLOR char_opposing_team(COLOR team) {
    if (team == WHITE) return BLACK;
    else return WHITE;
}

void say_pieces_of_team(Board* mainboard, COLOR current_team) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (mainboard->spaces[i][j] != NULL) {
                if (mainboard->spaces[i][j]->team == current_team)
                {
                    printf_s("%s on row %d row %d\n", mainboard->spaces[i][j]->name, mainboard->spaces[i][j]->row, mainboard->spaces[i][j]->column);
                }
            }
        }
    }
}

bool wrong_team(space piecetomove, COLOR current_team) {
    if (piecetomove == NULL) {
        return false;
    }
    if (piecetomove->team != current_team) {
        return true;
    }
    return false;
}

COLOR enemy_team(COLOR my_team) {
    if (my_team == WHITE) {
        return BLACK;
    }
    else
    {
        return WHITE;
    }
}

//Not sure this function is actually called.
/*
void do_move(Team** current_team, Board* mainboard, Move* move, Team* public_white_team, Team *public_black_team, int *move_count, Game_Status *new_status) {
    //We check that we are on the right team before we move
    if ((*current_team)->color == move->piece_that_moved->team) {
        Piece* piecelandedon = mainboard->spaces[move->end_row - 1][move->end_column - 1];
        int old_row = move->piece_that_moved->row;
        int old_column = move->piece_that_moved->column;
        printf("Used to be on row %d, column %d.\n", move->piece_that_moved->row, move->piece_that_moved->column);
        //This function basically always returns true. At least it should.
        if (mainboard->human_move_piece(move)) {
            printf("Valid move.\n");
            //Check for check;
            Game_Status still_in_check = mainboard->is_in_check(*current_team, ((*current_team)->enemy_team), mainboard);
            if (still_in_check != NEUTRAL) {
                printf("Invalid move. You are entering Check.\nI guess you're A.OK letting your opponet win.\n");
                //TODO: Re-write undoing this move and return.
                //UNDO_MOVE8
            }
          
            if (*current_team == public_white_team) {
                *current_team = public_black_team;
            }
            else {
                *current_team = public_white_team;
            }
        }
    }
}
*/
bool check_piece(Board* mainboard, Piece** piecetomove, int row, int column, char nameofpiecetomove[]) {
    piecetomove = &(mainboard->spaces[row][column]);
    for (int space_row = 0; space_row < 8; space_row++)
    {
        for (int space_column = 0; space_column < 8; space_column++) {
            if (mainboard->spaces[space_row][space_column] == NULL) {
                continue;
            }
            if (strcmp(nameofpiecetomove, mainboard->spaces[space_row][space_column]->name) != 0) {
                continue;
            }
            if (strcmp(nameofpiecetomove, mainboard->spaces[space_row][space_column]->name) == 0) {
                return true;
            }
        }
    }

    //If you get to this line, the piece you are looking for is not on the board..
    return false;
}

const char* team_name(COLOR team) {
    if (team == WHITE) {
        return "White";
    }
    else return "Black";
}