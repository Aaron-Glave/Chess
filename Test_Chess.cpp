/*
* Credit to Catch2 for making it easy to run tests!
*/
//Disable errors caused by scanf
#pragma warning(disable:4996)


#include "Catch2_code\catch_amalgamated.hpp"
#include "Chess_code\Chess.h"
#include "Chess_code\Board.h"
#include "Chess_code\move_diagnolly.h"
#include "Chess_code\Chess_non_main.h"
#include "Chess_code\Piece.h"
#include "Chess_code\King.h"
#include "Chess_code\Move_horizontally.h"
#include "Chess_code\Team.h"
#include "Chess_code\Pawn.h"
#include "Chess_code\Knight.h"
#include "Chess_code\Straight_Line.h"
#include "Chess_code\Rook.h"
#include "Chess_code\Bishop.h"

#include "Chess_code/CastleMove.h"
#include "Chess_code\Queen.h"
#include "Chess_code\Safety.h"
#include "Chess_code\Teamname.h"
#include "Chess_code\diagnoal_direction.h"
#include "Chess_code\InvalidPiece.h"
#include "Chess_code\Saver.h"
#include "Chess_Code\SpacelessName.h"
#include "Chess_Code\SpacelessChessInput.h"
#include <iostream>
#include <tuple>

void kill_piece(Board* mainboard, Piece* piece) {
    piece->alive = false;
    mainboard->spaces[piece->row - 1][piece->column - 1] = NULL;
}

TEST_CASE("User can hit space then type", "[spaceless]") {
    char myentry[7] = " \thi \n";
    remove_spaces(myentry, myentry);
    REQUIRE(strcmp(myentry, "hi") == 0);
    printf("Spaces removed correctly.\n");
}

TEST_CASE("Space-only strings can be made into empty strings", "[spaceless]") {
    char myentry[6] = "\n \t \n";
    remove_spaces(myentry, myentry);
    REQUIRE(strcmp(myentry, "") == 0);
    printf("Space-only string is empty as it should be.\n");
}

TEST_CASE("Names are good after cleaning", "[spaceless]") {
    char myentry[128] = " \n\t WPAWN7 \n \t ";
    char correctedentry[Piece::name_length];
    get_standardized_name(myentry, correctedentry);
    //REQUIRE(strcmp(myentry, "wPawn7") == 0);
    printf("Spaces and newlines before and after a piece name are removed correctly.\n");
}


//*
TEST_CASE("Newlines and spaces entered before a piece name are treated correctly", "[spaceless]") {
    char myentry[128] = "\n  \t  wPawn7                     \t \n\t \n ";

    char correctedentry[Piece::name_length];
    get_standardized_name(myentry, correctedentry);
    REQUIRE(strcmp(correctedentry, "wPawn7") == 0);
    printf("Spaces and newlines before a piece name are removed correctly.\n");
}
// */

TEST_CASE("Clean the name and find a matching piece", "[spaceless]") {
    Board mainboard = Board();
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);

    printf("White pieces:\n");
    for(int i = 0; i < 16; i++) {
        printf("%s\n", whiteteam.pieces[i]->name);
    }
    printf("Black pieces:\n");
    for (int i = 0; i < 16; i++) {
        printf("%s\n", blackteam.pieces[i]->name);
    }
    printf("Enter a piece name with spaces and newlines before and after it.\n");
    printf("Also, the name MUST be dirty in capitalism.\n");

    char myentry[128];
    char correctedentry[Piece::name_length];

    //Verify that the entered name is messy.
    bool had_spaces_and_named = false;
    while (!had_spaces_and_named) {
        get_name(myentry);
        bool start_spaces = false;
        bool anychars = false;
        for (int i = 0; myentry[i] != 0; i++) {
            if (is_space(myentry[i]) && !start_spaces) start_spaces = true;
            if (!is_space(myentry[i])) {
                anychars = true;
            }
            if (is_space(myentry[i]) && anychars && start_spaces) {
                had_spaces_and_named = true;
                break;
            }
        }
        if (!had_spaces_and_named) {
            printf("Remember, to test this you have to add spaces or tabs before AND after.\n");
        }
    }
    
    REQUIRE(had_spaces_and_named);
    remove_spaces(myentry, myentry);
    //We end in a period to make sure the name doesn't have any whitespace anymore.
    printf("Simplified to %s.\n", myentry);

    bool typed_invalid = false;
    bool real_piece = false;
    while (!typed_invalid) {
        for (int i = 0; i < 16; i++) {
            if ((strcmp(myentry, whiteteam.pieces[i]->name) == 0)
                || (strcmp(myentry, blackteam.pieces[i]->name) == 0)) {
                real_piece = true;
                break;
            }
        }
        if (real_piece) {
            //GET ANOTHER PIECE NAME
            printf("That piece's capitalization is valid.\nWe are trying to test BADLY capitalized pieces.\n");
            printf("Try again.\n");
            get_name(myentry);
            remove_spaces(myentry, myentry);
            real_piece = false;
        }
        else {
            typed_invalid = true;
        }
    }
    
    get_standardized_name(myentry, correctedentry);
    for (int i = 0; i < 16; i++) {
        if ((strcmp(correctedentry, whiteteam.pieces[i]->name) == 0)
            || (strcmp(correctedentry, blackteam.pieces[i]->name) == 0)) {
            real_piece = true;
            break;
        }
    }
    REQUIRE(real_piece);

    printf("Input parsed to %s.\n", correctedentry);
}

TEST_CASE("First turn pawns can't move like knights #dad", "[pieces][pawns]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    Move pawn2;
    mainboard.print_board();
    for (int i = 1; i <= 8; i++) {
        if (i != 5) {
            pawn2 = Move(2, 5, 4, i, &whiteteam.pawns[5 - 1], NULL);
            REQUIRE_FALSE(mainboard.human_move_piece(&pawn2));
			pawn2 = Move(7, 5, 5, i, &blackteam.pawns[8 - 5], NULL);
			printf("White pawn in column %d can't move to row 4 column 5\n", i);
			REQUIRE_FALSE(mainboard.human_move_piece(&pawn2));
            printf("Black pawn in column %d can't move to row 4 column 5\n", i);
        }
    }
    pawn2 = Move(2, 5, 4, 5, &whiteteam.pawns[5 - 1], NULL);
    REQUIRE(mainboard.human_move_piece(&pawn2));
    mainboard.print_board();
	printf("White pawns do their first move correctly.\n");
	pawn2 = Move(7, 5, 5, 5, &blackteam.pawns[8-5], NULL);
	REQUIRE(mainboard.human_move_piece(&pawn2));
    mainboard.print_board();
    printf("Black pawns do their first move correctly.\n");
}

TEST_CASE("Upgraded Black Pawn names", "[pieces][pawns][black]") {
	Board mainboard;
	Team whiteteam = Team(COLOR::WHITE, &mainboard);
	Team blackteam = Team(COLOR::BLACK, &mainboard);
	whiteteam.enemy_team = &blackteam;
	blackteam.enemy_team = &whiteteam;
	King* whiteking = &whiteteam.the_king;
	King* blackking = &blackteam.the_king;
    for (int i = 0; i < 16; i++) {
        Piece* makekingalone = whiteteam.pieces[i];
		if (whiteteam.pieces[i]->piecetype != TYPE::KING) {
			kill_piece(&mainboard, whiteteam.pieces[i]);
		}
    }
	
	mainboard.place(whiteking, 3, 5);
    mainboard.place(blackking, 6, 5);
	mainboard.print_board();
	for (int i = 0; i < 8; i++) {
		Pawn* blackpawn = &blackteam.pawns[i];
		mainboard.place(blackpawn, 2, blackpawn->column);
	}
	mainboard.print_board();
	for (int i = 0; i < 8; i++) {
		Pawn* blackpawn = &blackteam.pawns[7-i];
		
		Move move1 = Move(2, blackpawn->column, 1, blackpawn->column, blackpawn, NULL);
		mainboard.human_move_piece(&move1);
        REQUIRE(upgrade_pawn_if_needed(blackpawn, &blackteam, &mainboard, TYPE::ROOK) == TYPE::ROOK);
        if (i == 7) {
            mainboard.print_board();
        }
	}
    //*
    for (int i = 0; i < 8; i++) {
        if (blackteam.upgraded_pieces[7 - i] == NULL) {
			printf("Black team upgraded piece %d is incorrectly NULL.\n", 7 - i);
            REQUIRE(false);
        }
        else if (blackteam.upgraded_pieces[7 - i]->piecetype != TYPE::ROOK) {
            REQUIRE(false);
        }
        else if (strlen(blackteam.upgraded_pieces[7 - i]->name) < 7) {
            REQUIRE(false);
        }
        else {
            char rooknumber = blackteam.upgraded_pieces[7 - i]->name[6];
            Pawn* blackpawn = &blackteam.pawns[i];
            char pawnnumber = blackpawn->name[5];
            REQUIRE(blackteam.upgraded_pieces[7 - i]->alive == true);
            REQUIRE(rooknumber == pawnnumber);
        }
    }
    // */
    printf("Black pieces remember their names.\n");
}

TEST_CASE("Castling BLACK", "[castle][black]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    kill_piece(&mainboard, &blackteam.bishop2);
    kill_piece(&mainboard, &blackteam.knight2);
    mainboard.print_board();
    printf("Castling black.\n");
    CastleMove castle = CastleMove(Move(8, 5, 8, 7, &blackteam.the_king, NULL), &blackteam.rook2, CastleDirection::RIGHT, &mainboard, &blackteam);
    mainboard.human_move_piece(&castle);
    mainboard.print_board();
    0;
}

TEST_CASE("Castling respects movement rules", "[errors][castle][cstlemovement]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    King blackking = King(COLOR::BLACK);
    for (int i = 0; i < 16; i++) {
        if (!(
            (blackteam.pieces[i]->piecetype == TYPE::BISHOP) ||
            (blackteam.pieces[i]->piecetype == TYPE::KING) ||
            (blackteam.pieces[i]->piecetype == TYPE::ROOK)
            )) {
            kill_piece(&mainboard, blackteam.pieces[i]);
            //blackteam.pieces[i] = NULL;
        }
        kill_piece(&mainboard, whiteteam.pieces[i]);
    }
    kill_piece(&mainboard, &blackteam.bishop1);
    mainboard.print_board();
    Move move1 = Move(8, 6, 7, 7, &blackteam.bishop2, NULL);
    CastleMove* okcastle = NULL;
    bool errorcaught = false;
    bool success = false;
    mainboard.human_move_piece(&move1);
    mainboard.print_board();
    CastleMove castleright;
    try {
        castleright = CastleMove(Move(8, 5, 8, 7, &blackking, NULL), &blackteam.rook2, CastleDirection::RIGHT, &mainboard, &blackteam);
        success = true;
    }
    catch (InvalidMove e) {
        printf("%s\n", e.what());
        REQUIRE(false);
    }
    REQUIRE(blackteam.bishop2.first_turn_i_moved() == 1);
    REQUIRE(success == true);
    mainboard.human_move_piece(&castleright);
    mainboard.print_board();
    success = false;
    REQUIRE(blackking.first_turn_i_moved() == 2);
    REQUIRE(blackteam.rook2.first_turn_i_moved() == 2);
    mainboard.undo_move(&castleright);
    printf("Never mind...\n");
    REQUIRE(blackking.first_turn_i_moved() == -1);
    REQUIRE(blackteam.rook2.first_turn_i_moved() == -1);
    Move brook1move = Move(8, 1, 8, 2, &blackteam.rook1, NULL);
    Move brook1goback = Move(8, 2, 8, 1, &blackteam.rook1, NULL);
    mainboard.human_move_piece(&brook1move);
    mainboard.human_move_piece(&brook1goback);
    REQUIRE(blackteam.rook1.first_turn_i_moved() == 2);
}

TEST_CASE("Castling error checking", "[errors][castle]") {
    Board mainboard;
    Queen wqueen = Queen(COLOR::WHITE, 1, 5, 0);
    Rook wrook = Rook(COLOR::WHITE, 1, 1, 1);
    mainboard.place(&wqueen, 1, 5);
    mainboard.place(&wrook, 1, 1);
    mainboard.print_board();
    CastleMove* badcastle = NULL;
    bool error_thrown = false;
    try {
        badcastle = new CastleMove(Move(1, 5, 1, 3, &wqueen, NULL), &wrook, CastleDirection::LEFT, &mainboard, NULL);
    }
    catch (InvalidMove e) {
        REQUIRE(strcmp(e.what(), "wQueen is NOT a king!") == 0);
        error_thrown = true;
    }
    delete badcastle;
    badcastle = NULL;
    REQUIRE(error_thrown);
    printf("Castling error checking works. You CAN'T castle with a queen.\n");
    mainboard.print_board();
    printf("Queen can't castle. Check.\n");
    
}

TEST_CASE("Upgraded pawns downgrade and delete their upgrades after the move that upgraded them is undone..", "[upgrade][undo]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    kill_piece(&mainboard, mainboard.spaces[1][6]);

    mainboard.place(&blackteam.pawns[1], 2, 7);
    Move upgradepawn = Move(2, 7, 1, 8, &blackteam.pawns[1], NULL);
    mainboard.print_board();
    mainboard.human_move_piece(&upgradepawn);
    printf("The black pawn just moved to the bottom of the board...\n");
    mainboard.print_board();
    std::ignore = upgrade_pawn_if_needed(&blackteam.pawns[1], &blackteam, &mainboard, TYPE::KNIGHT);
    Piece** knightholder = &blackteam.upgraded_pieces[6];
    Knight* upgraded = dynamic_cast<Knight*>(blackteam.upgraded_pieces[6]);
    REQUIRE(blackteam.upgraded_pieces[6] != NULL);
    REQUIRE(blackteam.upgraded_pieces[6]->piecetype == TYPE::KNIGHT);
    mainboard.undo_move(&upgradepawn, &blackteam);
    mainboard.print_board();
    for (int i = 0; i < 8; i++) {
        REQUIRE(blackteam.upgraded_pieces[i] == NULL);
    }
    REQUIRE(blackteam.pieces[7 + 2]->piecetype == TYPE::PAWN);
    REQUIRE(blackteam.pieces[7 + 2]->row == 2);
    mainboard.place(&whiteteam.pawns[0], 7, 1);
    Move upwhite = Move(7, 1, 8, 2, &whiteteam.pawns[0], NULL);
    mainboard.human_move_piece(&upwhite);
    mainboard.print_board();
    std::ignore = upgrade_pawn_if_needed(&whiteteam.pawns[0], &whiteteam, &mainboard, TYPE::BISHOP);
    mainboard.print_board();
    REQUIRE(whiteteam.upgraded_pieces[0] != NULL);
    REQUIRE(whiteteam.upgraded_pieces[0]->piecetype == TYPE::BISHOP);
    mainboard.undo_move(&upwhite, &whiteteam);
    REQUIRE(whiteteam.upgraded_pieces[0] == NULL);
    REQUIRE(whiteteam.pieces[8] != NULL);
    bool undidwhiteupgrade = whiteteam.pawns[0].alive && whiteteam.pawns[0].row == 7 && whiteteam.pawns[0].column == 1;
    REQUIRE(undidwhiteupgrade);
    printf("Upgraded pieces deleted correctly.\n");
}

TEST_CASE("Castling in or through check should fail", "[castle][check]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    bool castle_failed = false;
    for (int i = 0; i < 16; i++) {
        if (whiteteam.pieces[i]->piecetype != TYPE::ROOK && whiteteam.pieces[i]->piecetype != TYPE::KING) {
            kill_piece(&mainboard, whiteteam.pieces[i]);
        }
    }
    //In check
    for (int i = 5; i >= 3; i--) {
        mainboard.place(&blackteam.rook2, 6, i);
        mainboard.print_board();
        try {
            CastleMove castlewhiteleft = CastleMove(
                Move(1, 5, 1, 3, &whiteteam.the_king, NULL),
                &whiteteam.rook1, CastleDirection::LEFT, &mainboard, &whiteteam
            );
        }
        catch (InvalidMove e) {
            printf("%s\n", e.what());
            castle_failed = true;
        }
        REQUIRE(castle_failed);
        castle_failed = false;
    }
    mainboard.place(&blackteam.rook2, 6, 2);
    mainboard.print_board();
    try {
        CastleMove castlewhiteleft = CastleMove(
            Move(1, 5, 1, 3, &whiteteam.the_king, NULL),
            &whiteteam.rook1, CastleDirection::LEFT, &mainboard, &whiteteam
        );
    }
    catch (InvalidMove e) {
        printf("%s\n", e.what());
        castle_failed = true;
    }
    REQUIRE_FALSE(castle_failed);
    printf("Castle check works on the left side.\n");
    for (int i = 5; i <= 7; i++) {
        mainboard.place(&blackteam.rook2, 6, i);
        mainboard.print_board();
        try {
            CastleMove castlewhiteright = CastleMove(
                Move(1, 5, 1, 7, &whiteteam.the_king, NULL),
                &whiteteam.rook2, CastleDirection::RIGHT, &mainboard, &whiteteam
            );
        }
        catch (InvalidMove e) {
            printf("%s\n", e.what());
            castle_failed = true;
        }
        REQUIRE(castle_failed);
        castle_failed = false;
    }
    mainboard.place(&blackteam.rook2, 6, 2);
    try {
        CastleMove castlewhiteright = CastleMove(
            Move(1, 5, 1, 7, &whiteteam.the_king, NULL),
            &whiteteam.rook2, CastleDirection::RIGHT, &mainboard, &whiteteam
        );
    }
    catch (InvalidMove e) {
        printf("%s\n", e.what());
        castle_failed = true;
    }
    REQUIRE_FALSE(castle_failed);
    bool king_is_in_starting_space = (whiteteam.the_king.row == 1 && whiteteam.the_king.column == 5 && whiteteam.the_king.first_turn_i_moved() == -1);
    REQUIRE(king_is_in_starting_space);
    printf("Castling is correctly followed!\n");
}

TEST_CASE("Undo castling", "[undo][castle]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    whiteteam.queen.alive = false;
    mainboard.print_board();
    kill_piece(&mainboard, &whiteteam.queen);
    kill_piece(&mainboard, &whiteteam.bishop1);
    kill_piece(&mainboard, &whiteteam.knight1);
    mainboard.print_board();
    CastleMove castle = CastleMove(Move(1, 5, 1, 3, &whiteteam.the_king, NULL), &whiteteam.rook1,
        CastleDirection::LEFT, &mainboard, &whiteteam);
    mainboard.human_move_piece(&castle);
    mainboard.print_board();
    REQUIRE(whiteteam.the_king.row == 1);
    REQUIRE(whiteteam.rook1.row == 1);
    REQUIRE(whiteteam.the_king.column == 3);
    REQUIRE(whiteteam.rook1.column == 4);
    mainboard.undo_move(&castle);
    mainboard.print_board();
    REQUIRE(whiteteam.the_king.row == 1);
    REQUIRE(whiteteam.rook1.row == 1);
    REQUIRE(whiteteam.the_king.column == 5);
    REQUIRE(whiteteam.rook1.column == 1);
    printf("Castling is undone.\n");
}

TEST_CASE("Pieces know the first turn they moved", "[FirstTurnPiece]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    Pawn* wpawn = &whiteteam.pawns[4];
    Pawn* bpawn = &blackteam.pawns[4];
    Move firstmove = mainboard.make_move(wpawn, 4, 5);
    Move secondmove = mainboard.make_move(bpawn, 5, 4);
    mainboard.human_move_piece(&firstmove);
    mainboard.human_move_piece(&secondmove);
    mainboard.print_board();
    REQUIRE(wpawn->first_turn_i_moved() == 1);
    REQUIRE(bpawn->first_turn_i_moved() == 2);
    printf("Turns seem to be counted.\n");
    King* wking = &whiteteam.the_king;
    Move thirdmove = mainboard.make_move(wking, 2, 5);
    mainboard.human_move_piece(&thirdmove);
    mainboard.print_board();
    REQUIRE(wpawn->first_turn_i_moved() == 1);
    REQUIRE(bpawn->first_turn_i_moved() == 2);
    REQUIRE(wking->first_turn_i_moved() == 3);
    printf("They definitely are.\n");
}

TEST_CASE("Undoing a first move correctly sets it back to -1", "[undo]") {
    Board mainboard;
    Rook brook = Rook(COLOR::BLACK, 2, 2, 1);
    King wking = King(COLOR::WHITE);
    mainboard.place(&brook, 3, 2);
    mainboard.place(&wking, 1, 5);
    Pawn wpawn = Pawn(COLOR::WHITE, 2, 1, 1);
    //Move firstmove = mainboard.make_move
}

TEST_CASE("I remember previous turn's passant", "[undo][passant]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    Move firstmove = mainboard.make_move(&whiteteam.pawns[5-1], 4, 5);
    mainboard.human_move_piece(&firstmove);
    mainboard.print_board();
    /*
    char pause[2];
    scanf("%1s", pause);
    clearinput();
    // */
    bool printed = false;
    mainboard.print_passant(&printed);
    REQUIRE(printed);
    REQUIRE(mainboard.passantpawn.get_piece() == &whiteteam.pawns[5 - 1]);
    Move secondmove = mainboard.make_move(&blackteam.pawns[5 - 1], 5, 4);
    mainboard.human_move_piece(&secondmove);
    mainboard.print_board();
    mainboard.print_passant(&printed);
    REQUIRE(printed);
    REQUIRE(mainboard.passantpawn.get_piece() == &blackteam.pawns[5 - 1]);
    mainboard.undo_move(&secondmove);
    REQUIRE(mainboard.passantpawn.get_piece() == &whiteteam.pawns[5 - 1]);
    printf("Looks like the passant is undone correctly.\n");
}
//I NEED TO KNOW THE TURN NUMBER ON THE BOARD IN ORDER TO MAKE THIS WORK
//ONE INT TO SAY THE TURN WHEN THE PASSANT WAS POSSIBLE, ANOTHER TO COUNT THE CURRENT TURN.
//REMEMBER, PAWNS HELP THE BOARD KNOW WHEN AN EN PASSANT HAPPENS, SO THEY HAVE TO ASK THE
//BOARD WHAT TURN IT IS.
TEST_CASE("Passants do have to happen immediately", "[passant][1turn]") {
    printf("Passants do have to happen immediately\n");
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    Move firstmove = mainboard.make_move(&whiteteam.pawns[5-1], 4, 5);
    mainboard.human_move_piece(&firstmove);
    mainboard.print_board();
    Move secondmove = mainboard.make_move(&blackteam.pawns[6 - 1], 6, 3);
    mainboard.human_move_piece(&secondmove);
    mainboard.print_board();
    REQUIRE(mainboard.passantpawn.get_piece() == NULL);
    mainboard.undo_move(&secondmove);
    REQUIRE(mainboard.passantpawn.get_piece() == &whiteteam.pawns[5 - 1]);
}

TEST_CASE("Loading a game with a passant pawn works", "[load][upgrade]") {
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
    printf("If you pass a pointer to the load function, the variable you point at should be updated\n");
    printf("Currently, it's %d\n", test_val);
    saver.Dads_LoadGame(&mainboard, &whiteteam, &blackteam, &current_team_pointer, test_ptr);
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
    Saver saver = Saver();
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    Move bpawn2m1 = Move(7, 2, 5, 2, &blackteam.pawns[8 - 2], NULL);
    Move bpawn2m2 = Move(5, 2, 4, 2, &blackteam.pawns[8 - 2], NULL);
    mainboard.human_move_piece(&bpawn2m1);
    mainboard.human_move_piece(&bpawn2m2);
    Move wpawn1 = Move(2, 1, 4, 1, &whiteteam.pawns[1 - 1], NULL);
    mainboard.human_move_piece(&wpawn1);
    mainboard.print_board();
    
    REQUIRE(mainboard.passantpawn.get_piece() == &whiteteam.pawns[1 - 1]);
    
    //Save the game.
    Team* current_team_pointer = &blackteam;
    saver.Dads_SaveGame(&mainboard, current_team_pointer, &whiteteam, &blackteam);
    mainboard.passantpawn.test_kill_passant();
    //Load the game.
    saver.Dads_LoadGame(&mainboard, &blackteam, &whiteteam, &current_team_pointer);
    mainboard.print_board();
    REQUIRE(mainboard.passantpawn.get_piece() == &whiteteam.pawns[1 - 1]);
    
    //Perform a passant;
    Move passant_move = Move(4, 2, 3, 1, &blackteam.pawns[8 - 2], NULL);
    mainboard.human_move_piece(&passant_move);
    mainboard.print_board();
    printf("Works for white pawns!\n");
}

TEST_CASE("The passant pawn status is saved and loaded correctly with a black pawn", "[load][passant][black]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    Saver saver = Saver();
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    Move wpawn2m1 = Move(2, 2, 4, 2, &whiteteam.pawns[2 - 1], NULL);
    Move wpawn2m2 = Move(4, 2, 5, 2, &whiteteam.pawns[2 - 1], NULL);
    mainboard.human_move_piece(&wpawn2m1);
    mainboard.human_move_piece(&wpawn2m2);
    Move bpawn1 = Move(7, 1, 5, 1, &blackteam.pawns[8 - 1], NULL);
    mainboard.human_move_piece(&bpawn1);
    mainboard.print_board();

    REQUIRE(mainboard.passantpawn.get_piece() == &blackteam.pawns[8 - 1]);

    //Save the game.
    Team* current_team_pointer = &blackteam;
    saver.Dads_SaveGame(&mainboard, current_team_pointer, &whiteteam, &blackteam);
    mainboard.passantpawn.test_kill_passant();
    //Load the game.
    saver.Dads_LoadGame(&mainboard, &blackteam, &whiteteam, &current_team_pointer);
    mainboard.print_board();
    REQUIRE(mainboard.passantpawn.get_piece() == &blackteam.pawns[8 - 1]);

    //Perform a passant;
    Move passant_move = Move(5, 2, 6, 1, &whiteteam.pawns[2 - 1], NULL);
    mainboard.human_move_piece(&passant_move);
    mainboard.print_board();
    printf("Works for black pawns!\n");
}

TEST_CASE("Undoing a successful en passant keeps the victim alive", "[undo][passant]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    Move firstmove = mainboard.make_move(&whiteteam.pawns[5 - 1], 4, 5);
    Move secondmove = mainboard.make_move(&whiteteam.pawns[5 - 1], 5, 5);
    Move thirdmove = mainboard.make_move(&blackteam.pawns[8 - 6], 5, 6);
    mainboard.human_move_piece(&firstmove);
    mainboard.print_board();
    mainboard.human_move_piece(&secondmove);
    mainboard.print_board();
    mainboard.human_move_piece(&thirdmove);
    mainboard.print_board();
    REQUIRE(mainboard.passantpawn.get_piece() == &blackteam.pawns[8 - 6]);
    Move passantmove = mainboard.make_move(&whiteteam.pawns[5 - 1], 6, 6);
    mainboard.human_move_piece(&passantmove);
    REQUIRE_FALSE(blackteam.pawns[8-6].alive);
    mainboard.print_board();
    mainboard.undo_move(&passantmove, &whiteteam);
    mainboard.print_board();
    REQUIRE(blackteam.pawns[6 - 1].alive);
    REQUIRE(mainboard.passantpawn.get_piece() == &blackteam.pawns[8 - 6]);
    REQUIRE(mainboard.spaces[5 - 1][6 - 1] == &blackteam.pawns[8 - 6]);
}

TEST_CASE("Throws errors upgrading pawns to themselves", "[errors]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    mainboard.place(&(whiteteam.pawns[0]), 8, 1);
    mainboard.print_board();
    try {
        upgrade_pawn_if_needed(&(whiteteam.pawns[0]), &whiteteam, &mainboard, TYPE::KING);
    }
    
    catch (InvalidPiece e) {
        printf("%s", e.what());
    }
    catch (std::exception didnt) {
        FAIL("An error wasn't caught?");
    }
    try {
        upgrade_pawn_if_needed(&(whiteteam.pawns[0]), &whiteteam, &mainboard, TYPE::PAWN);
    }
    catch (InvalidPiece e) {
        printf("%s", e.what());
    }
}

TEST_CASE("The Piece movement is properly changed through inheritance", "[pieces][virtual]") {
    King testKing = King(COLOR::WHITE);
    Piece* testkpiece = &testKing;
    Board mainboard = Board();
    mainboard.place(testkpiece, testKing.row, testKing.column);
    printf("Imagine a lonely king on a board alone.\n");
    mainboard.print_board();
    Move kingmove = mainboard.make_move(&testKing, 2, 6);
    REQUIRE(mainboard.human_move_piece(&kingmove));
    mainboard.print_board();
    printf("Kings are pieces just like everyone else.\n");
}

TEST_CASE("Pawns can catch pawns that jumped over", "[passant][capture]") {
    Board mainboard;
    Pawn wpawn1 = Pawn(COLOR::WHITE, 5, 1, 1);
    Pawn bpawn2 = Pawn(COLOR::BLACK, 7, 2, 2);
    mainboard.place(&wpawn1, 5, 1);
    mainboard.place(&bpawn2, 7, 2);
    Move passantmove1 = mainboard.make_move(&bpawn2, 5, 2);
    
    printf("Board before black pawn moved 2:\n");
    mainboard.print_board();
    REQUIRE(mainboard.human_move_piece(&passantmove1));
    printf("Board after black pawn moved 2:\n");
    mainboard.print_board();
    REQUIRE(mainboard.passantpawn.get_piece() == &bpawn2);
    Move passantmove2 = mainboard.make_move(&wpawn1, 6, 2); //6,2
    bool passant2 = mainboard.human_move_piece(&passantmove2);
    REQUIRE(passant2);
    printf("En Passant!\n");
    mainboard.print_board();
    REQUIRE_FALSE(bpawn2.alive);
}

TEST_CASE("Simple Castling test", "[interactive][castle]") {
    //ACTUALLY MAKE THEM CASTLE AND TELL WHETER OR NOT IT'S POSSIBLE. YOU WILL PROBABLY NEED TO TEST THIS IN 2 TESTS:
    //1. The king and rook are in the right place.
    //2. The king hasn't moved yet.
    printf("Testing Castling.\n");
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    whiteteam.queen.alive = false;
    kill_piece(&mainboard, &whiteteam.queen);
    kill_piece(&mainboard, &whiteteam.bishop1);
    kill_piece(&mainboard, &whiteteam.knight1);
    mainboard.print_board();
    printf("Pretend you just tryed to castle left.\nAll I did was pause the game so you can see what it looks like beforehand.\nType whatever you want to keep the tests running.\n");
    REQUIRE(can_castle(&whiteteam, &mainboard, "Left"));
    bool didcastle = false;
    Rook* whiterook1 = &(whiteteam.rook1);
    try {
        CastleMove castleleft = CastleMove(Move(1, 5, 1, 3, &whiteteam.the_king, NULL), whiterook1,
            CastleDirection::LEFT, &mainboard, &whiteteam);
        didcastle = true;
        didcastle &= mainboard.human_move_piece(&castleleft);
    }
    catch(InvalidMove e) {
        printf("%s\n", e.what());
    }
    catch (InvalidPiece e) {
        printf("%s\n", e.what());
    }
    REQUIRE(didcastle);
    mainboard.print_board();
    printf("Look. You castled!\n");
}

TEST_CASE("Simple check that pieces know when they first moved", "[pieces][first]") {
    Board mainboard;
    King wking = King(COLOR::WHITE);
    King bking = King(COLOR::BLACK);
    mainboard.place(&wking, 1, 5);
    mainboard.place(&bking, 8, 5);
    REQUIRE(wking.first_turn_i_moved() == -1);
    REQUIRE(bking.first_turn_i_moved() == -1);
    Move firstmove = mainboard.make_move(&wking, 2, 5);
    mainboard.human_move_piece(&firstmove);
    REQUIRE(wking.first_turn_i_moved() == 1);
    REQUIRE(bking.first_turn_i_moved() == -1);
    Move secondmove = mainboard.make_move(&bking, 7, 5);
    mainboard.human_move_piece(&secondmove);
    REQUIRE(wking.first_turn_i_moved() == 1);
    REQUIRE(bking.first_turn_i_moved() == 2);
    printf("Pieces know when they first moved.\n");
}

TEST_CASE("Queens moving diagonally", "[queen]") {
    Board mainboard;
    Queen testqueen = Queen(COLOR::WHITE, 5, 8, 1);
}

TEST_CASE("Upgrade a pawn. Pretend you typed.", "[interactive][upgrade]") {
    printf("You are the white team and you just landed a pawn on the top right square. Name a piece type to upgrade your pawn to.\n");
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    mainboard.place(&(whiteteam.pawns[7]), 8, 8);
    mainboard.print_board();
    TYPE pawn_upgrade_to = upgrade_pawn_if_needed(&(whiteteam.pawns[7]), &whiteteam, &mainboard);
    Piece* whitepawnfromearlier = &(whiteteam.pawns[7]);
    REQUIRE(whitepawnfromearlier != NULL);
    // We no longer point to a pawn, so this pointer is invalid.
    whitepawnfromearlier = NULL;
    Rook* upgradedrook = NULL;
    Knight* upgradedknight = NULL;
    Bishop* upgradedbishop = NULL;
    Queen* upgradedqueen = NULL;
    switch (pawn_upgrade_to)
    {
    case TYPE::ROOK:
        upgradedrook = dynamic_cast<Rook*>(whiteteam.upgraded_pieces[7]); //7 == 8-1
        REQUIRE(upgradedrook != NULL);
        break;
    case TYPE::KNIGHT:
        upgradedknight = dynamic_cast<Knight*>(whiteteam.upgraded_pieces[7]);
        REQUIRE(upgradedknight != NULL);
        break;
    case TYPE::BISHOP:
        upgradedbishop = dynamic_cast<Bishop*>((whiteteam.upgraded_pieces[7]));
        REQUIRE(upgradedbishop != NULL);
        break;
    case TYPE::QUEEN:
        upgradedqueen = dynamic_cast<Queen*>(whiteteam.upgraded_pieces[7]);
        REQUIRE(upgradedqueen != NULL);
    default:
        break;
    }
    REQUIRE(whiteteam.pieces[15] == whiteteam.upgraded_pieces[7]); //15 == 7 + 8
    mainboard.print_board();
}

TEST_CASE("Teams are correct", "[teams]") {
    REQUIRE(team_name(COLOR::BLACK) == "Black");
    REQUIRE(team_name(COLOR::WHITE) == "White");
}

TEST_CASE("Pieces can be cloned", "[pieces]") {
    Knight k1 = Knight(COLOR::WHITE, 1, 2, 1);
    Knight k2 = Knight(k1);
    printf("Pieces can be cloned.\n");
    REQUIRE(&k1 != &k2);
    k2.column = 3;
    REQUIRE(k1.column != k2.column);
}

TEST_CASE("Kings can hug", "[pieces][kings]") {
    printf("Kings are tested.\n");
    Board mainboard = Board();
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    REQUIRE(make_kings_hug(&whiteteam, &whiteteam, &blackteam) == true);
}
TEST_CASE("Rooks can't move diagonally", "[pieces][rooks]") {
    printf("Rooks are tested.\n");
    Rook testwhite = Rook(COLOR::WHITE, 1, 1, 1);
    Board mainboard;
    mainboard.place(&testwhite, 1, 1);
    mainboard.spaces[1][1] = NULL;
    REQUIRE(testwhite.can_classmove(2, 2, &mainboard) == false);
}
TEST_CASE("Move Down Bishop", "[bishop][pieces]") {
    Board mainboard;
    Bishop bbish = Bishop(COLOR::BLACK, 8, 6, 2);
    printf("Pretend a bishop starting at row 8 column 6 is moving down and to the right.\n");
    int row = bbish.row;
    int column = bbish.column;
    move_position_info_diag(&row, &column, DIAGONAL::DOWN_RIGHT, 1);
    printf("Row: %d, Column: %d.\n", row, column);
    REQUIRE(row == 7);
    REQUIRE(column == 7);
    printf("Now pretend it's moving 3 to the left.\n");
    move_position_info_diag(&row, &column, DIAGONAL::DOWN_LEFT, 3);
    REQUIRE(row == 4);
    REQUIRE(column == 4);
    printf("Row: %d, Column: %d.\n", row, column);
}
TEST_CASE("Pieces moving diagonally", "[diag][bishop]") {
    Board mainboard;
    Bishop wbish = Bishop(COLOR::WHITE, 5, 5, 2);
    mainboard.place(&wbish, 5, 5);
    //Assume there is nothing else.
    REQUIRE(can_move_diagnolly(&wbish, DIAGONAL::DOWN_LEFT, 3, &mainboard));
}

TEST_CASE("Pawns killing enemies", "[diag][pawn]") {
    Board mainboard;
    Bishop wbish = Bishop(COLOR::WHITE, 5, 5, 1);
    Pawn bpawn = Pawn(COLOR::BLACK, 6, 6, 1);
    Rook brook = Rook(COLOR::BLACK, 6, 5, 1);
    mainboard.place(&brook, 5, 6);
    mainboard.place(&wbish, 5, 5);
    mainboard.place(&bpawn, 6, 6);
    mainboard.print_board();
    printf("Can a black pawn be blocked by a black rook?\n");
    REQUIRE_FALSE(bpawn.can_classmove(5, 6, &mainboard));
    printf("Yes!\nCan it move like a bishop?\n");
    REQUIRE_FALSE(bpawn.can_classmove(4, 4, &mainboard));
    printf("No. And it shouldn't.\n");
    printf("Can it move to kill an enemy?\n");
    REQUIRE(bpawn.can_classmove(5, 5, &mainboard));
    printf("Yes.\n");
}

TEST_CASE("White pawn can be blocked by black pawn", "[pawn]") {
    Pawn wpawn = Pawn(COLOR::WHITE, 2, 1, 1);
    Pawn bpawn = Pawn(COLOR::BLACK, 3, 1, 1);
    Board mainboard;
    mainboard.place(&wpawn, 2, 1);
    mainboard.place(&bpawn, 3, 1);
    mainboard.print_board();
    printf("Is the white pawn blocked?\n");
    REQUIRE_FALSE(wpawn.can_classmove(3, 1, &mainboard));
    printf("Yes!\n");
}

TEST_CASE("White pawn moving up, black pawns moving down", "[pawn]") {
    Pawn wpawn = Pawn(COLOR::WHITE, 2, 2, 2);
    Pawn bpawn = Pawn(COLOR::BLACK, 7, 2, 2);
    Board mainboard;
    mainboard.place(&wpawn, 2, 2);
    mainboard.place(&bpawn, 7, 2);
    int bcolumn = 1;
    while (bcolumn != 4) {
        //printf("Pawn column %d\n", bcolumn);
        if (bcolumn != 2) {
            REQUIRE_FALSE(wpawn.can_classmove(3, bcolumn, &mainboard));
            REQUIRE_FALSE(bpawn.can_classmove(6, bcolumn, &mainboard));
        }
        else
        {
            REQUIRE(wpawn.can_classmove(3, bcolumn, &mainboard));
            REQUIRE(bpawn.can_classmove(6, bcolumn, &mainboard));
        }
        bcolumn++;
    }
    printf("Pawns are moving the way they should.\n");
}

TEST_CASE("Pieces getting blocked by their own team", "[diag][bishop]") {
    Board mainboard;
    Rook wRook = Rook(COLOR::WHITE, 1, 1, 1);
    Bishop wbish = Bishop(COLOR::WHITE, 5, 5, 2);
    mainboard.place(&wbish, 5, 5);
    mainboard.place(&wRook, 1, 1);
    REQUIRE_FALSE(can_move_diagnolly(&wbish, DIAGONAL::DOWN_LEFT, 4, &mainboard));
}

TEST_CASE("Pieces have to stop after killing an enemy moving diagonally", "[diag][pieces]") {
    Board mainboard;
    //Pretend this piece is a bishop.
    Bishop wbish = Bishop();
    Rook enemynear = Rook(COLOR::BLACK, 4, 4, 1);
    mainboard.place(&wbish, 5, 5);
    mainboard.place(&enemynear, 4, 4);
    printf("A bishop can move diagonally and catch an enemy, ");
    REQUIRE(can_move_diagnolly(&wbish, DIAGONAL::DOWN_LEFT, 1, &mainboard));
    printf("but after that, he has to stop.\n");
    REQUIRE_FALSE(can_move_diagnolly(&wbish, DIAGONAL::DOWN_LEFT, 2, &mainboard));
}

TEST_CASE("Pawns can kill only diagonally 1 space", "[diag][pawn]") {
    Board mainboard;
    Pawn bpawn = Pawn(COLOR::BLACK, 7, 2, 2);
    mainboard.place(&bpawn, 7, 2);
    Pawn wpawn1 = Pawn(COLOR::WHITE, 6, 1, 1);
    mainboard.place(&wpawn1, 6, 1);
    Pawn wpawn2 = Pawn(COLOR::WHITE, 6, 2, 2);
    mainboard.place(&wpawn2, 6, 2);
    Pawn wpawn3 = Pawn(COLOR::WHITE, 6, 3, 3);
    mainboard.place(&wpawn3, 6, 3);
    mainboard.print_board();
    REQUIRE_FALSE(bpawn.can_classmove(5, 2, &mainboard));
    REQUIRE_FALSE(bpawn.can_classmove(6, 2, &mainboard));
    printf("Blocked, can't move down.\n");
    REQUIRE(bpawn.can_classmove(6, 1, &mainboard));
    printf("Can capture diagonally left.\n");
    REQUIRE(bpawn.can_classmove(6, 3, &mainboard));
    printf("Can capture diagonally right.\n");
}
TEST_CASE("Pawns can normally move 2 on the start of their trurn, but not if they're blocked", "[pawn][pawn2]") {
    Board mainboard;
    Pawn wpawn4 = Pawn(COLOR::WHITE, 2, 4, 4);
    mainboard.place(&wpawn4, 2, 4);
    Move testmove2up = mainboard.make_move(&wpawn4, 4, 4);
    Pawn bpawn4 = Pawn(COLOR::BLACK, 7, 4, 4);
    mainboard.place(&bpawn4, 7, 4);
    Move testmove2down = mainboard.make_move(&bpawn4, 5, 4);
    mainboard.print_board();
    REQUIRE(mainboard.human_move_piece(&testmove2up));
    REQUIRE(mainboard.human_move_piece(&testmove2down));
    testmove2up.print_move();
    testmove2down.print_move();
    mainboard.print_board();
    printf("You can normally move a pawn 2 on your first turn.\n");
    mainboard.place(&wpawn4, 2, 4);
    mainboard.place(&bpawn4, 3, 4);
    mainboard.print_board();
    REQUIRE_FALSE(mainboard.human_move_piece(&testmove2up));
    printf("You tried and falied to move up 2.\n");
    mainboard.print_board();
    printf("You can be blocked if an opponent stands directly in front of you.\n");
    mainboard.spaces[2][3] = NULL;
    Bishop bbishop = Bishop(COLOR::BLACK, 4, 4, 1);
    mainboard.place(&bbishop, 4, 4);
    printf("But what if there was an enemy 2 spaces ahead of you instead?\n");
    mainboard.print_board();
    REQUIRE_FALSE(mainboard.human_move_piece(&testmove2up));
    printf("That doesn't work either.\n");
}

TEST_CASE("I remember En passant state after doing a move and undoing it.", "[undo][passant]") {
    printf("I remember En passant state after doing a move and undoing it.\n");
    Board mainboard;
    Pawn wpawn = Pawn(COLOR::WHITE, 2, 1, 1);
    mainboard.place(&wpawn, 2, 1);
    Pawn bpawn1 = Pawn(COLOR::BLACK, 7, 1, 1);
    mainboard.place(&bpawn1, 7, 1);
    Pawn bpawn2 = Pawn(COLOR::BLACK, 4, 2, 2);
    mainboard.place(&bpawn2, 4, 2);
    Move first_move = mainboard.make_move(&wpawn, 4, 1);
    mainboard.print_board();
    mainboard.human_move_piece(&first_move);
    mainboard.print_board();
    printf("I should remember the en passant state now.\n");
    REQUIRE(mainboard.spaces[3][0] == &wpawn);
    REQUIRE(mainboard.passantpawn.get_piece() == &wpawn);
    Move second_move = mainboard.make_move(&bpawn1, 6, 1);
    mainboard.human_move_piece(&second_move);
    printf("If I went here...\n");
    mainboard.print_board();
    REQUIRE(mainboard.passantpawn.get_piece() == NULL);
    printf("It's too late to make an en passant move.\n");
    mainboard.undo_move(&second_move);
    printf("But if I undid that move...\n");
    mainboard.print_board();
    REQUIRE(mainboard.passantpawn.get_piece() == &wpawn);
    printf("It is possible to kill the white piece by moving a black pawn to row %d, column %d.\n",
        mainboard.passantpawn.get_row(), mainboard.passantpawn.get_column());
    REQUIRE(bpawn2.can_classmove(3, 1, &mainboard));
    Move passantmove = mainboard.make_move(&bpawn2, 3, 1);
    mainboard.human_move_piece(&passantmove);
    mainboard.print_board();
    REQUIRE_FALSE(wpawn.alive);
    printf("Like this!\n");
}