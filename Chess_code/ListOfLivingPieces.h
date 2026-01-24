#pragma once

#include "Piece.h"
#include "../DoubleLists/DoublyLinkedList.h"

class ListOfLivingPieces : public DoublyLinkedList<Piece*> {
    bool add_if_living(Piece* piece_to_add) {
        if (piece_to_add != nullptr && piece_to_add->alive) {
            this->add_tail(piece_to_add);
            return true;
        }
        return false;
    }
}