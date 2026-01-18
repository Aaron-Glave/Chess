#pragma once
#include "Team.h"
#include "Saver.h"

int chess(bool talk_hug = false, bool show_debugging = false, bool should_load_man = false);
bool make_kings_hug(Team* current_team, Team* whiteteam, Team* blackteam);
bool require_yes(const char* static_question);
void main_load_game(Saver* game_saver, Board* mainboard,
    Team* blackteam, Team* whiteteam, Team** current_team,
    bool* has_loaded_file
);

