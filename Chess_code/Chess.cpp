// Chess.cpp : This file contains the 'main' function. Program execution begins and ends there.
// I grant credit for Dad. He helped a lot.
//Not even sure this is better than my old code.
#pragma warning(disable:4996)

#include <ctype.h>
#include <tuple>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <chrono>
#include <thread>

#include "Chess_non_main.h"

using namespace std;

void print_how_to_hug() {
    printf("To hug the king you just landed on instead of defeating him, just type Yes\nWith no punctuation.\n");
}

bool make_kings_hug(Team *current_team, Team*whiteteam, Team*blackteam) {
    printf("%s player hugs %s player. Both win!\n",
        current_team->color == whiteteam->color ? whiteteam->team_name() : blackteam->team_name(),
        current_team->enemy_team->color == blackteam->color ? blackteam->team_name() : whiteteam->team_name());
    return true;
}

int chess(bool talk_hug, bool show_debugging, bool should_load_man)
{
    /*Note that you can't castle while in check.
    * The board should know what teams are in check and prevent castling if the team trying to castle is in check.
    * It is possible for both teams to be in check, so the board needs 2 booleans to tell whether or not the current team is in check.
    * */
    Board mainboard;
    Team whiteteam = Team(COLOR::WHITE, &mainboard);
    Team blackteam = Team(COLOR::BLACK, &mainboard);
    whiteteam.enemy_team = &blackteam;
    blackteam.enemy_team = &whiteteam;
    Team* public_white_team = &whiteteam;
    Team* public_black_team = &blackteam;
    Piece* piecetomove = NULL;
    Saver game_saver = Saver();
    
    int m_row = 0;
    int m_column = 0;
    bool okmove = false;
    bool piecefound = false;
    bool isgameover = false;
    bool did_custom_command = false;
    char nameofpiecetomove[PIECE_NAME_LENGTH];
    int piece = 1;
    //char current_team = 'w';
    Team* current_team = &whiteteam;

    TYPE type_of_piecetomove = TYPE::PAWN;
    Game_Status current_status = Game_Status::NEUTRAL;

    //Alternative commands other than moving 1 piece.
    bool did_try_save = false;
    bool did_try_tie = false;
    bool did_try_castle = false;
    bool done_loading_man = !should_load_man;
    bool did_load = false;
    //You cannot load the game twice. TODO: FOLLOW THAT RULE.
    bool has_loaded_file = false;
    bool did_fail_loading = false;
    bool swapped_or_done = false;
    
    //Setup
    Piece* wKnight2 = whiteteam.pieces[6];
    Piece* bKnight2 = blackteam.pieces[6];
    Piece* wKing = whiteteam.pieces[3];
    Piece* bKing = blackteam.pieces[3];
    Piece* current_king = wKing;
    bool am_i_in_check = false;
    bool kings_want_to_hug = false;

    if (should_load_man) {
        //Swapping the current team sometimes helps you set up custom boards.
        printf("Type cteam to say you're done loading the previous game and the current team starts,...\n");
        printf("...or type oteam to let the opponent move...\n");
        printf("or type die after selecting a piece to remove a piece from the board.\n");
    }
    printf("\nGod answered my prayers and helped me make this game. He deserves credit!\n\n");
    
    //*
    while (wKing->alive && bKing->alive)
    {
        //You start the turn with no piece selected.
        piecetomove = NULL;
        mainboard.print_board();
        
        if (show_debugging) {
            printf("DEBUG: Turn %d\n", mainboard.current_turn());
        }

        
        if (current_team->color == COLOR::WHITE) {
            //We already checked if we are in check or checkmate at the end of our opponent's turn.
            if (whiteteam.current_status == Game_Status::CHECK) {
                am_i_in_check = true;
                printf("You are in check!\n");
            }
            if (whiteteam.current_status == Game_Status::CHECKMATE) {
                printf("Black team wins with a checkmate. Good game.\n");
                wKing->alive = false;
                clean_chess_input(nameofpiecetomove);
                return 0;
            }
        }
        else if (current_team->color == COLOR::BLACK) {
            if (blackteam.current_status == Game_Status::CHECK) {
                am_i_in_check = true;
                printf("You are in check!\n");
            }
            if (blackteam.current_status == Game_Status::CHECKMATE) {
                printf("White team wins with a checkmate. Good game.\n");
                bKing->alive = false;
                clean_chess_input(nameofpiecetomove);
                return 0;
            }
        }
        printf("%s turn.\n", team_name(current_team->color));
        printf("Which piece do you want to move? ");
        
        clean_chess_input(nameofpiecetomove);
        //Alternative commands other than moving 1 piece
        //TODO: Make options to kill a selected piece.
        if (should_load_man) {
            if (strcmp(nameofpiecetomove, "cTeam") == 0) {
                if (done_loading_man) {
                    printf("You can't swap teams without making a move anymore.\n");
                }
                else {
                    printf("You are done setting up the game. Now fight the king!\n");
                    if (talk_hug) {
                        printf("Or you can hug him.\n");
                        print_how_to_hug();
                    }
                    done_loading_man = true;
                }
                swapped_or_done = true;
            }
            else if (strcmp(nameofpiecetomove, "oTeam") == 0) {
                if (done_loading_man) {
                    printf("You are already done loading.\n");
                }
                else {
                    //NOTE: THE TEAMS SWAP ON THIS LINE. THE BOARD WAS CORRECT,
                    //BUT IT WAS THE WRONG PLAYER'S TURN.
                    current_team = current_team->enemy_team;
                    
                }
                swapped_or_done = true;
            }
            
        }

        if (strcmp(nameofpiecetomove, "sAve") == 0) {
            if (game_saver.Dads_SaveGame(&mainboard, current_team, &whiteteam, &blackteam)) {
                printf("Game saved.\n");
            }
            else {
                printf("ERROR saving game!\n");
            }
            did_try_save = true;
            did_custom_command = true;
        }
        if (strcmp(nameofpiecetomove, "lOad") == 0) {
            if (has_loaded_file) {
                printf("You have to quit before you can load again.\n");
                did_fail_loading = true;
            }
            else {
                if (game_saver.Dads_LoadGame(&mainboard, &blackteam, &whiteteam, &current_team)) {
                    printf("Game loaded.\n");
                    has_loaded_file = true;
                }
                else {
                    printf("ERROR loading game!\n");
                }
                did_load = true;
            }
            did_custom_command = true;
        }

        if (strcmp(nameofpiecetomove, "sUrrender") == 0) {
            printf("You give up. %s team wins!\n", current_team->enemy_team->full_name);
            clean_chess_input(nameofpiecetomove);
            return 0;
        }
        if (strcmp(nameofpiecetomove, "tIe") == 0) {
            printf("Opponent: Do you agree that this match should be called a tie? ");
            get_with_number_of_chars_including_null(nameofpiecetomove, 4);
            nameofpiecetomove[0] = toupper(nameofpiecetomove[0]);
            for (int i = 1; i < 3; i++) {
                nameofpiecetomove[i] = tolower(nameofpiecetomove[i]);
            }
            if (strcmp(nameofpiecetomove, "Yes") == 0) {
                printf("You both give up. Neither team wins!\n");
                clean_chess_input(nameofpiecetomove);
                return 0;
            }
            else {
                did_try_tie = true;
                did_custom_command = true;
            }
        }
        if (strcmp(nameofpiecetomove, "hUg") == 0) {
            make_kings_hug(current_team, &whiteteam, &blackteam);
            clean_chess_input(nameofpiecetomove);
            return 0;
        }
        if (show_debugging && strcmp(nameofpiecetomove, ("eXecute")) == 0) {
            printf("Name of piece to execute: ");
            clean_chess_input(nameofpiecetomove);
            did_custom_command = true;
        }

        //Maybe you are trying to castle.
        CastleMove castle_move;
        if (strcmp(nameofpiecetomove, "cAstle") == 0) {
            bool have_decided_direction = false;
            did_try_castle = true;
            did_custom_command = true;
            while (!have_decided_direction) {
                printf("Which side do you want to castle?\nYou can name a direction (Left or Right) or say Stop to change your mind.\n");
                get_with_number_of_chars_including_null(nameofpiecetomove, 6);
                nameofpiecetomove[0] = toupper(nameofpiecetomove[0]);
                for (int i = 1; i < 5; i++) {
                    nameofpiecetomove[i] = tolower(nameofpiecetomove[i]);
                }
                if (strcmp(nameofpiecetomove, "Left") == 0) {
                    try {
                        castle_move = CastleMove(
                            Move(current_team->the_king.row, 5, current_team->the_king.row, 3,
                                &(current_team->the_king),
                                NULL),
                            &(current_team->rook1),
                            CastleDirection::LEFT, &mainboard, current_team);
                        have_decided_direction = true;
                        okmove = true;
                    }
                    catch (InvalidMove e) {
                        printf("%s\n", e.what());
                        okmove = false;
                        break;
                    }
                    catch (std::exception e) {
                        printf("%s\n", e.what());
                        //You failed to castle.
                        okmove = false;
                        break;
                    }
                }
                else if (strcmp(nameofpiecetomove, "Right") == 0) {
                    try {
                        castle_move = CastleMove(
                            Move(current_team->the_king.row, 5, current_team->the_king.row, 7,
                                &(current_team->the_king),
                                NULL),
                            &(current_team->rook2),
                            CastleDirection::RIGHT, &mainboard, current_team);
                        have_decided_direction = true;
                        okmove = true;
                    }
                    catch (InvalidMove e) {
                        printf("%s\n", e.what());
                        okmove = false;
                        break;
                    }
                    catch (std::exception e) {
                        printf("%s\n", e.what());
                        //You failed to castle.
                        okmove = false;
                        break;
                    }
                }
                else if (strcmp(nameofpiecetomove, "Stop") == 0) {
                    okmove = false;
                    break;
                }
                else {
                    printf("Invalid direction. Try again.\n");
                }
            }

            //Still in the castling block and we know our move is legit if okmove is true.
            if (okmove) {
                try {
                    mainboard.human_move_piece(&castle_move);

                    /* UPDATE THE BOARD'S KNOWLEDGE OF BOTH TEAM'S CHECK HERE. */
                    current_team->enemy_team->current_status = mainboard.is_in_check(current_team->enemy_team, current_team, true);
                    current_team->current_status = mainboard.is_in_check(current_team, current_team->enemy_team, false);
                    /* NOTE: THE TEAMS SWAP ON THIS LINE. */
                    current_team = current_team->enemy_team;
                }
                catch (InvalidMove e) {
                    printf("%s\n", e.what());
                    //You failed to move.
                    okmove = false;
                }
            }
            did_custom_command = true;
        }

        //If not running an alernative command, Find piece with the entered name
        piecefound = false;
        for (int i = 0; i < 8 && !did_custom_command && !did_try_castle && !did_load && !did_try_save; i++) {
            if (piecefound) {
                break;
            }
            for (int j = 0; j < 8; j++) {
                if (mainboard.spaces[i][j] != NULL) {
                    //We need to check the type here because we don't yet know what type of piece we have selected.
                    if (strcmp(nameofpiecetomove, mainboard.spaces[i][j]->name) == 0) {
                        //We know the piece is alive because it's on the board.
                        piecetomove = mainboard.spaces[i][j];
                        piecefound = true;
                        type_of_piecetomove = piecetomove->piecetype;
                        break;
                    }
                }
            }
        }

        //The game looks at the board when searching for pieces, so if your piece is dead it WON'T be found and it will be called invalid.
        if (piecefound && wrong_team(piecetomove, current_team->color)) {
            printf("Wrong team, dummy!\n");
        } else if (!piecefound && !did_try_castle && !did_load && !did_try_save && !did_try_tie && !did_fail_loading && !swapped_or_done) {
            printf("That piece is either dead, or non-existent.\n");
        }
        
        
        //We found the piece. Now move it.
        bool landonokplace = false;
        bool entered_1_column = false;
        if (!wrong_team(piecetomove, current_team->color) && piecefound && !did_try_tie && !did_try_castle && !did_load) {
            char spacenum[2] = { '\0','\0' };
            printf("Where do you want to move %s?\n", nameofpiecetomove);
            printf("Enter your move.\n");
            char cspace = '\0';
            if (show_debugging) {
                printf("First move with %s: %d\n", piecetomove->name, piecetomove->first_turn_i_moved());
            }
            
            //printf("Row: ");
            printf("Column: ");
            m_column = get_column();
            //cspace = spacenum[0];
            entered_1_column = true;
            //m_column = atoi(&cspace);
            if ((m_column >= 1) && (m_column <= 8) && entered_1_column) {
                bool entered_1_row = false;
                printf("Row: ");
                get_with_number_of_chars_including_null(spacenum, 2);
                cspace = spacenum[0];
                entered_1_row = true;
                m_row = atoi(&cspace);
                if (((m_row >= 1) && (m_row <= 8)/* && enteredexactly1column*/)) {
                    landonokplace = true;
                }
                else if (!landonokplace/* && enteredexactly1column*/) {
                    printf("Invalid row.\n");
                }
            }
            else {
                printf("Invalid column.\n");
            }
        }

        if (!wrong_team(piecetomove, current_team->color) && piecefound && !did_try_tie && !did_try_castle && !did_load && landonokplace
            && !(should_load_man && (swapped_or_done))) {

            bool should_upgrade_pawn = false;
            okmove = piecetomove->can_classmove(m_row, m_column, &mainboard);

            
            //Where we actually make the move
            Move tried_move;
            tried_move.end_row = m_row;
            tried_move.end_column = m_column;
            tried_move.piece_that_moved = piecetomove;
            tried_move.start_row = piecetomove->row;
            tried_move.start_column = piecetomove->column;
            if (okmove && mainboard.is_on_board(m_row, m_column)) {
                try {
                    /*IMPORTANT TO NOTE: This function can throw errors!
                      Also, it figures out on its own what piece was landed on. */
                    mainboard.human_move_piece(&tried_move);
                }
                catch (InvalidMove problem) {
                    printf(problem.what());
                    break;
                }
                catch (InvalidPiece problem) {
                    printf(problem.what());
                    break;
                }
                //Important: Upgrade the pawn if needed.
                if (piecetomove->piecetype == TYPE::PAWN) {
                    upgrade_pawn_if_needed((Pawn*)piecetomove, current_team, &mainboard);
                }
                
                did_try_castle = false;
                //Was that move safe? IF not, I am still in check.
                //The kings might want to land on each other so they can hug.
                if(whiteteam.the_king.row == blackteam.the_king.row
                    && whiteteam.the_king.column == blackteam.the_king.column) {
                    kings_want_to_hug = true;
                }
                else {
                    kings_want_to_hug = false;
                }

                Game_Status player_who_just_moved_still_in_check = mainboard.is_in_check(current_team, current_team->enemy_team);
                if (player_who_just_moved_still_in_check != Game_Status::NEUTRAL && !kings_want_to_hug) {
                    printf("That's check, silly!\n");
                    printf("Do you want to undo that move? If so, type Yes\nWith no punctuation.\n");
                    get_with_number_of_chars_including_null(nameofpiecetomove, 4);
                    
                    nameofpiecetomove[0] = toupper(nameofpiecetomove[0]);
                    for (int i = 1; i < 3; i++) {
                        nameofpiecetomove[i] = tolower(nameofpiecetomove[i]);
                    }
                    if (strcmp(nameofpiecetomove, "Yes") == 0) {
                        mainboard.undo_move(&tried_move, current_team);
                        //After this if block, the chess status will be calculated anyways, so I don't have to calculate it here.
                        //NOTE: THE TEAMS SWAP AGAIN ON THIS LINE SO THAT WHEN THEY SWAP THEY RETURN TO THE ORIGINAL TEAM
                        current_team = current_team->enemy_team;
                    }
                }


                current_team->current_status = mainboard.is_in_check(current_team, current_team->enemy_team);
                current_team->enemy_team->current_status = mainboard.is_in_check(current_team->enemy_team, current_team, true);
                //NOTE: THE TEAMS SWAP ON THIS LINE
                current_team = current_team->enemy_team;
                // END
                // */
            } else {
                saybadmove();
            }

            
        }
        else if(did_try_tie) {
            //TODO: Test this
            printf("Your opponent doen't want to quit yet.\n");
        }
        else if (piecetomove != NULL && !piecetomove->alive) {
            printf("That piece is dead! Can't move it anymore;\n");
        }

        //If you did an Alternative command this turn, reset the variables.
        did_custom_command = false;
        did_try_tie = false;
        did_try_save = false;
        did_try_castle = false;
        did_fail_loading = false;
        swapped_or_done = false;
        did_load = false;
    }

    if (whiteteam.the_king.row == blackteam.the_king.row
        && whiteteam.the_king.column == blackteam.the_king.column) {
        print_how_to_hug();
        get_with_number_of_chars_including_null(nameofpiecetomove, 4);

        nameofpiecetomove[0] = toupper(nameofpiecetomove[0]);
        for (int i = 1; i < 3; i++) {
            nameofpiecetomove[i] = tolower(nameofpiecetomove[i]);
        }
        if (strcmp("Yes", nameofpiecetomove) == 0) {
            make_kings_hug(current_team->enemy_team, public_white_team, public_black_team);
            clean_chess_input(nameofpiecetomove);
            return 0;
        }
    }
    if (!whiteteam.the_king.alive) {
        printf("Black team wins. Good game.\n");
        clean_chess_input(nameofpiecetomove);
        return 0;
    }
    if (!blackteam.the_king.alive) {
        printf("White team wins. Good game.\n");
        clean_chess_input(nameofpiecetomove);
        return 0;
    }
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file