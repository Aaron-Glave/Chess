#pragma once
#include "SpacelessName.h"
#include "Piece.h"
void get_name_string(std::string& long_input);
void correct_with_length(std::string& input_string, char* enter_input, const int max_length);
void get_with_number_of_chars_including_null(char* in_name, int max_length);
void clean_chess_input(char* in_name);
void get_standardized_name(const char* input, char* command);
void remove_spaces_string(std::string& source, std::string& dest, const int dest_max_length);

