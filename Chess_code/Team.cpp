/*
Reminder: The black perspective is facing the other way, so their first rook
from their perspective is on the far RIGHT.
/*/ 
#include "Knight.h"
#include "King.h"
#include "Queen.h"
#include "Team.h"
#include "Teamname.h"
#include "Piece.h"
#include "Board.h"
#include "Rook.h"
#include "Check_vs_Checkmate.h"
#include "Pawn_Upgrader.h"
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS //prevents warnings for using non-Microsoft functions
#pragma warning(disable:4996)
//private
void Team::set_upgraded_pointers_to0() {
    for (int i = 0; i < 8; i++) {
        upgraded_pieces[i] = NULL;
    }
}

const char* Team::team_name() const {
    if (color == COLOR::WHITE) {
        return "White";
    }
    else if (color == COLOR::BLACK) {
        return "Black";
    }
    else {
        return "Unknown";
    }
}

//public
Team::Team() {
    enemy_team = nullptr;
    empty_spaces();
}

Team::~Team() {
    //NOTE: DELETE UPGRADED QUEENS HERE
    for (int i = 0; i < 8; i++) {
        if (upgraded_pieces[i] != NULL) {
            delete upgraded_pieces[i];
        }
    }
}

Team::Team(COLOR team_color, Board *the_board_shared) :the_king(COLOR::WHITE) /* Sets up the team on the board.
 * The team_color is the color of the team, either WHITE or BLACK.
 * the_board_shared is a pointer to the board on which the game will be played.
 * The black team looks at the board facing the opposite direction that the white team does,
 * so at the start of the game,
 * the white team's pawn n is the black team's pawn 9-n, where n is the visible pawn number, NOT the index.
 * NOTE: With i as the visible column number,
 *       the index in the white team's pawn array of the pawn that starts in column i is i-1,
 *       and the index in the black team's pawn array of the pawn that starts in column i is 8-i.
 * In general, the piece's variable names are set up from the top-down perspective, with the white team at the bottom and the black team on top.
 *    The name of a variable representing a piece is NOT the name displayed to players.
 *    For example, blackteam.bishop1.name is "bBishop2". Sorry for the confusion.
 */

{
    //NOTE THE QUEEN AND KING SHOULD SWAP
    enemy_team = nullptr;
    empty_spaces();
    color = team_color;
    current_status = Game_Status::NEUTRAL;
    if (team_color == COLOR::WHITE) {
        namefirstletter = 'w';
        strcpy(full_name, "White");
        empty_spaces();
        knight1 = Knight(team_color, 1, 2, 1);
        knight2 = Knight(team_color, 1, 7, 2);
        rook1 = Rook(team_color, 1, 1, 1);
        rook2 = Rook(team_color, 1, 8, 2);
        bishop1 = Bishop(team_color, 1, 3, 1);
        bishop2 = Bishop(team_color, 1, 6, 2);
        queen = Queen(team_color, 1, 4, 1);
        for (int i = 1; i <= 8; i++) {
            pawns[i-1] = Pawn(team_color, 2, i, i);
        }
    }
    else //team_color == BLACK
    {
        //NOTE: On the black team, the variables in code are named from the TOP DOWN perspective.
        // The pawn indexes are set up the other way, with the 0th pawn being the top right black pawn.
        //  But their displayed names are from the black team's perspective.
        //  So are the the pawn *indexes*.
        //  That means, for example, the black rook starting on the top right corner is displayed as bRook2 even though the variable is rook1,
        //  and the black pawn beneath it is displayed as bPawn8 and the variable is pawns[7].
        namefirstletter = 'b';
        strcpy(full_name, "Black");
        empty_spaces();
        knight1 = Knight(team_color, 8, 2, 2);
        knight2 = Knight(team_color, 8, 7, 1);
        rook1 = Rook(team_color, 8, 1, 2);
        rook2 = Rook(team_color, 8, 8, 1);
        bishop1 = Bishop(team_color, 8, 3, 2);
        bishop2 = Bishop(team_color, 8, 6, 1);
        queen = Queen(team_color, 8, 4, 0);
        for (int i = 1; i <= 8; i++) {
            /*The black king looks at his pawns from the opposite angle, so he calls the
            * pawn in the absolute top left pawn corner is pawn 8. That means pawns[8-1], (or in other words pawns[7]) is in row 7 column 1.
            * I know it's kinda confusing that the pawns are created from the team's perspective while the other pieces are created from the top-down perspective,
            * but to make it easier, just think of the pawn's index as the 8 - the pawn's column.
              */
            pawns[8-i] = Pawn(team_color, 7, i, i);
        }
    }
    pieces[0] = &rook1;
    pieces[1] = &knight1;
    pieces[2] = &bishop1;
    /*Sorry.Earlier when I was making this program, I incorrectly thought that the king was in column 4 at the start of the game,
    * and I wrote a lot of code tests making that impression, and then I figured out that the queen was on the king's LEFT side.
    * So instead of re-writing those tests, I just don't have the queen or king's indexes match their starting column.*/
    the_king = King(team_color);
    pieces[3] = &the_king;
    //Queen goes here.
    pieces[4] = &queen;
    pieces[5] = &bishop2;
    pieces[6] = &knight2;
    pieces[7] = &rook2;
    for (int i = 0; i < 8; i++) {
        pieces[i + 8] = &pawns[i];
    }

    // Now put the pieces on our team on the board.
    put_pieces_on_board(the_board_shared);
}
void Team::empty_spaces() {
    set_upgraded_pointers_to0();
    for (int i = 0; i < 16; i++) {
        pieces[i] = 0;
    }
}

void Team::put_pieces_on_board(Board* the_board_shared) {
    for (int i = 0; i < 16; i++) {
        if (pieces[i] != 0) {
            the_board_shared->spaces[pieces[i]->row - 1][pieces[i]->column - 1] = pieces[i];
        }
    }
}
