#include "Team.h"
#include "Board.h"
#include <stdio.h>
#include <memory.h>
//dear old Dad helped me.
void assignSavedData(Piece* piece_to_load, Piece* saved_data) {
	//Assign the data from the piece to load row, column, count, and alive.
	piece_to_load->row = saved_data->row;
	piece_to_load->column = saved_data->column;
	piece_to_load->count = saved_data->count;
	piece_to_load->alive = saved_data->alive;
	piece_to_load->team = saved_data->team;
}

const unsigned char g_cWhitesTurn = (unsigned char)0x08;
const unsigned char g_cBlacksTurn = (unsigned char)0x04;
const unsigned char g_cWhiteInCheck = (unsigned char)0x02;
const unsigned char g_cBlackInCheck = (unsigned char)0x01;
bool SaveGame(FILE* fp, Team *whiteteam, Team* blackteam, Team* current_team) {
    int i;

    if (fp == NULL)
        return false;

    for (i = 0; i < 16; i++)
        fwrite(whiteteam->pieces[i], sizeof(Piece), 1, fp);

    for (i = 0; i < 16; i++)
        fwrite(blackteam->pieces[i], sizeof(Piece), 1, fp);

    // Save whose turn it was & whether a king was in check
    unsigned char cStatus = (unsigned char)0x00000000;

    if (current_team == whiteteam)        cStatus |= (unsigned char)g_cWhitesTurn;
    else if (current_team == blackteam)   cStatus |= (unsigned char)g_cBlacksTurn;
    if (whiteteam->current_status == CHECK) cStatus |= g_cWhiteInCheck;
    if (blackteam->current_status == CHECK) cStatus |= g_cBlackInCheck;

    //    printf("Final Status = %d\n", cStatus);
    fwrite(&cStatus, sizeof(cStatus), 1, fp);
    fclose(fp);

    return true;
}

bool LoadGame(FILE* fp, Board* mainboard, Team* whiteteam, Team* blackteam, Team** current_team)
{
    bool bReturn = true;
    int i;
    size_t nRC;
    unsigned char data[sizeof(Piece) + 1]; // +1 for safety margin 
    Piece* pPc;

    if (fp == NULL)
        return false;

    for (i = 0; i < 16; i++)
    {
        memset(data, 0, sizeof(data));
        nRC = fread(data, sizeof(Piece), 1, fp);
        if (nRC != 1)
        {
            bReturn = false;
            break;
        }

        pPc = (Piece*)&data;

        mainboard->spaces[whiteteam->pieces[i]->row - 1][whiteteam->pieces[i]->column - 1] = NULL;
		assignSavedData(whiteteam->pieces[i], pPc);
        if (pPc->alive)
            mainboard->spaces[pPc->row - 1][pPc->column - 1] = whiteteam->pieces[i];
    }

    for (i = 0; i < 16; i++)
    {
        memset(data, 0, sizeof(data));
        nRC = fread(data, sizeof(Piece), 1, fp);
        if (nRC != 1)
        {
            bReturn = false;
            break;
        }

        pPc = (Piece*)&data;

        mainboard->spaces[blackteam->pieces[i]->row - 1][blackteam->pieces[i]->column - 1] = NULL;
		assignSavedData(blackteam->pieces[i], pPc);
        if (pPc->alive)
            mainboard->spaces[pPc->row - 1][pPc->column - 1] = blackteam->pieces[i];
    }

    unsigned char cStatus = (unsigned char)0x00000000;

    if (1 == fread(&cStatus, sizeof(unsigned char), 1, fp))
    {
        if (cStatus & g_cWhitesTurn)   current_team = &whiteteam;
        if (cStatus & g_cBlacksTurn)   current_team = &blackteam;
        if (cStatus & g_cWhiteInCheck) whiteteam->current_status = CHECK;
        if (cStatus & g_cBlackInCheck) blackteam->current_status = CHECK;
    }
    else bReturn = false;

    fclose(fp);
    //    printf("Status = %d\n", cStatus);
    return bReturn;
}



// chow for now - DAD