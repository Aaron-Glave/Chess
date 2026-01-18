#include <string.h>
#include <cstdio>
#include "Saver.h"
#pragma warning(disable:4996)

Saver::Saver() {
    //This class only has constant variables, specifically the savefile name.
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

//Used to save important boolean vaiarables as bits.
const unsigned char g_cWhitesTurn = (unsigned char)0x08;
const unsigned char g_cBlacksTurn = (unsigned char)0x04;
const unsigned char g_cWhiteInCheck = (unsigned char)0x02;
const unsigned char g_cBlackInCheck = (unsigned char)0x01;

/*Saves the entire current status of the game.
* The contents of the save file this, in order:
* Step 1: Current turn number (int)
* Step 2: The number of upgraded pawns on either team (int)
* Step 3: Save 2 Piece arrays, one for each team, with all 16 pieces,
*    including promoted pawns! (2 * (16 * sizeof(Piece)) )
*    The dead pawns don't need to be saved because they can't be brought back after saving when they're dead.
* Step 4: A character with 4 bits reprenting:
*    - Bit 4: 1 if it's the white team's turn, 0 if it's the black team's turn
*    - Bit 3: 1 if it's the black team's turn, 0 if it's the white team's turn
*    - Bit 2: 1 if the white king is in check, 0 if not
*    - Bit 1: 1 if the black king is in check, 0 if not
* Step 5: For each upgraded pawn on either team, a Piece structure.
*    The Piece structure contains all the info we need to fully rebuild the upgrade.
* Step 6: Save en passant info.
*   Note that you'll need the pawn's get_start_column() to figure out which pawn just moved 2. 
*   It will be a valid value if and only if the pawn pointer is not NULL.
* Final step: Save a dummy variable to test saving/loading
*/
bool Saver::Dads_SaveGame(Board* active_board, Team* current_team, Team* whiteteam, Team *blackteam)
//NOTE: A VARIABLE NUMBER OF UPGRADED PAWNS IS SAVED, AND I COUNT HOW MANY THERE ARE BEFORE I SAVE THEM.
{
    int i;
    FILE* fp = fopen(Saver_savefile, "w");

    if (fp == NULL)
        return false;

    int current_turn_count = active_board->current_turn();
    // Step 1
    // Save the current turn number first.
    fwrite(&current_turn_count, sizeof(int), 1, fp);
    // End Step 1

    // Step 2
    // Count the number of upgraded pawns and save the number.
    int upgraded_pawn_count = 0;
    for (i = 0; i < 8; i++)
    {
        if (whiteteam->upgraded_pieces[i] != NULL) {
            upgraded_pawn_count++;
        }

        if (blackteam->upgraded_pieces[i] != NULL) {
            upgraded_pawn_count++;
        }
    }
    fwrite(&upgraded_pawn_count, sizeof(int), 1, fp);
    // End Step 2
    
    // Step 3
    // Save white pieces (which also includes promoted pawns)
    for (i = 0; i < 16; i++)
        fwrite(whiteteam->pieces[i], sizeof(Piece), 1, fp);

    // Save black pieces (which also includes promoted pawns)
    for (i = 0; i < 16; i++)
        fwrite(blackteam->pieces[i], sizeof(Piece), 1, fp);
    // End Step 3

    // Begin Step 4: Use a character with 4 bits to represent important booleans.
    // Namely, save whose turn it was & whether a king was in check.
    unsigned char cStatus = (unsigned char)0x00000000;

    // Bit 4: Is it the white team's turn?
    if (current_team == whiteteam)        cStatus |= g_cWhitesTurn;
    // Bit 3: Is it the black team's turn?
    else if (current_team == blackteam)   cStatus |= g_cBlacksTurn;
    // Bit 2: Is the white king in check?
    if (whiteteam->current_status == Game_Status::CHECK) cStatus |= g_cWhiteInCheck;
    // Bit 1: Is the black king in check?
    if (blackteam->current_status == Game_Status::CHECK) cStatus |= g_cBlackInCheck;
    fwrite(&cStatus, sizeof(cStatus), 1, fp);
    // End Step 4

    // Step 5: Save the upgraded pawns
    for (i = 0; i < 8; i++)
    {
        if (whiteteam->upgraded_pieces[i] != NULL)
            fwrite(whiteteam->upgraded_pieces[i], sizeof(Piece), 1, fp);

        if (blackteam->upgraded_pieces[i] != NULL)
            fwrite(blackteam->upgraded_pieces[i], sizeof(Piece), 1, fp);
    }
    // End Step 5

    // Step 6: Save en passant info
    // After all that, now we can save an en passant!
    // Save the current passant pawn, saving a PassantPawn() if there
    // is no real passant pawn.
    // Note that you'll need the pawn's get_start_column() to figure out
    // which piece you're referring to.
    // It will be a valid value if and only if the pawn pointer is not NULL.
    // Our saved pointer is not valid, but when we can still tell
    // whether or not the passant pawn moved based on whether or not the
    // Pawn pointer is null..
    fwrite(&active_board->passantpawn, sizeof(PassantPawn), 1, fp);
    // End Step 6

    // Final step: Save a dummy variable to test saving/loading
    int test = 1;
    fwrite(&test, sizeof(int), 1, fp); 
    fclose(fp);
    // End final step. All done
    return true;
}

/*Returns 0 if successful, 1 if failure, -1 if the save file doesn't exist.*/
int Saver::Dads_LoadGame(Board* mainboard, Team* blackteam, Team* whiteteam, Team** current_team_p, int* test)
{
    FILE* fp = NULL;
    fp = fopen(Saver_savefile, "r");
    //Assume optimistic success.
    int return_value = 0;
    int i;
    size_t nRC;
    unsigned char data[sizeof(Piece) + 1]; // +1 for safety margin 
    Piece* pPc;
    int current_turn_count = 0;

    if (fp == NULL) {
        return -1;
    }

    int nRow, nCol;
    for (nRow = 0; nRow < 8; nRow++) // Clear the board
    {
        for (nCol = 0; nCol < 8; nCol++)
            mainboard->spaces[nRow][nCol] = NULL;

        if (whiteteam->upgraded_pieces[nRow] != NULL) delete whiteteam->upgraded_pieces[nRow];
        if (blackteam->upgraded_pieces[nRow] != NULL) delete blackteam->upgraded_pieces[nRow];

        whiteteam->upgraded_pieces[nRow] = blackteam->upgraded_pieces[nRow] = NULL;
    }

    //Step 1
    //Load the current turn count HERE.
    nRC = fread(&current_turn_count, sizeof(int), 1, fp);
    if (nRC != 1)
    {
        fclose(fp);
        return 1;
    }
    mainboard->set_turn(current_turn_count);
    //End Step 1

    //Step 2
    //Load the total number of upgraded pawns HERE.
    int upgraded_pawn_count = 0;
    nRC = fread(&upgraded_pawn_count, sizeof(int), 1, fp);
    if (nRC != 1)
    {
        fclose(fp);
        return 1;
    }
    //End Step 2

    //Step 3
    //Load standard pieces for both teams HERE.
    if (false == Dads_LoadStandardPieces(fp, whiteteam, mainboard)) {
        fclose(fp);
        return 1;
    }
    if (false == Dads_LoadStandardPieces(fp, blackteam, mainboard)) {
        fclose(fp);
        return 1;
    }
    //End Step 3

    //Step 4
    //Load important booleans from a character with 4 bits HERE.
    unsigned char cStatus = (unsigned char)0x00000000;

    if (1 == fread(&cStatus, sizeof(unsigned char), 1, fp))
    {
        // printf("Status = %d\n", cStatus);
        if (cStatus & g_cWhitesTurn)   *current_team_p = whiteteam;
        if (cStatus & g_cBlacksTurn)   *current_team_p = blackteam;
        if (cStatus & g_cWhiteInCheck) whiteteam->current_status = Game_Status::CHECK;
        if (cStatus & g_cBlackInCheck) blackteam->current_status = Game_Status::CHECK;
    }
    else {
        return_value = 1;
    }
    //End Step 4

    //Step 5
    //Load upgraded pawns HERE.
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

    // And now read in & create any promoted pawns.
    // We only save upgraded pawns we actually made.
    // Because we saved how many upgraded pawns there are,
    // we know exactly how many pieces we have to read in.
    for (int i = 0; i < upgraded_pawn_count; i++)
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
            if (pPc->alive)
                mainboard->spaces[pPc->row - 1][pPc->column - 1] = pNewPiece;
        }
    }

    //After that, now we can load an en passant!
    //We don't save the en passant piece, but we save the column it is in.
    PassantPawn saved_passant;
    fread(&saved_passant, sizeof(PassantPawn), 1, fp);
    int passant_column = saved_passant.get_column();
    int passant_row = saved_passant.get_row();

    if(saved_passant.get_row() != -1)
    {    
        if (passant_row == 3) { //We know this is a white passant pawn.
            saved_passant = PassantPawn(&whiteteam->pawns[passant_column - 1], passant_row, passant_column, saved_passant.get_turn_made());
        }
        else { //Since we know this passant pawn is real and not white, it's a black passant pawn.
            saved_passant = PassantPawn(&blackteam->pawns[8 - passant_column], passant_row, passant_column, saved_passant.get_turn_made());
        }
    }
    mainboard->passantpawn = saved_passant; // Set the mainboard's passant pawn to the saved one.
    

    //Or run a test to prove that the upgraded pieces do not half to be saved last.
    int score = 0;
    fread(&score, sizeof(int), 1, fp); // Load the a dummy value for the test.
    if (test != NULL)
    {
        *test = score; // This is just a test to prove that the upgraded pieces do not have to be saved last.
    }
    
    fclose(fp);

    return return_value;
}

bool Saver::LoadStandardPieces(FILE* fp, Team* pTeam, Board* mainboard) {
    size_t nRC = 0;
    unsigned char pawn_to_save[sizeof(Pawn)];
    for (int i = 0; i < 8; i++) {
        nRC = fread(pawn_to_save, sizeof(Pawn), 1, fp);
        if (nRC != 1) printf("Error reading pawn %d for team %s\n", i + 1, pTeam->team_name());
        Pawn* pPc = (Pawn*)&pawn_to_save;
        pTeam->pawns[i].AssignSavedData(pPc);
    }

    return true;
}

bool Saver::Dads_LoadStandardPieces(FILE* fp, Team* pTeam, Board *mainboard)
// Loads the 16 standard pieces for the given team from the given file pointer.
// Returns true on success, false on failure to load a pawn.
// TODO: PROBLEM: NOT ALL PAWNS WERE SAVED! WE NEED TO ENSURE EVEN THE DEAD PAWNS ARE SAVED!
{
    size_t nRC;
    unsigned char data[sizeof(Piece) + 1]; // +1 for safety margin 
    Piece* pPc = NULL;

    for (int i = 0; i < 16; i++)
    {
        memset(data, 0, sizeof(data));
        nRC = fread(data, sizeof(Piece), 1, fp);
        if (nRC != 1)
            return false;

        pPc = (Piece*)&data;

        /* Since upgraded pawns are assigned to the pieces array, 
        * we may be reading in a promoted pawn; if we do, kill the original pawn.
        * We will re-read the promoted pawn later.
        // */
        if (pPc->piecetype != pTeam->pieces[i]->piecetype)
        {
            // If it's a promoted pawn we kill the original pawn and skip loading it
            pTeam->pieces[i]->alive = false;
            continue;
        }

        pTeam->pieces[i]->AssignSavedData(pPc);
        // No Zombies! Only place alive pieces on the board.
        if (pPc->alive)
            mainboard->spaces[pPc->row - 1][pPc->column - 1] = pTeam->pieces[i];
    }

    return true;
}

const char* Saver::GetPieceName(Piece* pExistingPiece)
{
    return Piece::get_type_name(pExistingPiece->piecetype);
}
