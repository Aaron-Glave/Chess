#include "ListOfLivingPieces.h"

//Used to help saving living pieces.
bool ListOfLivingPieces::add_standard_if_living(Piece* piece_to_add) {
    if (piece_to_add != nullptr && piece_to_add->alive) {
        this->add_tail(piece_to_add);
        number_of_pieces++;
        return true;
    }
    return false;
}

int ListOfLivingPieces::get_number_of_living_pieces()
{
    return number_of_pieces;
}
