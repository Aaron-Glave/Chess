#pragma once
#include "../Catch2_code/catch_amalgamated.hpp"
#include "../Chess_code/Chess_non_main.h"
#include "Node.h"
#include "DoublyLinkedList.h"

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


//Doubly Linked List Tests
TEST_CASE("Node class can add any type of piece", "[node]") {
    Board mainboard = Board();
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    printf("Here's the starting board.\nI'm going to repeatedly kill different pieces,\n");
    printf("and then reload the starting board immediately every time I kill a piece.\n");
    mainboard.print_board();
    REQUIRE(kill_reload_and_live(&mainboard, &whiteteam.pawns[0]));
    REQUIRE(kill_reload_and_live(&mainboard, &blackteam.rook1));
    REQUIRE(kill_reload_and_live(&mainboard, &whiteteam.knight2));
    REQUIRE(kill_reload_and_live(&mainboard, &blackteam.bishop2));
    REQUIRE(kill_reload_and_live(&mainboard, &whiteteam.queen));
    REQUIRE(kill_reload_and_live(&mainboard, &blackteam.the_king));
    printf("After all that the board shouldn't have changed.\n");
    mainboard.print_board();
}

TEST_CASE("Use a linked list as a queue", "[linkedlist]") {
    int test1to3[] = {1, 2, 3};
    printf("Testing storing [1, 2, 3] in a doubly linked list used as a queue.\n");
    DoublyLinkedList<int> int_queue = DoublyLinkedList<int>();
    for (int i = 0; i < 3; i++) {
        int_queue.add_tail(test1to3[i]);
    }
    for (int i = 0; i < 3; i++) {
        int val = int_queue.remove_head();
        printf("Expected %d, got %d\n", test1to3[i], val);
        REQUIRE(val == test1to3[i]);
    }
}

TEST_CASE("Load a smiley face", "[linkedlist]") {

}

// Saving and loading tests
TEST_CASE("All saved pieces have their starting columns right", "[save][load][pieces]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    Saver saver = Saver();
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;

    //Save the game.
    Team* current_team_pointer = &whiteteam;
    saver.Dads_SaveGame(&mainboard, current_team_pointer, &whiteteam, &blackteam);
    printf("Players saved the game before making any moves.\n");

    /* Remember to use the index lookup function! */
    Move first_move = Move(2, 5, 4, 5, &whiteteam.pawns[Pawn::column_to_index(5)], NULL);
    Move second_move = Move(7, 4, 5, 4, &blackteam.pawns[Pawn::column_to_index(4)], NULL);
    Move moves_made[] = { first_move, second_move };
    for(int i = 0; i < 2; i++) {
        mainboard.human_move_piece(&moves_made[i]);
    }
    printf("After playing a little bit, the board looked like this.\n");
    mainboard.print_board();

    //Load the game.
    saver.LoadGame(&mainboard, &whiteteam, &blackteam, &current_team_pointer, NULL);
    printf("Then someone decided to load the earlier save. ");
    printf("Now the board looks like this!\n");
    mainboard.print_board();

    //Check that the starting columns are correct.
    for (int i = 0; i < 8; i++) {
        REQUIRE(whiteteam.pawns[i].column == whiteteam.pawns[i].get_start_column());
        REQUIRE(blackteam.pawns[i].column == blackteam.pawns[i].get_start_column());
    }
}

TEST_CASE("Loading a game with a passant pawn works", "[load][upgrade][passant]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    Saver saver = Saver();
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    Move pawn1 = Move(2, 5, 4, 5, &whiteteam.pawns[5 - 1], NULL);
    mainboard.human_move_piece(&pawn1);
    kill_piece(&mainboard, &blackteam.pawns[0]);
    mainboard.place(&whiteteam.pawns[0], 8, 1);
    upgrade_pawn_if_needed(&whiteteam.pawns[0], &whiteteam, &mainboard, TYPE::QUEEN);
    mainboard.print_board();

    REQUIRE(mainboard.passantpawn.get_piece() == &whiteteam.pawns[5 - 1]);

    //Save the game.
    Team* current_team_pointer = &blackteam;

    saver.Dads_SaveGame(&mainboard, current_team_pointer, &whiteteam, &blackteam);

    //Pretend I made a turn.
    current_team_pointer = current_team_pointer->enemy_team;
    mainboard.passantpawn.test_kill_passant();
    REQUIRE(current_team_pointer == &whiteteam);
    REQUIRE(mainboard.spaces[7][0] != NULL);
    kill_piece(&mainboard, mainboard.spaces[7][0]);
    printf("Pretend the black team took a magical move resulting in this board.\n");
    mainboard.print_board();

    //Load the game.
    int test_val = 2;
    int* test_ptr = &test_val;
    printf("If you pass a pointer to the load function,\nthe variable you point at should be updated to 1 signaling a successful load\n");
    printf("Currently, it's %d\n", test_val);
    saver.LoadGame(&mainboard, &blackteam, &whiteteam, &current_team_pointer, test_ptr);
    REQUIRE(test_val == 1);
    printf("Now it's %d!\n", test_val);
    REQUIRE(mainboard.spaces[7][0]->alive);
    printf("After loading, the upgraded pawn is still alive.\n");
    mainboard.print_board();
}

TEST_CASE("The passant pawn status is saved and loaded correctly with a white pawn", "[load][passant][white]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    int column_1_index = Pawn::column_to_index(1);
    int column_2_index = Pawn::column_to_index(2);
    Saver saver = Saver();
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    Move bpawn2m1 = Move(7, 2, 5, 2, &blackteam.pawns[column_2_index], NULL);
    Move bpawn2m2 = Move(5, 2, 4, 2, &blackteam.pawns[column_2_index], NULL);
    mainboard.human_move_piece(&bpawn2m1);
    mainboard.human_move_piece(&bpawn2m2);
    Move wpawn1 = Move(2, 1, 4, 1, &whiteteam.pawns[column_1_index], NULL);
    mainboard.human_move_piece(&wpawn1);
    mainboard.print_board();

    REQUIRE(mainboard.passantpawn.get_piece() == &whiteteam.pawns[column_1_index]);

    //Save the game.
    Team* current_team_pointer = &blackteam;
    saver.Dads_SaveGame(&mainboard, current_team_pointer, &whiteteam, &blackteam);
    mainboard.passantpawn.test_kill_passant();
    printf("We killed the passant pawn to see if loading restores it correctly.\n");
    //Load the game.
    saver.LoadGame(&mainboard, &whiteteam, &blackteam, &current_team_pointer, NULL);
    REQUIRE(mainboard.passantpawn.get_piece() == &whiteteam.pawns[column_1_index]);
    printf("After loading, the passant pawn should be back.\n");
    mainboard.print_board();
    //Perform a passant;
    Move passant_move = Move(4, 2, 3, 1, &blackteam.pawns[column_2_index], NULL);
    mainboard.human_move_piece(&passant_move);
    mainboard.print_board();
    printf("Works for killing white pawns via en passant!\n");
}

TEST_CASE("The passant pawn status is saved and loaded correctly with a black pawn", "[load][passant][black]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    int column_1_index = Pawn::column_to_index(1);
    int column_2_index = Pawn::column_to_index(2);
    Pawn* wPawn2 = &whiteteam.pawns[column_2_index];
    Pawn* bPawn8 = &blackteam.pawns[column_1_index];
    Saver saver = Saver();
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    Move wpawn2m1 = Move(2, 2, 4, 2, wPawn2, NULL);
    Move wpawn2m2 = Move(4, 2, 5, 2, wPawn2, NULL);
    
    printf("Testing passant status...\n");
    mainboard.human_move_piece(&wpawn2m1);
    mainboard.human_move_piece(&wpawn2m2);
    Move bpawn_column_1 = Move(7, 1, 5, 1, bPawn8, NULL);
    mainboard.human_move_piece(&bpawn_column_1);
    mainboard.print_board();

    Pawn* test_passant_pawn = mainboard.passantpawn.get_piece();
    REQUIRE(mainboard.passantpawn.get_piece() == bPawn8);

    //Save the game.
    Team* current_team_pointer = &blackteam;
    saver.Dads_SaveGame(&mainboard, current_team_pointer, &whiteteam, &blackteam);
    mainboard.passantpawn.test_kill_passant();
    printf("We killed the passant pawn to see if loading restores it correctly.\n");
    mainboard.print_board();

    //Load the game.
    saver.LoadGame(&mainboard, &whiteteam, &blackteam, &current_team_pointer, NULL);
    printf("After loading, the passant pawn should be back.\n");
    mainboard.print_board();
    test_passant_pawn = mainboard.passantpawn.get_piece();
    REQUIRE((test_passant_pawn == &blackteam.pawns[column_1_index]
    && (test_passant_pawn == 
        mainboard.spaces[test_passant_pawn->row - 1][test_passant_pawn->column - 1]
        ))
    );

    //Perform a passant;
    printf("Now we're gonna perform the en passant move...\n");
    Move passant_move = Move(5, 2, 6, 1, &whiteteam.pawns[2 - 1], NULL);
    mainboard.human_move_piece(&passant_move);
    mainboard.print_board();
    REQUIRE(!blackteam.pawns[column_1_index].alive);
    printf("Works for black pawns!\n");
}

