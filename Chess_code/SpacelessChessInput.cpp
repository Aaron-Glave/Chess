#pragma once
#include "SpacelessName.h"
#include "Piece.h"
#include "Safety.h"
#include <cstring>
#include <tuple>
#include <stdio.h>
#include <ctype.h>
#include <cstdlib>
#include <string>
#include <iostream>
//Disable errors caused by scanf
#pragma warning(disable:4996)

void get_name(char* long_input)
//NOTE: this function clears the input buffer.
{
    size_t length = strlen(long_input);
    std::string input_size = std::to_string(length);
    std::string scanf_string = "%" + input_size + "[^\n]";
    std::ignore = scanf(scanf_string.c_str(), long_input);
    clearinput();
}

void get_name_string(std::string& long_input) {
    std::getline(std::cin, long_input);
}

void get_standardized_name(const char* messy_input, char* command_to_run) 
/* Takes an input string that may have spaces and newlines before and after the actual command,
 * and copies the actual command into command_to_run, which should be a char array of at least Piece::name_length characters.
 * Also corrects the capitalization of the command.
 * At the end of this function, command_to_run will be at most Piece::name_length-1 characters long, with a null terminator at the end.
 * Note: The command_to_run must have at least Piece::name_length characters!
*/
{
    char spaceless_name[Piece::name_length] = "";
    remove_spaces(messy_input, spaceless_name);
    spaceless_name[Piece::name_length - 1] = '\0';
    for(int i = 0; i < Piece::name_length; i++) {
        command_to_run[i] = spaceless_name[i];
        if (spaceless_name[i] == '\0') {
            break;
        }
    }
    
    //Make the name all lowercase, except the second letter.
    command_to_run[0] = tolower(spaceless_name[0]);
    command_to_run[1] = toupper(spaceless_name[1]);
    for (int i = 2; i < Piece::name_length; i++) {
        command_to_run[i] = tolower(spaceless_name[i]);
    }
}

void remove_spaces_string(std::string& source, std::string& dest) {
    //Converts an input with spaces to one without spaces. Nothing after the first word is kept.
    int source_start = 0;
    bool found_first_nonspace = false;
    for (int i = 0; source[i] != '\0'; i++) {
        if (!is_space(source[i])) {
            found_first_nonspace = true;
            source_start = i;
            break;
        }
    }
    int current_wccount = source_start;
    for (int i = 0; !is_space(source[current_wccount]); i++, current_wccount++) {
        dest[i] = source[current_wccount];
    }
    dest[current_wccount - source_start] = '\0';
}

void get_with_length(char* enter_input, const int max_length) {
    //Gets user input, clears spaces, and makes sure it is no longer than max_length.

    std::string input_string = std::string(enter_input);
    get_name_string(input_string);
    remove_spaces_string(input_string, input_string);
    
    for(int i = 0; input_string[i] != '\0'; i++) {
        enter_input[i] = input_string[i];
    }
    enter_input[max_length] = '\0';
}

void clean_chess_input(char* in_name) {
    const size_t input_length = strlen(in_name);
    std::string long_input = std::string(in_name);
    get_name_string(long_input);
    get_standardized_name(long_input.c_str(), in_name);
}
