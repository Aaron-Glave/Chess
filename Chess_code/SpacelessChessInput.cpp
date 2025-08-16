#pragma once
#include "SpacelessName.h"
#include "Piece.h"
#include <cstring>
void clean_chess_input(char* messy_input, char* command_to_run) 
/* Takes an input string that may have spaces and newlines before and after the actual command,
 * and copies the actual command into command_to_run, which should be a char array of at least Piece::name_length characters.
 * At the end of this function, command_to_run will be at most Piece::name_length-1 characters long, with a null terminator at the end.
 *
Note: The command_to_run must have at least Piece::name_length characters!*/
{
    remove_spaces(messy_input, messy_input);
    messy_input[Piece::name_length - 1] = '\0';
    for(int i = 0; i < Piece::name_length; i++) {
        command_to_run[i] = messy_input[i];
        if (messy_input[i] == '\0') {
            break;
        }
    }
}