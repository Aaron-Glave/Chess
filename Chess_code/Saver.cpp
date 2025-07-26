#include <string.h>
#include <cstdio>
#include "Saver.h"
#pragma warning(disable:4996)

Saver::Saver() {
    //pass, this class only has constant variables
    Saver_savefile = "Saved_Game.chess";
}
//*
int Saver::GetPieceCount(Piece* pPc)
{
    int nCount = 0;  char i;

    if(pPc != NULL)
    {
        for(i = 0; i < strlen(pPc->name); i++)
        {
            if( (pPc->name[i] >= '0') && (pPc->name[i] <= '9') )
            {
                nCount = pPc->name[i] - '0';
                break;
            }
        }
    }

    return nCount;
} // */

const unsigned char g_cWhitesTurn = (unsigned char)0x08;
const unsigned char g_cBlacksTurn = (unsigned char)0x04;
const unsigned char g_cWhiteInCheck = (unsigned char)0x02;
const unsigned char g_cBlackInCheck = (unsigned char)0x01;

bool Saver::Dads_SaveGame(Board* active_board, Team* current_team, Team* whiteteam, Team *blackteam)
//NOTE: A VARIABLE NUMBER OF 
{
    int i;
    FILE* fp = fopen(Saver_savefile, "w");

    if (fp == NULL)
        return false;

    int current_turn_count = active_board->current_turn();
    //Save the current turn number first, and remember to load it before anything else.
    fwrite(&current_turn_count, sizeof(int), 1, fp);

    //Save the current passant pawn, saving a PassantPawn() if there is no real passant pawn.
    //Note that you'll need the pawn's get_start_column() because it is foolish to save memory ADDRESSES of pieces. 
    //It will be a valid value iff the pawn is not NULL.

    // Save standard pieces (which also includes promoted pawns)
    for (i = 0; i < 16; i++)
        fwrite(whiteteam->pieces[i], sizeof(Piece), 1, fp);

    for (i = 0; i < 16; i++)
        fwrite(blackteam->pieces[i], sizeof(Piece), 1, fp);

    // Save whose turn it was & whether a king was in check.
    // This is done AFTER saving the pieces, but BEFORE saving the upgraded pawns.
    unsigned char cStatus = (unsigned char)0x00000000;

    if (current_team == whiteteam)        cStatus |= (unsigned char)g_cWhitesTurn;
    else if (current_team == blackteam)   cStatus |= (unsigned char)g_cBlacksTurn;
    if (whiteteam->current_status == Game_Status::CHECK) cStatus |= g_cWhiteInCheck;
    if (blackteam->current_status == Game_Status::CHECK) cStatus |= g_cBlackInCheck;

    //    printf("Final Status = %d\n", cStatus);
    fwrite(&cStatus, sizeof(cStatus), 1, fp);

    // Upgraded pawns
    for (i = 0; i < 8; i++)
    {
        if (whiteteam->upgraded_pieces[i] != NULL)
            fwrite(whiteteam->upgraded_pieces[i], sizeof(Piece), 1, fp);

        if (blackteam->upgraded_pieces[i] != NULL)
            fwrite(blackteam->upgraded_pieces[i], sizeof(Piece), 1, fp);
    }
    fclose(fp);
    return true;
}

bool Saver::Dads_LoadGame(Board* mainboard, Team* blackteam, Team* whiteteam, Team** current_team_p)
{
    FILE* fp = NULL;
    fp = fopen(Saver_savefile, "r");
    bool bReturn = true;
    int i;
    size_t nRC;
    unsigned char data[sizeof(Piece) + 1]; // +1 for safety margin 
    Piece* pPc;
    int current_turn_count = 0;

    if (fp == NULL)
        return false;

    int nRow, nCol;
    for (nRow = 0; nRow < 8; nRow++) // Clear the board
    {
        for (nCol = 0; nCol < 8; nCol++)
            mainboard->spaces[nRow][nCol] = NULL;

        if (whiteteam->upgraded_pieces[nRow] != NULL) delete whiteteam->upgraded_pieces[nRow];
        if (blackteam->upgraded_pieces[nRow] != NULL) delete blackteam->upgraded_pieces[nRow];

        whiteteam->upgraded_pieces[nRow] = blackteam->upgraded_pieces[nRow] = NULL;
    }
    //Load the current turn count HERE.
    nRC = fread(&current_turn_count, sizeof(int), 1, fp);
    if (nRC != 1)
    {
        fclose(fp);
        return false;
    }
    mainboard->set_turn(current_turn_count);

    if (false == Dads_LoadStandardPieces(fp, whiteteam, mainboard)) return false;
    if (false == Dads_LoadStandardPieces(fp, blackteam, mainboard)) return false;

    unsigned char cStatus = (unsigned char)0x00000000;

    if (1 == fread(&cStatus, sizeof(unsigned char), 1, fp))
    {
        // printf("Status = %d\n", cStatus);
        if (cStatus & g_cWhitesTurn)   *current_team_p = whiteteam;
        if (cStatus & g_cBlacksTurn)   *current_team_p = blackteam;
        if (cStatus & g_cWhiteInCheck) whiteteam->current_status = Game_Status::CHECK;
        if (cStatus & g_cBlackInCheck) blackteam->current_status = Game_Status::CHECK;
    }
    else bReturn = false;

    Piece* pNewPiece = NULL;

    // First, clear any promoted pawns
    for (i = 0; i < 8; i++)
    {
        if (whiteteam->upgraded_pieces[i] != NULL)
        {
            delete whiteteam->upgraded_pieces[i];
            whiteteam->upgraded_pieces[i] = NULL;
        }
        if (blackteam->upgraded_pieces[i] != NULL)
        {
            delete blackteam->upgraded_pieces[i];
            blackteam->upgraded_pieces[i] = NULL;
        }
    }

    // And now read in & create any promoted pawns..
    // This HAS to be the last step, since we DON'T know how many promoted pawns there are.
    // We only save upgraded pawns we actually made.
    while (!feof(fp))
    {
        memset(data, 0, sizeof(data));
        nRC = fread(data, sizeof(Piece), 1, fp);
        if (nRC != 1) {
            //This happens when the file is empty or EOF is reached.
            break;
        }

        pPc = (Piece*)&data;

        Team* TheTeam = (pPc->team == COLOR::WHITE) ? whiteteam : blackteam;
        int n = GetPieceCount(pPc);

        try {
            pNewPiece = NULL;
            switch (pPc->piecetype)
            {
            case TYPE::QUEEN:  pNewPiece = new Queen(pPc->team, pPc->row, pPc->column, n);  break;
            case TYPE::ROOK:   pNewPiece = new Rook(pPc->team, pPc->row, pPc->column, n);   break;
            case TYPE::KNIGHT: pNewPiece = new Knight(pPc->team, pPc->row, pPc->column, n); break;
            case TYPE::BISHOP: pNewPiece = new Bishop(pPc->team, pPc->row, pPc->column, n); break;
            }
        }
        catch (const char* pszEx) {
            printf("EXCEPTION: while allocating %s\n%s\n", GetPieceName(pPc), pszEx);
        }
        catch (...) {
            printf("EXCEPTION: failed to allocate %s\n", GetPieceName(pPc));
        }

        if (pNewPiece != NULL)
        {
            strcpy(pNewPiece->name, pPc->name); // Update defaulted values of the new piece
            pNewPiece->alive = pPc->alive;

            TheTeam->upgraded_pieces[n - 1] = pNewPiece;
            TheTeam->pieces[n + 7] = pNewPiece;
            // PrintPieceDetails(pPc);
            if (pPc->alive)
                mainboard->spaces[pPc->row - 1][pPc->column - 1] = pNewPiece;
        }
    }
    
    fclose(fp);

    return bReturn;
}

bool Saver::Dads_LoadStandardPieces(FILE* fp, Team* pTeam, Board *mainboard)
{
    size_t nRC;
    unsigned char data[sizeof(Piece) + 1]; // +1 for safety margin 
    Piece* pPc = NULL;

    for (int i = 0; i < 16; i++)
    {
        memset(data, 0, sizeof(data));
        nRC = fread(data, sizeof(Piece), 1, fp); //THIS LINE RETURNS nRC=0 WHEN CALLED FOR THE BLACK TEAM!
        if (nRC != 1)
            return false;

        pPc = (Piece*)&data;

        // Since upgraded pawns are assigned to the pieces array we may be reading in a promoted pawn;
        if (pPc->piecetype != pTeam->pieces[i]->piecetype)
        {
            if (pTeam->pieces[i]->piecetype != TYPE::PAWN)
                return false;

            pTeam->pieces[i]->alive = false; // If it's a promoted pawn we kill the original pawn
            continue;
        }

        pTeam->pieces[i]->AssignSavedData(pPc);
        if (pPc->alive)
            mainboard->spaces[pPc->row - 1][pPc->column - 1] = pTeam->pieces[i]; // No Zombies !!!
    }

    return true;
}

const char* Saver::GetPieceName(Piece* pExistingPiece)
{
    switch (pExistingPiece->piecetype)
    {
    case TYPE::PAWN:   return "Pawn";
    case TYPE::ROOK:   return "Rook";
    case TYPE::KNIGHT: return "Knight";
    case TYPE::BISHOP: return "Bishop";
    case TYPE::QUEEN:  return "Queen";
    case TYPE::KING:   return "King";
    default:     return "";
    }
}
