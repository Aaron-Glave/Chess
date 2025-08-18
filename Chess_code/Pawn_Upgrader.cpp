#include "Team.h"
#include <stdio.h>
#include <string.h>
#include <tuple>
#include "ctype.h"
#include "Pawn_Upgrader.h"
#include "Board.h"
#include "InvalidPiece.h"
#include "SpacelessChessInput.h"
#pragma warning(disable:4996)
bool is_pawn(Piece* shouldbepawn) {
    return shouldbepawn->piecetype == TYPE::PAWN;
}
//NOTE: This function is called EVERY time a pawn is moved, and it does nothing is the pawn is not on it.
// It returns EMPTY if the pawn can't be upgraded.
//Possible errors: InvalidPiece
TYPE upgrade_pawn_if_needed(Pawn* to_upgrade, Team* team_owner, Board* mainboard, TYPE upgradeTo) {
    if (upgradeTo == TYPE::PAWN) {
        std::string error = std::string("Pawn");
        throw InvalidPiece(error);
    }
    else if (upgradeTo == TYPE::KING) {
        std::string error = std::string("King");
        throw InvalidPiece(error);
    }
    //Other classes are playable. Like Queen.
    //const char* classes[] = { "Rook",  "Bishop", "Knight", "Queen" };
    if (!is_pawn(to_upgrade)) {
        return TYPE::EMPTY;
    }
    
    /*NOTE: Pawns are in posisitions 2:1-8,
            which means the team treats them as pieces 8-15*/
    int final_pawn_row = 8;
    if (to_upgrade->team == COLOR::BLACK) {
        final_pawn_row = 1;
    }
    //Make sure the pawn is on the last row.
    if (to_upgrade->row != final_pawn_row) {
        return TYPE::EMPTY;
    }
    //SELECT THE PIECE BASED ON USER INPUT
    if (upgradeTo == TYPE::EMPTY) {
        upgradeTo = get_valid_upgrade_type();
    }
    really_perform_upgrade(to_upgrade, upgradeTo, team_owner, mainboard);
    return upgradeTo;
}

TYPE get_valid_upgrade_type() {
    TYPE to_upgrade_to = TYPE::EMPTY;
    bool valid_type = false;
    char typewanted[10] = "";
    for (int i = 0; i < 10; i++) {
        typewanted[i] = '\0';
    }
    while (!valid_type) {
        printf("What kind of piece do you want? ");
        get_with_number_of_chars_including_null(typewanted, 10);
        typewanted[0] = toupper(typewanted[0]);
        for (int i = 1; i < 10; i++) {
            typewanted[i] = tolower(typewanted[i]);
        }
        if (strcmp(typewanted, "Rook") == 0) {
            to_upgrade_to = TYPE::ROOK;
            valid_type = true;
        }
        else if (strcmp(typewanted, "Knight") == 0) {
            to_upgrade_to = TYPE::KNIGHT;
            valid_type = true;
        }
        else if (strcmp(typewanted, "Bishop") == 0) {
            to_upgrade_to = TYPE::BISHOP;
            valid_type = true;
        }
        else if (strcmp(typewanted, "Queen") == 0) {
            to_upgrade_to = TYPE::QUEEN;
            valid_type = true;
        }
        // */
        if (!valid_type) {
            printf("%s is not a valid type to upgrade a pawn to. Try again.\n", typewanted);
        }
    }
    return to_upgrade_to;
}

void place_upgraded_piece(Team* team_owner, Pawn* pawn_i_was, const char *newpiece_type, Piece* upgraded_piece, Board* mainboard) {
    //TODO: UPGRADED PIECES ARE PLACED IN THE WRONG SPOTS IN MEMORY. We get the CREDIT: 
    //Dad reminded me that I can tell what type of piece a Piece pointer points to based on it class,
    //so I don't need a different array for each possible piece type.
    pawn_i_was->alive = false;
    //This may not be needed, but it is safe.
    upgraded_piece->alive = true;
    char column_name = '0';
    if (pawn_i_was->team == COLOR::BLACK) {
        column_name += 9 - pawn_i_was->get_start_column();
    }
    else {
        column_name = column_name + pawn_i_was->get_start_column();
    }
    
    // Assume we're the white team.
    int piecenum = pawn_i_was->get_start_column() + 7;
    char my_column_name = '0';
    Piece* new_piece = team_owner->pieces[piecenum];
    /*THE COLUMNS NEED TO BE RE-CALCULATED FOR THE BLACK TEAM!
    * Their pieces are not in the same order as the white team.
      team_owner[piecenum] needs to be adjusted for black pieces .*/
    if (new_piece->team == COLOR::BLACK) {
        piecenum = (9 - pawn_i_was->get_start_column()) + 7;
        my_column_name += 9 - pawn_i_was->get_start_column();
    }
    else {
        my_column_name += + pawn_i_was->get_start_column();
    }
    sprintf(upgraded_piece->name, "%c%sp%c", pawn_i_was->team, newpiece_type, my_column_name);
    // VERY IMPORTANT NOTE: THIS IS WHERE THE UPGRADED PAWN IS ADDED TO THE TEAM!
    
    
    team_owner->pieces[piecenum] = upgraded_piece;
    mainboard->place(upgraded_piece, upgraded_piece->row, upgraded_piece->column);
}

//VERY IMPORTANT NOTE: I CREATE A NEW PIECE, SO IT MUST BE DELETED WHEN THE MOVE IS UNDONE!
//Calculate the index so that PIECES ARE PLACED IN THE PROPER SPOT. 
void really_perform_upgrade(Pawn* to_upgrade, TYPE new_class, Team* team_owner, Board* mainboard) {
    for (int i = 8; i < 16; i++) {
        if (team_owner->pieces[i] == to_upgrade) {
            int upgraded_index = to_upgrade->get_start_column() - 1;
            switch (new_class)
            {
                //NOTE: WE PLACE THE UPGRADED PAWNS BASED ON THEIR STARTING COLUMN!
            case TYPE::BISHOP:
                team_owner->upgraded_pieces[upgraded_index] = new Bishop(to_upgrade->team, to_upgrade->row, to_upgrade->column, to_upgrade->count);
                place_upgraded_piece(team_owner, to_upgrade, "Bishop", team_owner->upgraded_pieces[upgraded_index], mainboard);
                return;
                break;

            case TYPE::KNIGHT:
                team_owner->upgraded_pieces[upgraded_index] = new Knight(to_upgrade->team, to_upgrade->row, to_upgrade->column, to_upgrade->count);
                place_upgraded_piece(team_owner, to_upgrade, "Knight", team_owner->upgraded_pieces[upgraded_index], mainboard);
                return;
                break;

            case TYPE::ROOK:
                team_owner->upgraded_pieces[upgraded_index] = new Rook(to_upgrade->team, to_upgrade->row, to_upgrade->column, to_upgrade->count);
                place_upgraded_piece(team_owner, to_upgrade, "Rook", team_owner->upgraded_pieces[upgraded_index], mainboard);
                return;
                break;
            case TYPE::QUEEN:
                team_owner->upgraded_pieces[upgraded_index] = new Queen(to_upgrade->team, to_upgrade->row, to_upgrade->column, to_upgrade->count);
                place_upgraded_piece(team_owner, to_upgrade, "Queen", team_owner->upgraded_pieces[upgraded_index], mainboard);
                return;
                break;
            default:
                break;
            }
        }
    }
}