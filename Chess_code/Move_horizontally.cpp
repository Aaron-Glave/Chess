#include "Piece.h"
#include "Board.h"
#include "Move_safety.h"
bool move_horizontally(Piece* valid_piece, int b_row, int b_column, Board* board_to_move_on) {
    if (!is_safe(valid_piece, board_to_move_on, b_row, b_column)) return false;
    if (valid_piece->row != b_row && valid_piece->column != b_column) return false;
    //Row up
    bool up = false;
    bool right = false;
    if (b_row != valid_piece->row) {
        //We're moving vertically.
        up = b_row > valid_piece->row;
        if (up) {
            //Row up
            //I need to stop once I hit a piece and I don't yet. This loop will never ever end the function with True.
            for (int u_row = valid_piece->row + 1; u_row < b_row; u_row++) {
                if (board_to_move_on->does_have_any_piece(u_row, b_column)) {
                    return false;
                }
            }
            return board_to_move_on->no_ally_there(valid_piece->team, b_row, b_column);
        }
        else {
            //Row down
            for (int d_row = valid_piece->row - 1; d_row > b_row; d_row--) {
                if (board_to_move_on->does_have_any_piece(d_row, b_column)) {
                    return false;
                }
            }
            return board_to_move_on->no_ally_there(valid_piece->team, b_row, b_column);
        }
    }
    else {
        //We're moving horizontally.
        right = b_column > valid_piece->column;
        if (right) {
            //Column to the right
            for (int rhead_column = valid_piece->column + 1; rhead_column < b_column; rhead_column++) {
                if (board_to_move_on->does_have_any_piece(b_row, rhead_column)) {
                    return false;
                }
            }
            return board_to_move_on->no_ally_there(valid_piece->team, b_row, b_column);
        }
        else {
            //Column to the left
            for (int lhead_column = valid_piece->column - 1; lhead_column > b_column; lhead_column--) {
                if (board_to_move_on->does_have_any_piece(b_row, lhead_column)) {
                    return false;
                }
            }
            return board_to_move_on->no_ally_there(valid_piece->team, b_row, b_column);;
        }
    }

    return false;
}