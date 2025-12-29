
#include <stdio.h>
#include <ctype.h>

#include "SpacelessChessInput.h"

int column_from_char(char entered_char) {
    entered_char = toupper(entered_char);
    if(entered_char >= '1' && entered_char <= '8') {
        return int(entered_char - '0');
    }
    entered_char = toupper(entered_char);
    int calculated_column = entered_char - int('A') + 1;
    if (!(calculated_column >= 1 && calculated_column <= 8)) {
        return -1;
    }
    else return calculated_column;
}

char char_from_column(int column) {
    if (column < 1 || column > 8) {
        return '\0';
    }
    return char(int('A') + (column - 1));
}

int get_column() {
    char column_input[2] = "";
    get_with_number_of_chars_including_null(column_input, 2);
    
    return column_from_char(column_input[0]);
}
