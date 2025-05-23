#pragma once
#include "Team.h"
#include "Board.h"
void assignSavedData(Piece* piece_to_load, Piece* saved_data);
bool SaveGame(FILE* fp, Team* whiteteam, Team* blackteam, Team* current_team);
bool LoadGame(FILE* fp, Board* mainboard, Team* whiteteam, Team* blackteam, Team** current_team);