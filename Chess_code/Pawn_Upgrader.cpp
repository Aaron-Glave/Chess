#include "Team.h"
#include <stdio.h>
#include <string.h>
#include "ctype.h"
#include "Pawn_Upgrader.h"
#include "Board.h"
#pragma warning(disable:4996)
bool is_pawn(Piece* shouldbepawn) {
	return shouldbepawn->piecetype == PAWN;
}
//NOTE: This function is called EVERY time a pawn is moved, and it does nothing is the pawn is not on it.
TYPE upgrade_pawn_if_needed(Piece* to_upgrade, Team* team_owner, Board* mainboard) {
	//TODO Add other playable classes. Like Queen.
	if (!is_pawn(to_upgrade)) {
		return EMPTY;
	}
	const char* classes[] = { "Rook",  "Bishop", "Knight" };
	/*NOTE: Pawns are in posisitions 2:1-2:8,
	        which means the team treats them as pieces 8-15*/
	int final_pawn_row = 8;
	if (to_upgrade->team == BLACK) {
		final_pawn_row = 1;
	}
	if (to_upgrade->row != final_pawn_row) {
		return EMPTY;
	}
	//SELECT THE PIECE BASED ON USER INPUT
	TYPE upgrade_to = get_valid_upgrade_type();
	really_perform_upgrade(to_upgrade, upgrade_to, team_owner, mainboard);
	return upgrade_to;
}

TYPE get_valid_upgrade_type() {
	TYPE to_upgrade_to = EMPTY;
	bool valid_type = false;
	char typewanted[10];
	for (int i = 0; i < 10; i++) {
		typewanted[i] = '\0';
	}
	while (!valid_type) {
		scanf("%9s", typewanted);
		typewanted[0] = toupper(typewanted[0]);
		for (int i = 1; i < 10; i++) {
			typewanted[i] = tolower(typewanted[i]);
		}
		if (strcmp(typewanted, "Rook") == 0) {
			to_upgrade_to = ROOK;
			valid_type = true;
		}
		else if (strcmp(typewanted, "Knight") == 0) {
			to_upgrade_to = KNIGHT;
			valid_type = true;
		}
		else if (strcmp(typewanted, "Bishop") == 0) {
			to_upgrade_to = BISHOP;
			valid_type = true;
		}
		/* TODO: Uncomment this once you've actually made queens.
		else if (strcmp(typewanted, "Queen") == 0) {
			to_upgrade_to = QUEEN;
			valid_type = true;
		}*/
		if (!valid_type) {
			printf("%s is not a valid type to upgrade a pawn to. Try again.", typewanted);
		}
	}
	return to_upgrade_to;
}

void place_upgraded_piece(Team* team_owner, Piece* pawn_i_was, const char *newpiece_type, Piece* upgraded_piece, Board* mainboard) {
	//CREDIT: Dad reminded me that I can tell what type of piece a Piece pointer points to based on it class,
	//so I don't need a different array for each possible piece type.
	pawn_i_was->alive = false;
	//This may not be needed, but it is safe.
	upgraded_piece->alive = true;
	sprintf(upgraded_piece->name, "%c%sp%d", pawn_i_was->team, newpiece_type, pawn_i_was->count);
	team_owner->pieces[pawn_i_was->count + 7] = upgraded_piece;
	mainboard->place(upgraded_piece, upgraded_piece->row, upgraded_piece->column);
}

void really_perform_upgrade(Piece* to_upgrade, TYPE new_class, Team* team_owner, Board* mainboard) {
	for (int i = 8; i < 16; i++) {
		if (team_owner->pieces[i] == to_upgrade) {
			//TODO MAKE ARRAYS OF UPGRADED PIECE POINTERS AND SET THE NEW PIECES TO THEM INSTEAD OF NORMAL PIECES
			switch (new_class)
			{
			case BISHOP: 
				team_owner->upgraded_pieces[i - 8] = new Bishop(to_upgrade->team, to_upgrade->row, to_upgrade->column, to_upgrade->count);
				place_upgraded_piece(team_owner, to_upgrade, "Bishop", team_owner->upgraded_pieces[i - 8], mainboard);
				break;

			case KNIGHT:
				team_owner->upgraded_pieces[i - 8] = new Knight(to_upgrade->team, to_upgrade->row, to_upgrade->column, to_upgrade->count);
				place_upgraded_piece(team_owner, to_upgrade, "Knight", team_owner->upgraded_pieces[i - 8], mainboard);
				break;

			case ROOK: 
				team_owner->upgraded_pieces[i - 8] = new Rook(to_upgrade->team, to_upgrade->row, to_upgrade->column, to_upgrade->count);
				place_upgraded_piece(team_owner, to_upgrade, "Rook", team_owner->upgraded_pieces[i - 8], mainboard);
				break;

			//TODO MAKE UPGRADING TO QUEEN POSSIBLE

			default:
				break;
			}
		}
	}
}