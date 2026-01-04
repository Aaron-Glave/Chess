#pragma once
#pragma warning(disable:4996)
#include <string.h>
#include "Piece.h"
#include "Board.h"
#include <stdio.h>
#include <stdlib.h>
#include "Team.h"
#include "Safety.h"
//#include "Pawn_Upgrader.h"
#include "Move.h"
//#include "CastleMove.h"
//#include <windows.h>
#include <ctime>
void tellmeaspace(Piece* piecetomove) {
    if (piecetomove->alive) {
        (*piecetomove).sayspace();
    }
}

void saybadmove() {
    printf("Illegal move!\n");
}

//Column always comes before row.
void say(int column, int row, Board mainboard) {
    if (mainboard.spaces[row][column] != NULL) {
        printf("%s\n", mainboard.spaces[row][column]->name);
    }

}

COLOR char_opposing_team(COLOR team) {
    if (team == COLOR::WHITE) return COLOR::BLACK;
    else return COLOR::WHITE;
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
    if (my_team == COLOR::WHITE) {
        return COLOR::BLACK;
    }
    else
    {
        return COLOR::WHITE;
    }
}

//*
bool can_castle(Team* current_team, Board* mainboard, const char *direction_castle) {
    /*
    * Do something like this in the chess() function which should call this function:
    char direction_castle[6];
    scanf("%5s", direction_castle);
    direction_castle[0] = toupper(direction_castle[0]);
    for (int i = 1; i < 5; i++) {
        direction_castle[i] = tolower(direction_castle[i]);
    }
    Then assign the direction_castle to the direction_castle parameter.
    */
    clearinput();
    if (strcmp("Left", direction_castle) != 0 && strcmp("Right", direction_castle) != 0) {
        printf("Invalid direction.\n");
        return false;
    }
    if(strcmp("Left", direction_castle) == 0)

    //You can do this move.
    current_team->enemy_team->current_status = mainboard->is_in_check(current_team->enemy_team, current_team);
    return true;
}
// */
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
    if (team == COLOR::WHITE) {
        return "White";
    }
    else return "Black";
}

void kill_piece(Board* mainboard, Piece* piece) {
    piece->alive = false;
    mainboard->spaces[piece->row - 1][piece->column - 1] = NULL;
}
