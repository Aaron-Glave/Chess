#include "Board.h"
void kill_piece(Board* mainboard, Piece* piece) {
    piece->alive = false;
    mainboard->spaces[piece->row - 1][piece->column - 1] = NULL;
}
