#include "Team.h"
void sleep5();
void get_name(char* long_input);
void standardize_name(char* in_name);
int chess(bool should_load = false, bool show_debugging = false, bool show_hugging = false);
bool make_kings_hug(Team* current_team, Team* whiteteam, Team* blackteam);
