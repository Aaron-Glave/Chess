#pragma once
#include "SpacelessName.h"
#include "Piece.h"
void get_name_string(std::string& long_input);
void get_with_length(char* in_name, int max_length);
void clean_chess_input(char* in_name);
void get_standardized_name(const char* input, char* command);

