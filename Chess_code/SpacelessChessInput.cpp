#pragma once
#include "SpacelessName.h"
#include "Piece.h"
#include <cstring>
void clean_chess_input(char* input_to_clean, char* command_to_run) 
/*Note: The command_to_run must have at least Piece::name_length characters!*/
{
    remove_spaces(input_to_clean, input_to_clean);
    input_to_clean[Piece::name_length - 1] = '\0';
    for(int i = 0; i < Piece::name_length; i++) {
        command_to_run[i] = input_to_clean[i];
        if (input_to_clean[i] == '\0') {
            break;
        }
    }
}