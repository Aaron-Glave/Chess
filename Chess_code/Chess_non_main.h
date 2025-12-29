#include "Piece.h"
#include "Board.h"
#include "Team.h"
#include "move_diagnolly.h"
#include "diagnoal_direction.h"
#include "Check_vs_Checkmate.h"
#include "Move.h"
#include "Pawn_Upgrader.h"
#include "CastleMove.h"
#include "Column_Notation.h"
//NOTE: The teams have access to their enemys
bool can_castle(Team* current_team, Board* mainboard, const char *direction_castle);
void tellmeaspace(Piece* piecetomove);
void saybadmove();
void say(int column, int row, Board mainboard);
COLOR char_opposing_team(COLOR team);
void say_pieces_of_team(Board* mainboard, COLOR current_team);
bool wrong_team(space piecetomove, COLOR current_team);
COLOR enemy_team(COLOR my_team);
//void do_move(Team** current_team, Board* mainboard, Move* move, Team* public_white_team, Team* public_black_team, bool switch_teams = true);
//TYPE move_pawn(Pawn* pawn_to_move, Board* mainboard, Team* current_team);
bool check_piece(Board* mainboard, Piece** piecetomove, int row, int column, char nameofpiecetomove[]);
const char* team_name(COLOR team);