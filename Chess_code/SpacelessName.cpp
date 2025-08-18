#ifndef SPACELESSNAME_CPP

#include "SpacelessName.h"
#include <cstdio>
#include <cstring>
bool is_space(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\0';
}

void remove_spaces(const char* source, char* dest) {
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
    int current_nonblank_index = source_start;
    for(int i = 0; !is_space(source[current_nonblank_index]); i++, current_nonblank_index++) {
        dest[i] = source[current_nonblank_index];
    }
    dest[current_nonblank_index - source_start] = '\0';
}
#endif // !SPACELESSNAME_CPP