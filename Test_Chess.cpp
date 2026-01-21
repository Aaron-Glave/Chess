/*
* Credit to Catch2 for making it easy to run tests!
*/
//Disable errors caused by scanf
#pragma warning(disable:4996)


#include "Catch2_code/catch_amalgamated.hpp"
#include "Chess_code/Chess.h"
#include "Chess_code/Board.h"
#include "Chess_code/move_diagnolly.h"
#include "Chess_code/Chess_non_main.h"
#include "Chess_code/Piece.h"
#include "Chess_code/King.h"
#include "Chess_code/Move_horizontally.h"
#include "Chess_code/Team.h"
#include "Chess_code/Pawn.h"
#include "Chess_code/Knight.h"
#include "Chess_code/Straight_Line.h"
#include "Chess_code/Rook.h"
#include "Chess_code/Bishop.h"

#include "Chess_code/CastleMove.h"
#include "Chess_code/Queen.h"
#include "Chess_code/Safety.h"
#include "Chess_code/Teamname.h"
#include "Chess_code/diagnoal_direction.h"
#include "Chess_code/InvalidPiece.h"
#include "Chess_code/Saver.h"
#include "Chess_Code/SpacelessName.h"
#include "Chess_Code/SpacelessChessInput.h"
#include "Chess_code/Column_Notation.h"
#include <iostream>
#include <tuple>


TEST_CASE("User can hit space then type", "[spaceless]") {
    char myentry[7] = " \thi \n";
    remove_spaces(myentry, myentry, PIECE_NAME_LENGTH);
    REQUIRE(strcmp(myentry, "hi") == 0);
    printf("Spaces removed correctly.\n");
}

TEST_CASE("Space-only strings can be made into empty strings", "[spaceless]") {
    char myentry[6] = "\n \t \n";
    remove_spaces(myentry, myentry, PIECE_NAME_LENGTH);
    REQUIRE(strcmp(myentry, "") == 0);
    printf("Space-only string is empty as it should be.\n");
}

TEST_CASE("Names are good after cleaning", "[spaceless]") {
    char myentry[128] = " \n\t WPAWN7 \n \t ";
    char correctedentry[PIECE_NAME_LENGTH];
    get_standardized_name(myentry, correctedentry);
    //REQUIRE(strcmp(myentry, "wPawn7") == 0);
    printf("Spaces and tabs before and after a piece name are removed correctly.\n");
}

TEST_CASE("Truncated names are made empty", "[spaceless]") {
    char myentry[3] = "";
    std::string input_str = std::string(myentry);
    bool typed_too_long = false;
    while (!typed_too_long) {
        printf("Type at least 2 non-empty characters.\n");
        get_name_string(input_str);
        remove_spaces_string(input_str, input_str, 1024);
        if (strlen(input_str.c_str()) < 2) {
            printf("That name was too short; I read %s.\n", input_str.c_str());
        }
        else {
            typed_too_long = true;
        }
    }
    correct_with_length(input_str, myentry, 2);
    REQUIRE(strcmp(myentry, "") == 0);
}

TEST_CASE("Checkmate is detected correctly", "[checkmate]") {
    Board mainboard = Board();
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    printf("Aaron: Starting board:\n");
    mainboard.print_board();
    //Delete all black pieces except the king.
    for (int i = 0; i < 16; i++) {
        if (blackteam.pieces[i]->piecetype != TYPE::KING) {
            kill_piece(&mainboard, blackteam.pieces[i]);
        }
    }

    mainboard.place(&whiteteam.rook2, 7, 1);
    // Upgrade the white team's pawn into a rook so that it puts the black king in check.
    mainboard.place(&whiteteam.pawns[0], 8, 1);
    really_perform_upgrade(&whiteteam.pawns[0], TYPE::ROOK, &whiteteam, &mainboard);
    //Also place a black bishop in a place to capture the checking rook.
    blackteam.bishop1.alive = true;
    mainboard.place(&blackteam.bishop1, 7, 2);
    printf("Board setup for check test:\n");
    mainboard.print_board();

    Game_Status black_in_checkmate = mainboard.is_in_check(&blackteam, &whiteteam, true);
    printf("Should just be a check.\n");
    REQUIRE(black_in_checkmate == Game_Status::CHECK);
    printf("Check detected correctly.\n");

    printf("But if that bishop wasn't there, it would be checkmate.\n");
    kill_piece(&mainboard, &blackteam.bishop1);
    mainboard.print_board();
    black_in_checkmate = mainboard.is_in_check(&blackteam, &whiteteam, true);
    printf("Should be a checkmate.\n");
    REQUIRE(black_in_checkmate == Game_Status::CHECKMATE);
    printf("Checkmate detected correctly.\n");
}

TEST_CASE("Checkmate can be blocked by guarding your king", "[checkmate]") {
    Board mainboard = Board();
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    //Delete all black pieces except the king.
    for (int i = 0; i < 16; i++) {
        if (blackteam.pieces[i]->piecetype != TYPE::KING) {
            kill_piece(&mainboard, blackteam.pieces[i]);
        }
    }

    //Delete the white pawn blocking the rook.
    kill_piece(&mainboard, &whiteteam.pawns[1]);
    
    //Because I going to place a pawn in the space where the blackteam.rook2 (called that because it's in the same column as whiiteteam.rook2),
    //I need to move the black rook to a safe space first.
    blackteam.rook2.alive = true;
    mainboard.place(&blackteam.rook2, 1, 2);
    
    //Now I can safely place the white pawn in the same row as the black king.
    mainboard.place(&whiteteam.rook1, 7, 1);
    mainboard.place(&whiteteam.pawns[0], 8, 1);
    printf("Before I upgrade a pawn:\n");
    mainboard.print_board();

    // Upgrade the white team's pawn into a rook so that it puts the black king in check.
    really_perform_upgrade(&whiteteam.pawns[0], TYPE::ROOK, &whiteteam, &mainboard);
    printf("Upgraded a pawn:\n");
    mainboard.print_board();
    Piece* upgraded_piece = whiteteam.upgraded_pieces[0];
    
    printf("Board setup for check test:\n");
    mainboard.print_board();

    Game_Status black_in_checkmate = mainboard.is_in_check(&blackteam, &whiteteam, true);
    printf("Should just be a check.\n");
    REQUIRE(black_in_checkmate == Game_Status::CHECK);
    printf("Check detected correctly.\n");

    printf("But if that rook wasn't there, it would be checkmate.\n");
    kill_piece(&mainboard, &blackteam.rook2);
    //upgraded_piece->alive = true;
    //mainboard.place(upgraded_piece, 8, 1, true);
    mainboard.print_board();
    black_in_checkmate = mainboard.is_in_check(&blackteam, &whiteteam, true);
    printf("Should be a checkmate.\n");
    REQUIRE(black_in_checkmate == Game_Status::CHECKMATE);
    printf("Checkmate detected correctly.\n");
}

TEST_CASE("An en passant move CAN save you from checkmate", "[checkmate][passant]") {
    Board mainboard = Board();
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;

    for (int i = 0; i < 16; i++) {
        if ((whiteteam.pieces[i] != &whiteteam.the_king) &&
            !((whiteteam.pieces[i]->piecetype == TYPE::PAWN) && (whiteteam.pieces[i]->column == 5))) {
            kill_piece(&mainboard, whiteteam.pieces[i]);
        }
    }
    int index_white_pawn_column_5 = Pawn::column_to_index(5);
    int index_black_pawn_column_2 = Pawn::column_to_index(2);
    int index_black_pawn_column_5 = Pawn::column_to_index(5);
    
    mainboard.place(&whiteteam.pawns[index_white_pawn_column_5], 5, 5);
    
    mainboard.place(&whiteteam.the_king, 4, 5);
    //Remember, this is named Rook2 to the user because the code views the pieces from the absolute top-down perspective,
    //but names them from the user's perspective.
    mainboard.place(&blackteam.rook1, 5, 1);
    mainboard.place(&blackteam.rook2, 3, 8);

    //We need a few more rooks to paralize the king, so upgrade some pawns.
    mainboard.place(&blackteam.pawns[index_black_pawn_column_2], 1, 4);
    upgrade_pawn_if_needed(&blackteam.pawns[index_black_pawn_column_2], &blackteam, &mainboard, TYPE::ROOK);
    mainboard.place(&blackteam.pawns[index_black_pawn_column_5], 1, 6);
    upgrade_pawn_if_needed(&blackteam.pawns[index_black_pawn_column_5], &blackteam, &mainboard, TYPE::ROOK);
    //mainboard.place(&blackteam.pawns[8 - 7], 1, 7);
    //upgrade_pawn_if_needed(&blackteam.pawns[8 - 7], &blackteam, &mainboard, TYPE::ROOK);
    //mainboard.place(blackteam.pieces[7 + (9-7)], 5, 8);
    mainboard.print_board();

    //Now the only way to escape this check is to do an en passant move.
    Move move_weak_to_passant = Move(7, 6, 5, 6, &blackteam.pawns[Pawn::column_to_index(6)], NULL);
    mainboard.human_move_piece(&move_weak_to_passant);
    mainboard.print_board();
    Game_Status white_in_check = mainboard.is_in_check(&whiteteam, &blackteam, true);
    
    //Because an en passant move is possible we should only be in check!
    REQUIRE(white_in_check == Game_Status::CHECK);
    /*Even though we don't set the piece_landed_on variable to the pawn that was killed,
    * since it was killed via an en passant move, the game still revives the pawn killed with an en passant.
    * I wrote to specifically check for that in undo_move.
    // */
    Move move_passant = Move(5, 5, 6, 6, &whiteteam.pawns[Pawn::column_to_index(5)], NULL);
    mainboard.human_move_piece(&move_passant);
    mainboard.print_board();
    printf("Looks like your pawn saved you via an en passant move.\n");
    
    //Without having the en passant move available, there would be a checkmate.
    mainboard.undo_move(&move_passant, &whiteteam);
    kill_piece(&mainboard, &whiteteam.pawns[4]);
    white_in_check = mainboard.is_in_check(&whiteteam, &blackteam, true);
    mainboard.print_board();
    REQUIRE(white_in_check == Game_Status::CHECKMATE);
    printf("You NEEDED an en passant move to save you.\n");
}

TEST_CASE("Tab and spaces entered before a piece name are treated correctly", "[spaceless]") {
    char myentry[128] = "\n  \t  wPawn7                     \t \n\t \n ";

    char correctedentry[PIECE_NAME_LENGTH];
    get_standardized_name(myentry, correctedentry);
    REQUIRE(strcmp(correctedentry, "wPawn7") == 0);
    printf("Spaces and tabs before a piece name are removed correctly.\n");
}

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
    printf("Enter a piece name with spaces and tabs before and after it.\n");
    printf("Also, the name MUST be dirty in capitalization.\n");
    bool entered_testable_name = false;
    bool real_piece = false;
    //Use this flag to verify that the entered name is messy and dirty in capitalization.
    bool had_spaces_and_named = false;
    char correctedentry[PIECE_NAME_LENGTH] = "";
    while (!entered_testable_name) {
        char myentry[128] = "         ";
        had_spaces_and_named = false;
        correctedentry[0] = '\0';
        //NOTE: I use manual imput here to verify that the user entered a MESSY name.
        std::ignore = scanf("%128[^\n]", myentry);
        clearinput();
            
        bool start_spaces = false;
        bool anychars = false;
        for (int i = 0; myentry[i] != 0; i++) {
            //Verify that we had spaces before any real characters in the name.
            if (is_space(myentry[i]) && !start_spaces && !anychars) {
                start_spaces = true;
            }
            if (!is_space(myentry[i])) {
                anychars = true;
            }
            //Verify that we also end in spaces.
            if (is_space(myentry[i]) && anychars && start_spaces) {
                had_spaces_and_named = true;
                break;
            }
        }
        if (!had_spaces_and_named) {
            printf("Remember, to test this you have to add spaces or tabs before AND after.\n");
            printf("Try again.\n");
            continue;
        }
        //From this point on, we know the entered string did start and end with whitespace.

        REQUIRE(had_spaces_and_named);
        remove_spaces(myentry, myentry, PIECE_NAME_LENGTH);
        //We end this printed line in a period to make sure the name doesn't have any whitespace anymore.
        printf("Simplified to %s.\n", myentry);

        real_piece = false;
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
            continue;
        }

        bool typed_invalid = false;
        get_standardized_name(myentry, correctedentry);
        for (int i = 0; i < 16; i++) {
            if ((strcmp(correctedentry, whiteteam.pieces[i]->name) == 0)
                || (strcmp(correctedentry, blackteam.pieces[i]->name) == 0)) {
                entered_testable_name = true;
            }
        }

        //Because of the continue keyword in the if (real_piece) block above, we only reach this block if we DIDN'T enter a real piece name.
        if (!entered_testable_name) {
            printf("There's no piece named %s.\n", myentry);
            printf("Remember to enter an improperly capitalized name of a real piece with spaces or tabs before AND after it.\n");
        }
    }
    
    REQUIRE(entered_testable_name);

    printf("Input parsed to %s.\n", correctedentry);
}

TEST_CASE("First turn pawns can't move like knights #dad", "[pieces][pawns]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    Move pawn2;
    int index_for_column5 = Pawn::column_to_index(5);
    bool can_move;
    mainboard.print_board();
    for (int i = 1; i <= 8; i++) {
        if (i != 5) {
            pawn2 = Move(2, 5, 4, 5, &whiteteam.pawns[Pawn::column_to_index(i)], NULL);
            REQUIRE_FALSE(mainboard.human_move_piece(&pawn2));
			printf("White pawn in column %d can't move to row 4 column 5\n", i);
            pawn2 = Move(7, 5, 5, 5, &blackteam.pawns[Pawn::column_to_index(i)], NULL);
			REQUIRE_FALSE(mainboard.human_move_piece(&pawn2));
            printf("Black pawn in column %d can't move to row 5 column 5\n", i);
        }
    }
    int correct_index = Pawn::column_to_index(5);
    pawn2 = Move(2, 5, 4, 5, &whiteteam.pawns[correct_index], NULL);
    REQUIRE(mainboard.human_move_piece(&pawn2));
    mainboard.print_board();
	printf("White pawns do their first move correctly.\n");
	pawn2 = Move(7, 5, 5, 5, &blackteam.pawns[correct_index], NULL);
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
		Pawn* blackpawn = &blackteam.pawns[i];
		
		Move move1 = Move(2, blackpawn->column, 1, blackpawn->column, blackpawn, NULL);
		mainboard.human_move_piece(&move1);
        REQUIRE(upgrade_pawn_if_needed(blackpawn, &blackteam, &mainboard, TYPE::ROOK) == TYPE::ROOK);
        if (i == 7 || true) {
            mainboard.print_board();
        }
	}
    //*
    for (int i = 0; i < 8; i++) {
        if (blackteam.upgraded_pieces[i] == NULL) {
			printf("Black team upgraded piece %d is incorrectly NULL.\n", i);
            REQUIRE(false);
        }
        else if (blackteam.upgraded_pieces[7 - i]->piecetype != TYPE::ROOK) {
            REQUIRE(false);
        }
        else if (strlen(blackteam.upgraded_pieces[i]->name) < 7) {
            REQUIRE(false);
        }
        else {
            char rooknumber = blackteam.upgraded_pieces[i]->name[6];
            Pawn* blackpawn = &blackteam.pawns[i];
            char pawnnumber = blackpawn->name[5];
            REQUIRE(blackteam.upgraded_pieces[i]->alive == true);
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

TEST_CASE("Castling respects movement rules", "[errors][castle][castlemovement]") {
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

TEST_CASE("Upgraded pawns downgrade and delete their upgrades after the move that upgraded them is undone.", "[upgrade][undo]") {
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
    Pawn* wpawn = &whiteteam.pawns[Pawn::column_to_index(5)];
    Pawn* bpawn = &blackteam.pawns[Pawn::column_to_index(4)];
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
    Move firstmove = mainboard.make_move(&whiteteam.pawns[Pawn::column_to_index(5)], 4, 5);
    mainboard.human_move_piece(&firstmove);
    mainboard.print_board();
    
    bool printed = false;
    mainboard.print_passant(&printed);
    REQUIRE(printed);
    REQUIRE(mainboard.passantpawn.get_piece() == &whiteteam.pawns[Pawn::column_to_index(5)]);
    Move secondmove = mainboard.make_move(&blackteam.pawns[Pawn::column_to_index(4)], 5, 4);
    mainboard.human_move_piece(&secondmove);
    mainboard.print_board();
    mainboard.print_passant(&printed);
    REQUIRE(printed);
    REQUIRE(mainboard.passantpawn.get_piece() == &blackteam.pawns[Pawn::column_to_index(4)]);
    mainboard.undo_move(&secondmove);
    REQUIRE(mainboard.passantpawn.get_piece() == &whiteteam.pawns[Pawn::column_to_index(5)]);
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
    Move firstmove = mainboard.make_move(&whiteteam.pawns[Pawn::column_to_index(5)], 4, 5);
    mainboard.human_move_piece(&firstmove);
    mainboard.print_board();
    Move secondmove = mainboard.make_move(&blackteam.pawns[Pawn::column_to_index(3)], 6, 3);
    mainboard.human_move_piece(&secondmove);
    mainboard.print_board();
    REQUIRE(mainboard.passantpawn.get_piece() == NULL);
    mainboard.undo_move(&secondmove);
    REQUIRE(mainboard.passantpawn.get_piece() == &whiteteam.pawns[Pawn::column_to_index(5)]);
}

TEST_CASE("Moving a non-pawn onto the space a jumping pawn passed doesn't kill it", "[passant]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    printf("Testing to ensure that only pawns can capture en passant.\n");

    //Make the space in front of the pawn safe for the king to stand.
    //Kill black pawns in columns 3 and 6
    kill_piece(&mainboard, &blackteam.pawns[Pawn::column_to_index(3)]);
    kill_piece(&mainboard, &blackteam.pawns[Pawn::column_to_index(6)]);
    kill_piece(&mainboard, &blackteam.queen);
    kill_piece(&mainboard, &blackteam.bishop1);
    //kill_piece(&mainboard, &blackteam.pawns[8-6]);
    
    Piece* pieces_to_test[] = {
        &whiteteam.the_king, &whiteteam.queen, &whiteteam.rook1,
        &whiteteam.knight1, &whiteteam.bishop1
    };
    mainboard.place(&whiteteam.the_king, 6, 5);
    mainboard.place(&whiteteam.rook1, 6, 1);
    mainboard.place(&whiteteam.knight1, 4, 5);
    mainboard.place(&whiteteam.bishop1, 5, 3);
    mainboard.place(&whiteteam.queen, 5, 5);

    //Mark down the pawn indexes for the pawns we want to test
    int black_pawn_index_column4 = Pawn::column_to_index(4);
    Move bpawnmove = Move(7, 4, 5, 4, &blackteam.pawns[black_pawn_index_column4], NULL);
    mainboard.human_move_piece(&bpawnmove);
    mainboard.print_board();
    REQUIRE(mainboard.passantpawn.get_piece() == &blackteam.pawns[black_pawn_index_column4]);
    REQUIRE(mainboard.passantpawn.get_row() == 6);
    REQUIRE(mainboard.passantpawn.get_column() == 4);
    mainboard.print_board();
    REQUIRE(mainboard.passantpawn.get_piece() == &blackteam.pawns[black_pawn_index_column4]);
    REQUIRE(mainboard.is_in_check(&whiteteam, &blackteam, false) == Game_Status::NEUTRAL);
    mainboard.print_board();
    for (int i = 0; i < 5; i++) {
        Piece* enemy_of_pawn = pieces_to_test[i];
        REQUIRE(enemy_of_pawn->can_classmove(6, 4, &mainboard) == true);
        Move testmove = Move(enemy_of_pawn->row, enemy_of_pawn->column, 6, 4, enemy_of_pawn, NULL);
        mainboard.human_move_piece(&testmove);
        mainboard.print_board();
        REQUIRE(blackteam.pawns[black_pawn_index_column4].alive == true);
        mainboard.undo_move(&testmove);
        REQUIRE(mainboard.passantpawn.get_piece() == &blackteam.pawns[black_pawn_index_column4]);
    }
    REQUIRE(blackteam.pawns[black_pawn_index_column4].alive == true);
    mainboard.print_board();
    printf("Non-pawn pieces can't capture with en passant moves.\n");
}

TEST_CASE("Just print black pawn names", "[pieces][names][black]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    printf("Black pawn names:\n");
    for (int i = 0; i < 8; i++) {
        printf("Black pawn %s:\nColumn: %d\n\n",
            blackteam.pawns[i].name, blackteam.pawns[i].column);
        REQUIRE(blackteam.pawns[i].column == 8 - i);
    }
    
    printf("This means blackteam->pawns[i] is in column 8-i.\n");
}

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

    /* Remember, with 0 <= i < 8, for white pawns, column = i + 1
       and for black pawns, column = 8 - i */
    Move first_move = Move(2, 5, 4, 5, &whiteteam.pawns[5 - 1], NULL);
    Move second_move = Move(7, 4, 5, 4, &blackteam.pawns[8 - 4], NULL);

    //Load the game.
    saver.Dads_LoadGame(&mainboard, &whiteteam, &blackteam, &current_team_pointer, NULL);

    //Check that the starting columns are correct.
    for(int i = 0; i < 8; i++) {
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
    printf("If you pass a pointer to the load function, the variable you point at should be updated\n");
    printf("Currently, it's %d\n", test_val);
    saver.Dads_LoadGame(&mainboard, &blackteam, &whiteteam, &current_team_pointer, test_ptr);
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
    saver.Dads_LoadGame(&mainboard, &whiteteam, &blackteam, &current_team_pointer, NULL);
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
    saver.Dads_LoadGame(&mainboard, &whiteteam, &blackteam, &current_team_pointer, NULL);
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
    const int white_index_for_column5 = Pawn::column_to_index(5);
    const int black_index_for_column6 = Pawn::column_to_index(6);
    Move firstmove = mainboard.make_move(&whiteteam.pawns[white_index_for_column5], 4, 5);
    Move secondmove = mainboard.make_move(&whiteteam.pawns[white_index_for_column5], 5, 5);
    Move thirdmove = mainboard.make_move(&blackteam.pawns[black_index_for_column6], 5, 6);
    mainboard.human_move_piece(&firstmove);
    mainboard.print_board();
    mainboard.human_move_piece(&secondmove);
    mainboard.print_board();
    mainboard.human_move_piece(&thirdmove);
    mainboard.print_board();
    REQUIRE(mainboard.passantpawn.get_piece() == &blackteam.pawns[black_index_for_column6]);
    Move passantmove = mainboard.make_move(&whiteteam.pawns[white_index_for_column5], 6, 6);
    mainboard.human_move_piece(&passantmove);
    REQUIRE_FALSE(blackteam.pawns[black_index_for_column6].alive);
    mainboard.print_board();
    mainboard.undo_move(&passantmove, &whiteteam);
    mainboard.print_board();
    REQUIRE(blackteam.pawns[black_index_for_column6].alive);
    REQUIRE(mainboard.passantpawn.get_piece() == &blackteam.pawns[black_index_for_column6]);
    REQUIRE(mainboard.spaces[5 - 1][6 - 1] == &blackteam.pawns[black_index_for_column6]);
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

TEST_CASE("The Piece movement is properly changed through inheritance", "[pieces][upgrade]") {
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
    printf("Testing to make sure these pawns can only move up or down and they can only move 1 or 2 spaces.\n");
    mainboard.print_board();
    int bcolumn = 1;
    while (bcolumn != 4) {
        if (bcolumn != 2) {
            REQUIRE_FALSE(wpawn.can_classmove(3, bcolumn, &mainboard));
            REQUIRE_FALSE(bpawn.can_classmove(6, bcolumn, &mainboard));
        }
        else
        {
            for(int m_row = 1; m_row <= 8; m_row++) {
                if(m_row == 3 || m_row == 4) {
                    REQUIRE(wpawn.can_classmove(m_row, bcolumn, &mainboard));
                    REQUIRE_FALSE(bpawn.can_classmove(m_row, bcolumn, &mainboard));
                } else if(m_row == 6 || m_row == 5) {
                    REQUIRE(bpawn.can_classmove(m_row, bcolumn, &mainboard));
                    REQUIRE_FALSE(wpawn.can_classmove(m_row, bcolumn, &mainboard));
                } else {
                    REQUIRE_FALSE(wpawn.can_classmove(m_row, bcolumn, &mainboard));
                    REQUIRE_FALSE(bpawn.can_classmove(m_row, bcolumn, &mainboard));
                }
            }
        }
        bcolumn++;
    }
    printf("Pawns can't normally change their columns.\n");
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

TEST_CASE("Getting the column works right", "[column_input]") {
    REQUIRE((column_from_char('a') == column_from_char('A') && column_from_char('A') == 1));
    REQUIRE((column_from_char('b') == column_from_char('B') && column_from_char('B') == 2));
    REQUIRE((column_from_char('c') == column_from_char('C') && column_from_char('C') == 3));
    REQUIRE((column_from_char('d') == column_from_char('D') && column_from_char('D') == 4));
    REQUIRE((column_from_char('e') == column_from_char('E') && column_from_char('E') == 5));
    REQUIRE((column_from_char('f') == column_from_char('F') && column_from_char('F') == 6));
    REQUIRE((column_from_char('g') == column_from_char('G') && column_from_char('G') == 7));
    REQUIRE((column_from_char('h') == column_from_char('H') && column_from_char('H') == 8));
    REQUIRE((column_from_char('i') == -1 && column_from_char('I') == -1));

    REQUIRE(column_from_char('\n') == -1);
    REQUIRE(column_from_char('0') == -1);
}

TEST_CASE("Getting a character from the index of a column works", "[column_input]") {
    REQUIRE(char_from_column(1) == 'A');
    REQUIRE(char_from_column(2) == 'B');
    REQUIRE(char_from_column(3) == 'C');
    REQUIRE(char_from_column(4) == 'D');
    REQUIRE(char_from_column(5) == 'E');
    REQUIRE(char_from_column(6) == 'F');
    REQUIRE(char_from_column(7) == 'G');
    REQUIRE(char_from_column(8) == 'H');
    REQUIRE(char_from_column(9) == '\0');
}

TEST_CASE("Entering integers for columns works too", "[column_input]") {
    REQUIRE(column_from_char('1') == 1);
    REQUIRE(column_from_char('2') == 2);
    REQUIRE(column_from_char('3') == 3);
    REQUIRE(column_from_char('4') == 4);
    REQUIRE(column_from_char('5') == 5);
    REQUIRE(column_from_char('6') == 6);
    REQUIRE(column_from_char('7') == 7);
    REQUIRE(column_from_char('8') == 8);
    REQUIRE(column_from_char('9') == -1);
}

TEST_CASE("Pawn indexes are what they should be", "[info][pieces][pawns][indexes]") {
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    
    printf("White pawn indexes:\n");
    int test_index = -1, test_column = -1;
    for (int i = 0; i < 8; i++) {
        printf("White pawn %s:\nColumn: %d\nIndex: %d\n\n",
            whiteteam.pawns[i].name, whiteteam.pawns[i].column, i);
        test_index = Pawn::column_to_index(whiteteam.pawns[i].column);
        test_column = Pawn::index_to_column(COLOR::WHITE, i);
        REQUIRE(test_index == i);
        REQUIRE(test_column == whiteteam.pawns[i].column);
    }

    printf("Black pawn indexes:\n");
    for (int i = 0; i < 8; i++) {
        printf("Black pawn %s:\nColumn: %d\nIndex: %d\n\n",
            blackteam.pawns[i].name, blackteam.pawns[i].column, i);
        int test_index = Pawn::column_to_index(blackteam.pawns[i].column);
        test_column = Pawn::index_to_column(COLOR::BLACK, i);
        REQUIRE(test_index == i);
        REQUIRE(test_column == blackteam.pawns[i].column);
    }
    printf("This means our methods to convert pawn columns to indexes works.\n");
}

TEST_CASE("All pieces are the same size", "[pieces][size]") {
    printf("Checking piece sizes...\n");
    size_t piece_size = sizeof(Piece);
    size_t pawn_size = sizeof(Pawn);
    size_t rook_size = sizeof(Rook);
    size_t knight_size = sizeof(Knight);
    size_t bishop_size = sizeof(Bishop);
    size_t queen_size = sizeof(Queen);
    size_t king_size = sizeof(King);
    printf("Pawn size: %zu\nRook size: %zu\nKnight size: %zu\nBishop size: %zu\nQueen size: %zu\nKing size: %zu\n",
        pawn_size, rook_size, knight_size, bishop_size, queen_size, king_size);
    REQUIRE(piece_size == pawn_size);
    REQUIRE(piece_size == rook_size);
    REQUIRE(piece_size == knight_size);
    REQUIRE(piece_size == bishop_size);
    REQUIRE(piece_size == queen_size);
    REQUIRE(piece_size == king_size);
    printf("All pieces are the same size.\n");
}