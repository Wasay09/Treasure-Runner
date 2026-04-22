#ifndef HELPER_H
#define HELPER_H

#include "game_engine.h"

Status game_engine_get_total_treasures(const GameEngine *eng, int *count_out);
Status game_engine_use_portal(GameEngine *eng);
Status game_engine_player_move_on_portal(GameEngine *eng, Direction dir);
Status game_engine_undo_move(GameEngine *eng);
Status game_engine_reset_current_room(GameEngine *eng);
Status game_engine_player_move_with_undo(GameEngine *eng, Direction dir);

#endif