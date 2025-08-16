#pragma once
#include "SpacelessName.h"
#include "Piece.h"
#include "Safety.h"
#include <cstring>
#include <tuple>
#include <stdio.h>
#include <ctype.h>
//Disable errors caused by scanf
#pragma warning(disable:4996)

void get_name(char* long_input)
//NOTE: long_input should be at least 16 characters long.
//Also, this function clears the input buffer.
{
    std::ignore = scanf("%16[^\n]", long_input);
    clearinput();
}

void get_standardized_name(char* messy_input, char* command_to_run) 
/* Takes an input string that may have spaces and newlines before and after the actual command,
 * and copies the actual command into command_to_run, which should be a char array of at least Piece::name_length characters.
 * Also corrects the capitalization of the command.
 * At the end of this function, command_to_run will be at most Piece::name_length-1 characters long, with a null terminator at the end.
 * Note: The command_to_run must have at least Piece::name_length characters!
*/
{
    remove_spaces(messy_input, messy_input);
    messy_input[Piece::name_length - 1] = '\0';
    for(int i = 0; i < Piece::name_length; i++) {
        command_to_run[i] = messy_input[i];
        if (messy_input[i] == '\0') {
            break;
        }
    }
    
    //Make the name all lowercase, except the second letter.
    messy_input[0] = tolower(messy_input[0]);
    messy_input[1] = toupper(messy_input[1]);
    for (int i = 2; i < Piece::name_length; i++) {
        messy_input[i] = tolower(messy_input[i]);
    }
    strcpy(command_to_run, messy_input);
}

void get_with_length(char* enter_input, int max_length) {
    //Gets user input, clears spaces, and makes sure it is no longer than max_length.
    char long_input[16];
    get_name(long_input);
    remove_spaces(long_input, long_input);
    long_input[max_length] = '\0';
    for(int i = 0; i < max_length && long_input[i] != '\0'; i++) {
        enter_input[i] = long_input[i];
    }

}

void clean_chess_input(char* in_name) {
    char long_input[16];
    get_name(long_input);
    get_standardized_name(long_input, in_name);
}
