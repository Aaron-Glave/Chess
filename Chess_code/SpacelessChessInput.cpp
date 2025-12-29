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

void get_name_string(std::string& long_input) {
    std::getline(std::cin, long_input);
}

void get_standardized_name(const char* messy_input, char* command_to_run) 
/* Takes an input string that may have spaces and newlines before and after the actual command,
 * and copies the actual command into command_to_run, which should be a char array of at least PIECE_NAME_LENGTH characters.
 * Also corrects the capitalization of the command.
 * At the end of this function, command_to_run will be at most PIECE_NAME_LENGTH-1 characters long, with a null terminator at the end.
 * Note: The command_to_run must have at least PIECE_NAME_LENGTH characters!
*/
{
    char spaceless_name[PIECE_NAME_LENGTH] = "";
    remove_spaces(messy_input, spaceless_name, PIECE_NAME_LENGTH);
    spaceless_name[PIECE_NAME_LENGTH - 1] = '\0';
    for(int i = 0; i < PIECE_NAME_LENGTH; i++) {
        command_to_run[i] = spaceless_name[i];
        if (spaceless_name[i] == '\0') {
            break;
        }
    }
    
    //Make the name all lowercase, except the second letter.
    command_to_run[0] = tolower(spaceless_name[0]);
    command_to_run[1] = toupper(spaceless_name[1]);
    for (int i = 2; i < PIECE_NAME_LENGTH - 1; i++) {
        command_to_run[i] = tolower(spaceless_name[i]);
    }
    command_to_run[PIECE_NAME_LENGTH - 1] = '\0';
}

void remove_spaces_string(std::string& source, std::string& dest, const int dest_max_length) {
    /*Converts an input with spaces to one without spaces.Nothing after the first word is kept.
      Since the dest_max_length character is always set to the null character,
        if you enter a word with more than max_length characters,
        every character with an index >= dest_max_length is ignored!
      Note: dest will have at most dest_max_length-1 characters, plus a null terminator.
            And source and dest may be the same string.
    // */
    int source_start = 0;
    bool found_first_nonspace = false;
    for (int i = 0; source[i] != '\0'; i++) {
        if (!is_space(source[i])) {
            found_first_nonspace = true;
            source_start = i;
            break;
        }
    }

    //Because source and dest may be the same string,
    //we need to remember where the non-whitepace string ORIGINALLY started,
    //and this loop lets us figure out where the last non-whitespace character ORIGANALLY was.
    int current_wccount = source_start;
    for (int i = 0; !is_space(source[current_wccount]); i++, current_wccount++) {
        dest[i] = source[current_wccount];
    }
    int end = current_wccount - source_start;
    if(end >= dest_max_length) {
        dest[0] = '\0'; //If we truncated, empty the dest string.
        return;
    }
    //We know end < dest_max_length so this is safe.
    dest[end] = '\0';
}

void correct_with_length(std::string& input_string, char* enter_input, const int max_length) {
    remove_spaces_string(input_string, input_string, max_length);

    for (int i = 0; (input_string[i] != '\0') && (i < max_length); i++) {
        enter_input[i] = input_string[i];
    }
    size_t end_location = strlen(input_string.c_str());
    if (end_location > max_length) {
        end_location = 0;
    }

    enter_input[end_location] = '\0';
}

void get_with_number_of_chars_including_null(char* enter_input, const int max_length) {
    /*Gets user input, clears spaces, and makes sure it's no longer than max_length.
    WARNING: enter_input must have at least max_length characters, counting the null character!
      Since the max_length character is always set to the null character,
        if you enter a word with more than max_length characters,
        every character with an index >= max_length is ignored!
    */

    std::string input_string = std::string(enter_input);
    get_name_string(input_string);
    correct_with_length(input_string, enter_input, max_length);
}



void clean_chess_input(char* in_name) {
    const size_t input_length = strlen(in_name);
    std::string long_input = std::string(in_name);
    get_name_string(long_input);
    get_standardized_name(long_input.c_str(), in_name);
}
