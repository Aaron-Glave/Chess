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
    bool SaveGame(Board* active_board, Team* current_team, Team* whiteteam, Team* blackteam);
    int LoadGame(Board* mainboard, Team* whiteteam, Team* blackteam, Team** current_team_p, int* test = NULL);
    bool Dads_LoadStandardPieces(FILE *fp, Team* pTeam, Board* mainboard);
    
    //TODO: Write your own functions to save and load the standard and upgraded pawns!
    size_t Aaron_SaveOnePiece(FILE* fp, Piece* pPc);
    bool Aaron_LoadOnePiece(FILE* fp, Piece* pPc, Board* mainboard);
    bool Aaron_SaveStandardPieces(FILE* fp, Team* current_team, Board* active_board);
    bool Aaron_LoadStandardPieces(FILE* fp, Team* current_team, Board* mainboard);
    
    
};
