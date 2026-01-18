#pragma once
#include "Piece.h"
#include "Team.h"
#include "Board.h"
#define _CRT_SECURE_NO_WARNINGS //prevents warnings for using non-Microsoft functions
class Saver {
private:
    const char* Saver_savefile;
public:
    Saver();
    int GetPieceCount(Piece* pPc);
    bool Dads_SaveGame(Board* active_board, Team* current_team, Team* whiteteam, Team* blackteam);
    bool Dads_LoadStandardPieces(FILE *fp, Team* pTeam, Board* mainboard);
    int Dads_LoadGame(Board* mainboard, Team* blackteam, Team* whiteteam, Team** current_team_p, int* test = NULL);
    bool LoadStandardPieces(FILE* fp, Team* pTeam, Board* mainboard);
    const char* GetPieceName(Piece* pExistingPiece);
};
