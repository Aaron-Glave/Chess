#pragma once
#include "Teamname.h"
class Board;
enum class TYPE { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, EMPTY };

//The longest name is 9 characters and the last character is \0.
#define PIECE_NAME_LENGTH 10

//NOTE: Subclasses must not have any additional member variables, only functions.
class Piece
{
protected:
    int first_turn;
    int starting_column;
public: // public variables
    COLOR team;
    char chess_class[8];
    //Printed when the board is printed. Important: Length of name.
    char name[PIECE_NAME_LENGTH];
    //Important. This decides whether or not the piece should stay on the board.
    bool alive;
    int count;
    int row;
    int column;
    TYPE piecetype;
    


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
    // void can_move(Board *pBoard);
    // void setupnullpiece();
    void sayspace() const;
    void know_i_change_position(int b_row, int b_column, int on_turn);
   // virtual void setup(char b_team, int b_column, int b_row, int b_count);
    void setspace(COLOR b_team, int b_row, int b_column, int b_count);
    void set_up_full_name();
    bool is_on_board(int b_row, int b_column);
    bool same_team(COLOR piece_team) const;
    void place(int b_row, int b_column);
    void AssignSavedData(Piece* pPc);
};

