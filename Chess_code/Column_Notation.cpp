
#include <stdio.h>
#include <ctype.h>

#include "SpacelessChessInput.h"

int column_from_char(char entered_char) {
    entered_char = toupper(entered_char);
    int calculated_column = entered_char - int('A') + 1;
    if (!(calculated_column >= 1 && calculated_column <= 8)) {
        return -1;
    }
    else return calculated_column;
}

int get_column() {
    char column_input[2] = "";
    get_with_number_of_chars_including_null(column_input, 2);
    
    return column_from_char(column_input[0]);
}
