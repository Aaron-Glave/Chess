#pragma once
#include "Teamname.h"
class Board;
enum class TYPE { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING,
    //Used to represent invalid pieces to promote.
    EMPTY };

//The longest class name is "Bishop" which is 6 characters +1 for null character.
#define CLASS_NAME_LENGTH 7

/* The name of a piece is:
1 character for team + up to 6 characters for the class name,
plus up to 2 characters to represent an upgraded pawn ('p' + column number),
plus 1 for null character.
// */
#define PIECE_NAME_LENGTH 10

//NOTE: Subclasses must not have any additional member variables, only functions.
class Piece
{
protected:
    int first_turn;
    int starting_column;
public: // public variables
    //The longest class name is "Bishop" which is 6 characters +1 for null character.
    char chess_class[CLASS_NAME_LENGTH];
    //Printed when the board is printed.
    // Important: See PIECE_NAME_LENGTH for the length of a name.
    char name[PIECE_NAME_LENGTH];

    COLOR team;
    TYPE piecetype;
    //Important. This decides whether or not the piece should stay on the board.
    bool alive;
    int count;
    int row;
    int column;
    
public: // public functions
    
    Piece();
    Piece(COLOR color);
    Piece(Piece* clone);
    static const char* get_type_name(TYPE piecetype);

    /* Used to name the piece.
     * b_row and b_column range from 1 to 8 because they are displayed to the user.*/
    void setup(COLOR b_team, int b_row, int b_column, int b_count, TYPE b_piecetype);

    //Useful for testing.
    int first_turn_i_moved() const;
    void know_i_moved(int turn_i_moved_on);
    char team_character() const;
    bool has_moved() const;
    
    //b_row and b_column are 1 at minimun.
    //This function should not edit the state!!
    virtual bool can_classmove(int b_row, int b_column, Board *board_to_move_on);
    
    bool do_team_match(Piece* team_there) const;
    void be_safe(COLOR color);
    void sayspace() const;
    void know_i_change_position(int b_row, int b_column, int on_turn);
    void setspace(COLOR b_team, int b_row, int b_column, int b_count);
    void set_up_full_name();
    bool is_on_board(int b_row, int b_column);
    bool same_team(COLOR piece_team) const;
    void place(int b_row, int b_column);

    //Starting column is protected, but we still need read access.
    int get_starting_column();

    //Set your data to equal that of the passed argument.
    void AssignSavedData(Piece* pPc);
};

