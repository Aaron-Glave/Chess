#pragma once

#include "Piece.h"
#include "../DoubleLists/DoublyLinkedList.h"

//Expects to be used in Saver.cpp to track saved and loaded pieces
class ListOfLivingPieces : public DoublyLinkedList<Piece*> {
private:
    int number_of_pieces;
public:
    bool add_standard_if_living(Piece* piece_to_add);
    int get_number_of_living_pieces();
};