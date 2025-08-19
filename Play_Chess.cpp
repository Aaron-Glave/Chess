#include "Chess_code\Chess.h"
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <thread>
//Main game
int main(int argc, char*argv[]) {
    // There are command line args now.
    bool talk_hug = false;
    bool should_load_man = false;
    bool show_debugging = false;
    bool two_bishops_only = false;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strcmp("--hug", argv[i]) == 0) {
                talk_hug = true;
            }
            if (strcmp("--2bishops", argv[i]) == 0) {
                two_bishops_only = true;
            }
            if (strcmp(argv[i], "/?") == 0 || strcmp(argv[i], "--help") == 0) {
                printf("In-game command load: Load the file from a saved game.\n");
                printf("> To save a game, type save\n");
                printf("> Your game will be saved in the same directory that you run this exe in\n> with the name Saved_Game.chess\n");
               
                printf("\nSpace numbering:\nThe top right square is row 8 column 8,\n and the bottom left square is row 1 column 1.\n");
                printf("> Note that when I ask for the row and column,\n> I read the numbers top-down with the white team on the bottom.\n");
                printf("> When you have to enter a row or column, you must be very precise.\n> You can't type anything after the single number character.\n");
                printf("Pro type for black team:\n> To figure out the column of your pawn,\n> (assuming it's in its starting column),\n> subtract the number in its name from 9.\n");
                
                printf("\nWhen castling is a legal move for you,\n> you can castle by typing the word castle with no punctuation.\n");
                printf("> The game will ask you whether you want to castle left or right.\n");
                printf("> Castling left always involves the rook in column 1,\n> and castling right always involves the rook in column 8.\n");
                
                printf("\nYou can surrender by typing the word surrender.\n");
                printf("Or if your opponent agrees, you can tie by typing the word tie.\n");
                printf("You can be killed. Or you can hug!\n");
                printf("You can make kings hug by helping them land on the same space and meet,\n> or by typing the word hug instead of selecting a piece.\n");
                printf("> Hugging is a secret move. Think of it as a reward for being nice.\n");
                
                printf("\nCommand line arguments:\n--help or /?: View these help messages.\n");
                printf("--hug: Display that the kings can hug when starting the actual game!\n");
                printf("--loadman: MANUALLY Set the game in its prior state.\n");
                printf("> If loading manually, I recommend noting which movement was\n> the last one you made, and taking a screenshot.\n");
                printf("> Type oteam to switch to the enemy team and move them more.\n> To say you're done setting up, type cteam.\n");
                printf("> After running cteam, you can't swap anymore.\n");
                printf("--printdebug: Print debugging messages, mainly the current turn count,\n");
                printf("> and when each piece was first moved.\n");
                printf("\nGod answered my prayers and helped me make this game. He deserves credit!\n");
                return 0;
            }
            if (strcmp("--loadman", argv[i]) == 0) {
                should_load_man = true;
            }
            if (strcmp("--printdebug", argv[i]) == 0) {
                show_debugging = true;
            }
        }
        if (talk_hug) {
            printf("You can make kings hug by helping them land on the same space and meet, or by typing the word hug instead of selecting a piece.\n");
        }
    }
    
    printf("Note that when I ask for the row and column,\nI read the numbers from a top-down perspective with the white team on the bottom.\n");
    printf("I'll tell you this exactly once:\nThe top right square is row 8 column 8 and the bottom left square is row 1 column 1.\n");
    printf("When you have to enter a row or column, you must be very precise.\nYou can't type anything after the single number character.\n");
    printf("You can be killed.\n");
    
    chess(talk_hug, show_debugging, should_load_man, two_bishops_only);
    sleep5();
    
    return 0;
}