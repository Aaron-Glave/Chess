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
    bool enable_debug = false;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "/?") == 0 || strcmp(argv[i], "--help") == 0) {
                printf("Space numbering:\nThe bottom left square is row 1 column A,\nand the top right square is row 8 column H.\n");
                printf("You can type a number instead of A-H when selecting a column:\n> A=1, B=2, C=3, D=4, E=5, F=6, G=7, H=8.\n");
                //Do I need to say the spaces are top-down?
                //printf("> Note that when I ask for the row and column,\n> I read the numbers with the white team on the bottom.\n");
                printf("> When you have to enter a row or column, you must be very precise.\n> You can't type anything other than spaces or tabs\n  after the single number or letter character.\n");
                printf("> Spaces and tabs are unnecessary and ignored.\n");
                //printf("When entering a column, you can type either a letter A-H or a number 1-8.\n");
                printf("For columns, capitalisation doesn't matter. a and A both mean column 1.\n");
                printf("For rows only numbers 1 through 8 are accepted.\nCharacters A-H are not!\n");

                printf("\nIn-game command load: Load the file from a saved game.\n");
                printf("> To save a game, type save\n");
                printf("> Your game will be saved in the same directory that you run this exe in\n> with the name Saved_Game.chess\n");
                
                printf("Note that when I ask you to say the word Yes,\n> you must NOT type any punctuation after it if you really mean Yes\n");
                
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


                printf("--debug: Print debugging messages, mainly the current turn count,\n");
                printf("> and when each piece was first moved.\n");
                printf("> If the --debug flag was passed in the command-line arguments, you can do this:\n");
                printf("  > Type oteam to switch to the enemy team and move them more.\n");
                printf("  > To say you're done setting up, type cteam.\n");
                printf("  > After running cteam, you can't swap anymore.\n");
                
                printf("\nGod answered my prayers and helped me make this game. He deserves credit!\n");
                return 0;
            }

            if (strcmp("--hug", argv[i]) == 0) {
                talk_hug = true;
            }
            if (strcmp("--debug", argv[i]) == 0) {
                enable_debug = true;
            }
            if (strcmp("--loadman", argv[i]) == 0) {
                should_load_man = true;
            }
        }
        if (talk_hug) {
            printf("You can make kings hug by helping them land on the same space and meet,\nor by typing the word hug instead of selecting a piece.\n");
        }
    }
    
    printf("Note that when I ask for the row and column,\nI read the numbers from a top-down perspective with the white team on the bottom.\n");
    printf("For columns, you can name them with numbers from 1-8 or characters from A-H.\n");
    printf("I'll tell you this exactly once:\nThe top right square is row 8 column 8 and the bottom left square is row 1 column 1.\n");
    printf("When you have to enter a row or column, you must be very precise.\nYou can't type anything after the single number or character.\n");
    printf("You can be killed.\n");
    if (should_load_man) {
        //Swapping the current team sometimes helps you set up custom boards.
        printf("Manual loading assistance:\n");
        printf("Type cteam to say you're done loading the previous game and you're on the right team,\n");
        printf("...or type oteam to let the opponent move,\n");
        printf("...or type execute to select a piece and remove it from the board.\n");
        printf("cteam and oteam both say you're done loading the game.\n");

    }
    
    chess(talk_hug, enable_debug, should_load_man);
    
    return 0;
}