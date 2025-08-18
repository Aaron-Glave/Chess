#ifndef SPACELESSNAME_CPP

#include "SpacelessName.h"
#include <cstdio>
#include <cstring>
bool is_space(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\0';
}

void remove_spaces(const char* source, char* dest, const int max_length) {
    //Converts an input with spaces to one without spaces. Nothing after the first word is kept.
    //Note that max_length is the NUMBER of characters in the dest array, INCLUDING the null character!
    ///That means max_length-1 should be the index of the null character.
    int source_start = 0;
    bool found_first_nonspace = false;
    for (int i = 0; (source[i] != '\0') /*&& (i < max_length)*/ ; i++) {
        if (!is_space(source[i])) {
            found_first_nonspace = true;
            source_start = i;
            break;
        }
    }
    int current_wccount = source_start;
    for(int i = 0; !is_space(source[current_wccount]) && (i < max_length); i++, current_wccount++) {
        dest[i] = source[current_wccount];
    }
    int end = current_wccount - source_start;
    if (end >= max_length) {
        end = max_length - 1;
        if(source[end] != '\0') {
            dest[0] = '\0'; //If we truncated, return an empty string.
        }
    }
    dest[end] = '\0';
}
#endif // !SPACELESSNAME_CPP