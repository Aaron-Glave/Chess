#ifndef AARON_PASSANT
#define AARON_PASSANT
#include "Pawn.h"
#include <stdio.h>
struct PassantPawn {
    //Note: This struct is read-only.
    int get_row() const {
        return passant_row;
    }

    int get_column() const {
        return passant_column;
    }

    int get_turn_made() const {
        return turn_made;
    }

    Pawn* get_piece() {
        return pawnthatjustmoved2;
    }
    
    PassantPawn() {
        pawnthatjustmoved2 = NULL;
        passant_row = -1;
        passant_column = -1;
        turn_made = -1;
    }
    PassantPawn(Pawn* b_pawnthatjustmoved2, int b_passant_row, int b_passant_column, int passant_turn) {
        pawnthatjustmoved2 = b_pawnthatjustmoved2;
        passant_row = b_passant_row;
        passant_column = b_passant_column;
        turn_made = passant_turn;
    }

    void test_kill_passant() {
        pawnthatjustmoved2 = NULL;
        passant_row = -1;
        passant_column = -1;
        turn_made = -1;
    }
private:
    int passant_row;
    int passant_column;
    int turn_made;
    Pawn* pawnthatjustmoved2;
};
#endif // !AARON_PASSANT
