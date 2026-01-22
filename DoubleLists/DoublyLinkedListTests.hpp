#pragma once
#include "../Catch2_code/catch_amalgamated.hpp"
#include "../Chess_code/Chess_non_main.h"
#include "Node.h"

template<typename T> void print_survival(Board* mainboard, T* piece_to_test) {
    if (piece_to_test->alive) printf("%s is alive.\n", piece_to_test->name);
    else printf("%s is still dead.\n", piece_to_test->name);
}

template<typename T> bool kill_reload_and_live(Board *mainboard, T* piece_to_test) {
    Node<T>* node = new Node<T>(*piece_to_test);
    kill_piece(mainboard, piece_to_test);
    printf("%s died. ", piece_to_test->name);
    mainboard->print_board();
    piece_to_test->AssignSavedData(&node->data);
    printf("But after reloading, ");
    if (piece_to_test->alive) {
        mainboard->place(piece_to_test, piece_to_test->row, piece_to_test->column);
    }
    delete node;
    print_survival(mainboard, piece_to_test);
    return piece_to_test->alive && 
        (mainboard->spaces[piece_to_test->row-1][piece_to_test->column-1] == piece_to_test);
}



TEST_CASE("Node class can add any type of piece", "[node]") {
    Board mainboard = Board();
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    mainboard.print_board();
    REQUIRE(kill_reload_and_live(&mainboard, &whiteteam.pawns[0]));
    REQUIRE(kill_reload_and_live(&mainboard, &blackteam.rook1));
    REQUIRE(kill_reload_and_live(&mainboard, &whiteteam.knight2));
    REQUIRE(kill_reload_and_live(&mainboard, &blackteam.bishop2));
    REQUIRE(kill_reload_and_live(&mainboard, &whiteteam.queen));
    REQUIRE(kill_reload_and_live(&mainboard, &blackteam.the_king));
    printf("After reloading all pieces, the board is:\n");
    mainboard.print_board();
}

TEST_CASE("Load a smiley face", "[linkedlist]") {

}