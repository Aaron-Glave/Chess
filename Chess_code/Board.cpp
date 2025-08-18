#include "Piece.h"
#include "Knight.h"
#include "Board.h"

#include <iostream>
#include "Safety.h"
#include "Check_vs_Checkmate.h"
#include "CastleMove.h"
#include "Pawn_Upgrader.h"
#include "InvalidPiece.h"
#include <string>
using namespace std;

Board::Board() {
    team_in_check = COLOR::NONE;
    turn_number = 1;
    whiteturn = true;
    threatens_white = NULL;
    threatens_black = NULL;
    previous_move = Move();
    //NOTE: The PassantPawn fields are automatically initialized in the PassantPawn constructor.

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            spaces[i][j] = NULL;
        }
    }
}

//Column and row both range from 1 to 8.
//The piece has to know where it was when this move happens
//Teleporting pieces is useful for testing.
//Note that this does NOT count as making a move.
//If there used to be another piece here, do NOT set its old space to NULL!
//The function undo_move in Board.cpp will revive the piece that was landed on BEFORE calling this.
void Board::place(Piece* piece, int row, int column, bool revivedinsameplace) {
    if (!revivedinsameplace) {
        spaces[piece->row - 1][piece->column - 1] = NULL;
    } 
    //spaces[piece->row - 1][piece->column - 1] = NULL;
    spaces[row - 1][column - 1] = piece;
    piece->row = row;
    piece->column = column;
}

bool Board::does_have_any_piece(int row, int column) const {
    return spaces[row - 1][column - 1] != NULL;
}

bool Board::no_ally_there(COLOR my_team, int row, int column) const {
    if (spaces[row - 1][column - 1] != NULL) {
        return spaces[row - 1][column - 1]->team != my_team;
    }
    else {
        return true;
    }
}

int Board::get_turn()
{
    return turn_number;
}

void Board::next_turn()
{
    turn_number++;
}

void Board::set_turn(int turn) {
    turn_number = turn;
}

//Make sure the correct argument is passed each time!
Game_Status Board::is_in_check(Team* my_team, Team* enemy_team, bool look_for_checkmate) {
    bool is_hugging_allowed = false;
    bool is_in_check = false;
    for (int i = 0; i < 16; i++) {
        if (enemy_team->pieces[i] != 0) {
            //Failing around here?
            if (enemy_team->pieces[i]->can_classmove(my_team->the_king.row, my_team->the_king.column, this)) {
                //Intentionally allow Kings to risk their lives
                is_in_check = true;
                break;
                
            }
        }
    }
    if (is_in_check) {
        if(look_for_checkmate) {
            return try_to_escape(my_team, enemy_team, this);
        }
        else {
            return Game_Status::CHECK;
        }
    }
    return Game_Status::NEUTRAL;
}

Game_Status Board::try_to_escape(Team* my_team, Team* enemy_team, Board* mainboard) {
    //See if the player currently in check can make any move that gets them out of check.
    Piece* one_of_my_pieces;
    Move tried_move;
    Game_Status after_trying_escape = Game_Status::CHECK;
    for (int i = 0; i < 16; i++)
    {
        one_of_my_pieces = my_team->pieces[i];
        if (!one_of_my_pieces->alive) continue;
        tried_move.piece_that_moved = my_team->pieces[i];
        tried_move.start_row = my_team->pieces[i]->row;
        tried_move.start_column = my_team->pieces[i]->column;
        for (int tryrow = 1; tryrow <= 8; tryrow++) {
            tried_move.end_row = tryrow;
            for (int trycolumn = 1; trycolumn <= 8; trycolumn++) {
                tried_move.end_column = trycolumn;
                // We know we can go here, so we might as well try.
                //TODO CHECK THAT YOU REALLY CAN MOVE HERE!
                if (tried_move.piece_that_moved->can_classmove(tryrow, trycolumn, this)) {
                    bool did_i_move = human_move_piece(&tried_move);
                    //No matter whether we're in check or not, this move still has to be undone.
                    after_trying_escape = is_in_check(my_team, enemy_team, false);
                    if (did_i_move) {
                        undo_move(&tried_move, my_team);
                    }
                    
                    if (after_trying_escape == Game_Status::NEUTRAL) {
                        return Game_Status::CHECK;
                    }
                }
            }
        }
        return Game_Status::CHECKMATE;
    }

    /* 
    //Check upgraded pieces too.
    for (int i = 0; i < 8; i++) {
        if (my_team->upgraded_pieces[i] != NULL) {
            one_of_my_pieces = my_team->upgraded_pieces[i];
            if (!one_of_my_pieces->alive) {
                continue;
            }
            tried_move.piece_that_moved = one_of_my_pieces;
            tried_move.start_row = one_of_my_pieces->row;
            tried_move.start_column = one_of_my_pieces->column;
            for (int tryrow = 1; tryrow <= 8; tryrow++) {
                tried_move.end_row = tryrow;
                for (int trycolumn = 1; trycolumn <= 8; trycolumn++) {
                    tried_move.end_column = trycolumn;
                    // We know we can go here, so we might as well try.
                    human_move_piece(&tried_move);
                    if (is_in_check(my_team, enemy_team) == Game_Status::NEUTRAL) {
                        undo_move(&tried_move, my_team);
                        return Game_Status::CHECKMATE;
                    }
                }
            }
        }
    }
    //Maybe that isn't neeeded. */

    //Here I know the move that saved me and I haven't actually made it on purpose yet,
    // so I need to undo the move before exiting the loop.
    undo_move(&tried_move, my_team);
    // END OF IMAGINARY LOOP
    return Game_Status::CHECKMATE;
};

bool Board::is_on_board(int b_row, int b_column) {
    if (b_row < 1 || b_column < 1 || b_row > 8 || b_column > 8) {
        return false;
    }
    return true;
}
//NOTE: This is only used in tests?
//It had better not change any state.
Move Board::make_move(Piece* piece_that_moved, int erow, int ecolumn) {
    if (piece_that_moved == NULL) {
        throw InvalidPiece(piece_that_moved);
    }
    //Pawn* passantpawnpiece = NULL;
    int passantrow = -1;
    int passantcolumn = -1;
    
    Move nextmove = Move(piece_that_moved->row, piece_that_moved->column, erow, ecolumn, 
        piece_that_moved, spaces[erow - 1][ecolumn - 1],
        false);
    return nextmove;
}

/*
b_column and b_row range from 1 to 8. We subtract 1 whenever we reference a space on the board.
That's because a person starts counting spaces with 1 but the computer starts counting with 0.
Detects if the move lands on an enemy piece and stores the enemy piece in the move in case it is un-done.

If pawn moves 2, I save a Piece pointer to the pawn and the spot that it would have
been in if it moved 1 in a special PassantPawn object. I call that object SS in this example.
If the opponent lands on SS THE EXACT TURN AFTER the pawn moved, the pawn dies.
That means killing the pawn and setting SS's pointer to NULL after making a move on the opponent's turn.
If an en passant move is made, the passant status of the killed pawn is saved in the prevepassant variable,
so that it can be revived if the move is undone.

Note that this function DOESN'T UPGRADE Pawns. The function to upgrade pawns is called upgrade_pawn_if_needed,
and it's called after this function is called in Chess.cpp.
*/
bool Board::human_move_piece(Move* move_to_make) {
    if (move_to_make == NULL) {
        throw InvalidPiece(NULL);
    }
    space piece = move_to_make->piece_that_moved;
    int b_row = move_to_make->end_row;
    int b_column = move_to_make->end_column;
    bool changepast = false;
    CastleMove* castlemove = dynamic_cast<CastleMove*>(move_to_make);
    int row = 1;
    if (piece->team == COLOR::BLACK) {
        row = 8;
    }
    if (castlemove != NULL) {
        switch (castlemove->which_side_you_castled)
        {
        case CastleDirection::LEFT:
            for (int column = 4; column >= 2; column--) {
                if (spaces[row - 1][column - 1] != NULL) {
                    throw InvalidMove("Can't castle. There's a piece in the way.");
                }
            }
            place(castlemove->rook_that_moved, castlemove->rook_that_moved->row, 4);
            place(castlemove->piece_that_moved, castlemove->piece_that_moved->row, 3);
            break;
        case CastleDirection::RIGHT:
            for (int column = 6; column <= 7; column++) {
                if (spaces[row - 1][column - 1] != NULL) {
                    throw InvalidMove("Can't castle. There's a piece in the way.");
                }
            }
            place(castlemove->rook_that_moved, castlemove->rook_that_moved->row, 6);
            place(castlemove->piece_that_moved, castlemove->piece_that_moved->row, 7);
            break;
        default:
            break;
        }
        //NOTE: INCREMENT TURN NUMBER BEFORE RETURNING TRUE. ALWAYS.
        castlemove->piece_that_moved->know_i_moved(turn_number);
        castlemove->rook_that_moved->know_i_moved(turn_number);
        turn_number++;
        return true;
    }
    if (piece != NULL) {
        int p_column = piece->column;
        int p_row = piece->row;
        if (castlemove != NULL) {
            if (castlemove->piece_that_moved == NULL) {
                throw InvalidMove(string("Somehow the piece moving in your castle move was null. That shouldn't happen!"));
            }
            if (castlemove->piece_that_moved->piecetype != TYPE::KING) {
                throw InvalidMove(string(castlemove->piece_that_moved->name) + string(" is NOT your king!"));
            }
            if (castlemove->piece_that_moved->first_turn_i_moved() != -1) {
                throw InvalidMove(string("Too late to castle. You've already moved!"));
            }
            turn_number++;
            return true;
        }
        bool can_move = piece->can_classmove(b_row, b_column, this);
        if (move_to_make->piece_that_moved == NULL) {
            throw InvalidPiece(move_to_make->piece_that_moved);
        }
        if (can_move) {
            // If you landed on a piece on your team:
            if (piece->do_team_match(spaces[b_row - 1][b_column - 1])) {
                return false;
            }

            //Opposing team:
            if (spaces[b_row - 1][b_column - 1] != NULL) {
                /*NOTE: THE piece_landed_on variable CAN be null before this function was called.
                * Remember the piece I landed on in case this move has to be undone. */
                move_to_make->piece_landed_on = spaces[b_row - 1][b_column - 1];
                spaces[b_row - 1][b_column - 1]->alive = false;
                spaces[b_row - 1][b_column - 1] = piece;
                
            }

            //Check if an en passant was made.
            if (passantpawn.get_piece() != NULL) {
                if (b_row == passantpawn.get_row() && b_column == passantpawn.get_column() && passantpawn.get_piece()->team != piece->team) {
                    kill_passant();
                }
            }
            

            //Think about passantpieces moving through a pipe.
            //First, you forget your oldest memory.
            //Then your current memory gets older.
            //Then you make a new memory.
            if (prevepassant.get_piece() != NULL) {
                if (prevepassant.get_piece()->team == piece->team) {
                    prevepassant = PassantPawn();
                }
            }
            
            if (passantpawn.get_piece() != NULL) {
                if ((passantpawn.get_turn_made() < turn_number) && (passantpawn.get_piece() != NULL)) {
                    prevepassant = passantpawn;
                    passantpawn = PassantPawn();
                }
            }
            //Either way move the piece.
            piece->know_i_moved(turn_number);
            spaces[move_to_make->start_row - 1][move_to_make->start_column - 1] = NULL;
            place(piece, b_row, b_column);
            piece->know_i_change_position(b_row, b_column, turn_number);
            if (passantpawn.get_piece() != NULL) {
                //Apply passant if needed.
                if (piece->piecetype == TYPE::PAWN && b_row == passantpawn.get_row() && b_column == passantpawn.get_column() && piece->team != passantpawn.get_piece()->team) {
                    kill_passant();
                }
                else if (passantpawn.get_piece()->team != piece->team) {
                    prevepassant = passantpawn;
                    //Create a new instance of a readonly class to reset it.
                    passantpawn = PassantPawn();
                }
            }


            
            //Maybe the passant didn't happen.
            if (turn_number > passantpawn.get_turn_made() && passantpawn.get_piece() != NULL) {
                prevepassant = passantpawn;
                passantpawn = PassantPawn();
            }
            
            //Maybe we set one up for our opponent though.
            if (piece->piecetype == TYPE::PAWN) {
                if (piece->team == COLOR::WHITE && move_to_make->start_row == 2 && move_to_make->end_row == 4) {
                    passantpawn = PassantPawn((Pawn*)piece, 3, piece->column, turn_number);
                }
                else if (piece->team == COLOR::BLACK && move_to_make->start_row == 7 && move_to_make->end_row == 5) {
                    passantpawn = PassantPawn((Pawn*)piece, 6, piece->column, turn_number);
                }
            }

            //Now we know we're done moving.
            turn_number++;
            return true;
        }
        return false;
    }
    return false;
}

void Board::kill_passant() {
    if (passantpawn.get_piece() == NULL) {
        throw InvalidMove("No passant pawn to kill.");
    }
    passantpawn.get_piece()->alive = false;
    spaces[passantpawn.get_piece()->row - 1][passantpawn.get_piece()->column - 1] = NULL;
    //This is safe because doing a passant will NEVER be followed by a passant.
    //Save the deleted passant pawn in case an undo is made.
    prevepassant = passantpawn;
    passantpawn = PassantPawn();
}

int Board::current_turn() const
{
    return turn_number;
}

// IF UNDO You might have to downgrade a pawn.
// Check if the piece that moved was a pawn and if it moved to the end of the board.
// If i did then delete team_owner->upgraded_pieces[move_i_made->piece_that_moved->count-8]
// THEN SET IT TO NULL RIGHT AFTER!
// Note: IT IS VERY IMPORTANT IN THE LIVE GAME TO ALWAYS PASS THE TEAM THAT MOVED!
//BUG: Undoing an en passant does not reset the passant pawn.
void Board::undo_move(Move* move_i_made, Team* team_that_moved) {
    
    passantpawn = prevepassant;
    //Revive the passant pawn if he was killed.
    Pawn* last_passant = passantpawn.get_piece();
    if (last_passant != NULL) {
        last_passant->alive = true;
        spaces[last_passant->row - 1][last_passant->column - 1] = last_passant;
    }
    prevepassant = PassantPawn();
    int previousrow = move_i_made->end_row;
    int previouscolumn = move_i_made->end_column;
    Piece* piecethatmoved = move_i_made->piece_that_moved;
    if (move_i_made == NULL) {
        throw InvalidPiece(NULL);
    }

    //Undo upgrades here.
    if (move_i_made->piece_that_moved->piecetype == TYPE::PAWN) {
        Pawn* movedpawn = (Pawn*)move_i_made->piece_that_moved;
        bool did_upgrade = false;
        switch (move_i_made->piece_that_moved->team) {
        case COLOR::WHITE:
            did_upgrade = move_i_made->end_row == 8;
            break;
        
        case COLOR::BLACK:
            did_upgrade = move_i_made->end_row == 1;
            break;
        }

        //Undo the upgrade.
        if (team_that_moved != NULL && did_upgrade) {
            // Assume we're the white team.
            int piecenum = movedpawn->get_start_column() + 7;
            Piece* new_piece = team_that_moved->pieces[piecenum];
            /*THE COLUMNS NEED TO BE RE-CALCULATED FOR THE BLACK TEAM!
            * Their pieces are not in the same order as the white team.
              team_owner[piecenum] needs to be adjusted for black pieces .*/
            if (new_piece->team == COLOR::BLACK) {
                piecenum = (9 - movedpawn->get_start_column()) + 7;
            }
            if (team_that_moved->pieces[piecenum] != NULL) {
                if (team_that_moved->upgraded_pieces[movedpawn->get_start_column() - 1] != NULL) {
                    delete team_that_moved->upgraded_pieces[movedpawn->get_start_column() - 1];
                    team_that_moved->upgraded_pieces[movedpawn->get_start_column() - 1] = NULL;
                    team_that_moved->pieces[piecenum] = movedpawn;
                    team_that_moved->pieces[piecenum]->alive = true;
                }
            }
        }
    }

    CastleMove* castlemove = NULL;
    castlemove = dynamic_cast<CastleMove*>(move_i_made);
    
    if (castlemove != NULL) {
        /* No matter which side it's on if we know it castled we know it had'nt moved before this.
        * Reset the king too.*/
        castlemove->rook_that_moved->know_i_moved(-1);
        castlemove->piece_that_moved->know_i_moved(-1);
        switch (castlemove->which_side_you_castled)
        {
        case CastleDirection::LEFT:
            place(castlemove->rook_that_moved, castlemove->rook_that_moved->row, 1);
            break;
        case CastleDirection::RIGHT:
            place(castlemove->rook_that_moved, castlemove->rook_that_moved->row, 8);
            break;
        default:
            break;
        }
    }

    bool revivedinsameplace = false;
    if (move_i_made->piece_landed_on != NULL) {
        if ((move_i_made->end_row == move_i_made->piece_landed_on->row)
            && (move_i_made->end_column == move_i_made->piece_landed_on->column)) {
            revivedinsameplace = true;
        }
    };
    if (move_i_made->piece_landed_on != NULL) {
        move_i_made->piece_landed_on->alive = true;
        spaces[move_i_made->end_row - 1][move_i_made->end_column - 1] = move_i_made->piece_landed_on;
    }
    place(piecethatmoved, move_i_made->start_row, move_i_made->start_column, revivedinsameplace);
    piecethatmoved->row = move_i_made->start_row;
    piecethatmoved->column = move_i_made->start_column;
    turn_number--;
    if (move_i_made->piece_that_moved->first_turn_i_moved() >= turn_number) {
        move_i_made->piece_that_moved->know_i_moved(-1);
    }
}
static void print_piece(Piece *piece /*bool islast*/) {
    char piecename[11] = "          ";
    piecename[10] = '\0';
    if (piece != NULL) {
        int charofname = 0;
        for (; piece->name[charofname] != '\0' && charofname < 10; charofname++) {
            if (piece->name[charofname] == '\0') {
                break;
            }
            piecename[charofname] = piece->name[charofname];
        }
        while (charofname < 10) {
            piecename[charofname] = ' ';
            charofname++;
        }
        printf("%s|", piecename);
    }
    else printf("          |");
}

const static void print_columns() {
    printf("    Columns:\n");
    char columnname[11] = "          ";
    columnname[10] = '\0';
    printf("   |");
    for (int i = 1; i <= 8; i++) {
        columnname[0] = '0' + i;
        printf("%s|", columnname);
    }
    printf("\n");
}

/*Looks nice now.*/
void Board::print_board() const {
    const int length_of_name = 12;
    const int number_of_spaces = 8;
    bool firstcolumn = true;
    printf("Here's the board. Row numbers are printed on the left-hand side of each row.\n");
    print_columns();
    //printf("Rows:\n");
    for (int i = 0; i < 11 + length_of_name * (number_of_spaces - 1); i++) {
        printf("-");
    }
    printf("\n");
    
    //Top row.
    for (int row = 7; row >= 0; row--) {
        firstcolumn = true;
        if (row == 7 || row == 0) {
            printf("[]%d|", row+1);
        }

        else if (firstcolumn) {
            printf("  %d|", row+1);
        }
        else {
            printf("|    ");
        }
        for (int column = 0; column <= 7; column++) {
            if (firstcolumn && column != 0) {
                firstcolumn = false;
            }
            print_piece(spaces[row][column]);
        }
        if (row == 0 || row == 7) {
            printf("[]|");
        }
        else {
            printf("  |");
        }
        printf("\n");
        if (row > 0) {
            printf("   |");
            for (int i = 0; i < 4 + length_of_name * (number_of_spaces - 1); i++) {
                if ((i % (length_of_name-1)) != length_of_name-2/* != length_of_name - 1 - minus*/) {
                    printf("_");
                }
                else {
                    printf("|");
                }
            }
            printf("  |\n");
        }
        if (row == 0) {
            //
        }
    }
    //Bottom row
    for (int i = 0; i < 11 + length_of_name * (number_of_spaces - 1); i++) {
        printf("-");
    }
    printf("\n");
}

void Board::print_passant(bool* testprinted) {
    *testprinted = false;
    Pawn* passantpawnpiece = passantpawn.get_piece();
    if (passantpawnpiece != NULL) {
        printf("%s just jumped over row %d, column %d\n", passantpawnpiece->name, passantpawn.get_row(), passantpawn.get_column());
        *testprinted = true;
    }
}
