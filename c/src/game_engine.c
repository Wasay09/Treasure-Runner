#include <stdlib.h>
#include <string.h>
#include "game_engine.h"
#include "world_loader.h"
#include "graph.h"
#include "room.h"
#include "datagen.h"
#include "player.h"
#include "helper.h"

static Room *find_room(const GameEngine *eng, int room_id);
static const Room *current_room(const GameEngine *eng);
static void calculate_next_pos(int x, int y, Direction dir, int *nextx, int *nexty);
static Status portal_tiles(GameEngine *eng, int pdest);
static void uncollect_treasure(Player *p, int id);

typedef struct {
	int player_x;
	int player_y;
	int player_room_id;
	int treasure_id_collected;
	int pushable_idx;
	int pushable_old_x;
	int pushable_old_y;
	int pushable_room_id;
	bool has_undo;
} UndoMove;

static UndoMove *get_undo(void);

Status game_engine_create(const char *config_file_path, GameEngine **engine_out){
	int xstart = 0;
	int ystart = 0;
	Player *pl = NULL;
	
	if(config_file_path == NULL || engine_out == NULL){
		return INVALID_ARGUMENT;
	}

	GameEngine *eng = malloc(sizeof(GameEngine));
	if(eng == NULL){
		return NO_MEMORY;
	}

	eng->graph = NULL;
	eng->player = NULL;
	eng->room_count = 0;
	eng->initial_room_id = 0;
	eng->initial_player_x = 0;
	eng->initial_player_y = 0;

	Graph *graph = NULL;
	Room *firstroom = NULL;
	int roomscount = 0;
	Charset ch;

	Status st = loader_load_world(config_file_path, &graph, &firstroom, &roomscount, &ch);
	if(st != OK){
		free(eng);
		return st;
	}

	eng->graph = graph;
	eng->charset = ch;
	eng->room_count = roomscount;

	if(firstroom == NULL){
		game_engine_destroy(eng);
		return WL_ERR_DATAGEN;
	}

	st = room_get_start_position(firstroom, &xstart, &ystart);
	if(st != OK){
		game_engine_destroy(eng);
		return st;
	}

	eng->initial_room_id = firstroom->id;
	eng->initial_player_x = xstart;
	eng->initial_player_y = ystart;

	st = player_create(firstroom->id, xstart, ystart, &pl);
	if(st != OK){
		game_engine_destroy(eng);
		return st;
	}

	eng->player = pl;
	*engine_out = eng;
	return OK;
}

void game_engine_destroy(GameEngine *eng){
	if(eng != NULL){
		player_destroy(eng->player);
		graph_destroy(eng->graph);
		free(eng);
	}
}

const Player *game_engine_get_player(const GameEngine *eng){
	if(eng == NULL){
		return NULL;
	}
	return eng->player;
}

static Status portal_tiles(GameEngine *eng, int pdest){
	int destx = 0;
	int desty = 0;
	
	Room *destroom = find_room(eng, pdest);
	if(destroom == NULL){
		return GE_NO_SUCH_ROOM;
	}

	Status st = room_get_start_position(destroom, &destx, &desty);
	if(st != OK){
		return INTERNAL_ERROR;
	}

	st = player_move_to_room(eng->player, pdest);
	if(st != OK){
		return INTERNAL_ERROR;
	}
	
	st = player_set_position(eng->player, destx, desty);
	if(st != OK){
		return INTERNAL_ERROR;
	}
	return OK;
}

Status game_engine_move_player(GameEngine *eng, Direction dir){
	int curx = 0;
	int cury = 0;
	int nextx = 0;
	int nexty = 0;
	int id = -1;
	
	if(eng == NULL || eng->player == NULL){
		return INVALID_ARGUMENT;
	}
	if(dir != DIR_NORTH && dir != DIR_SOUTH && dir != DIR_EAST && dir != DIR_WEST){
		return INVALID_ARGUMENT;
	}

	Room *curroom = (Room *)current_room(eng);
	if(curroom == NULL){
		return GE_NO_SUCH_ROOM;
	}

	Status st = player_get_position(eng->player, &curx, &cury);
	if(st != OK){
		return INTERNAL_ERROR;
	}

	calculate_next_pos(curx, cury, dir, &nextx, &nexty);
	RoomTileType tile = room_classify_tile(curroom, nextx, nexty, &id);

	if(tile == ROOM_TILE_WALL || tile == ROOM_TILE_INVALID){
		return ROOM_IMPASSABLE;
	}
	if(tile == ROOM_TILE_PUSHABLE){
		st = room_try_push(curroom, id, dir);
		if(st != OK){
			return ROOM_IMPASSABLE;
		}
	}
	if(tile == ROOM_TILE_TREASURE){
		Treasure *t = NULL;
		st = room_pick_up_treasure(curroom, id, &t);
		if(st == OK && t != NULL){
			t->collected = false;
			player_try_collect(eng->player, t);
		}
		return OK;
	}
	if(tile == ROOM_TILE_PORTAL){
		return portal_tiles(eng, id);
	}

	st = player_set_position(eng->player, nextx, nexty);
	if(st != OK){
		return INTERNAL_ERROR;
	}
	return OK;
}

Status game_engine_get_room_count(const GameEngine *eng, int *count_out){
	if(eng == NULL){
		return INVALID_ARGUMENT;
	}
	if(count_out == NULL){
		return NULL_POINTER;
	}
	*count_out = eng->room_count;
	return OK;
}

Status game_engine_get_room_dimensions(const GameEngine *eng, int *width_out, int *height_out){
	if(eng == NULL){
		return INVALID_ARGUMENT;
	}
	if(width_out == NULL || height_out == NULL){
		return NULL_POINTER;
	}
	
	const Room *room = current_room(eng);
	if(room == NULL){
		return GE_NO_SUCH_ROOM;
	}

	*width_out = room_get_width(room);
	*height_out = room_get_height(room);
	if(*width_out <= 0 || *height_out <= 0){
		return INTERNAL_ERROR;
	}
	return OK;
}

Status game_engine_reset(GameEngine *eng){
	if(eng == NULL){
		return INVALID_ARGUMENT;
	}

	if(eng->player == NULL){
		return INTERNAL_ERROR;
	}

	Status st = player_reset_to_start(eng->player, eng->initial_room_id, eng->initial_player_x, eng->initial_player_y);
	if(st != OK){
		return INTERNAL_ERROR;
	}

	const void * const *p = NULL;
	int count = 0;
	GraphStatus s = graph_get_all_payloads(eng->graph, &p, &count);
	if(s != GRAPH_STATUS_OK){
		return INTERNAL_ERROR;
	}

	for(int i = 0; i < count; i++){
		Room *r = (Room *)p[i];
		if(r != NULL){
			for(int i = 0; i < r->treasure_count; i++){
				r->treasures[i].collected = false;
			}

			for(int i = 0; i < r->pushable_count; i++){
				r->pushables[i].x = r->pushables[i].initial_x;
				r->pushables[i].y = r->pushables[i].initial_y;
			}
		}
	}
	return OK;
}

Status game_engine_render_current_room(const GameEngine *eng, char **str_out){
	int width = 0;
	int height = 0;
	int px = 0;
	int py = 0;
	int outputsize = 0;
	int idx = 0;

	if(eng == NULL || str_out == NULL){
		return INVALID_ARGUMENT;
	}

	const Room *room = current_room(eng);
	if(room == NULL){
		return INTERNAL_ERROR;
	}

	height = room_get_height(room);
	width = room_get_width(room);

	if(width <= 0 || height <= 0){
		return INTERNAL_ERROR;
	}

	char *buffer = malloc(sizeof(char) * width * height);
	if(buffer == NULL){
		return NO_MEMORY;
	}

	Status st = room_render(room, &eng->charset, buffer, width, height);
	if(st != OK){
		free(buffer);
		return INTERNAL_ERROR;
	}

	st = player_get_position(eng->player, &px, &py);
	if(st != OK){
		free(buffer);
		return INTERNAL_ERROR;
	}

	if(px >= 0 && px < width && py >= 0 && py < height){
		buffer[py * width + px] = eng->charset.player;
	}

	outputsize = (width + 1) * height + 1;
	char *output = malloc(sizeof(char) * outputsize);
	if(output == NULL){
		free(buffer);
		return NO_MEMORY;
	}

	for(int ii = 0; ii < height; ii++){
		for(int jj = 0; jj < width; jj++){
			output[idx++] = buffer[ii * width + jj];
		}
		output[idx++] = '\n';
	}
	output[idx] = '\0';

	free(buffer);
	*str_out = output;
	return OK;
}

Status game_engine_render_room(const GameEngine *eng, int room_id, char **str_out){
	int width = 0;
	int height = 0;
	int outputsize = 0;
	int idx = 0;
	
	if(eng == NULL){
		return INVALID_ARGUMENT;
	}
	if(str_out == NULL){
		return NULL_POINTER;
	}

	const Room *room = find_room(eng, room_id);
	if(room == NULL){
		return GE_NO_SUCH_ROOM;
	}

	height = room_get_height(room);
	width = room_get_width(room);

	if(width <= 0 || height <= 0){
		return INTERNAL_ERROR;
	}

	char *buffer = malloc(sizeof(char) * width * height);
	if(buffer == NULL){
		return NO_MEMORY;
	}

	Status st = room_render(room, &eng->charset, buffer, width, height);
	if(st != OK){
		free(buffer);
		return INTERNAL_ERROR;
	}

	outputsize = (width + 1) * height + 1;
	char *output = malloc(sizeof(char) * outputsize);
	if(output == NULL){
		free(buffer);
		return NO_MEMORY;
	}

	for(int ii = 0; ii < height; ii++){
		for(int jj = 0; jj < width; jj++){
			output[idx++] = buffer[ii * width + jj];
		}
		output[idx++] = '\n';
	}
	output[idx] = '\0';

	free(buffer);
	*str_out = output;
	return OK;
}

Status game_engine_get_room_ids(const GameEngine *eng, int **ids_out, int *count_out){
	const void * const *payloads = NULL;
	int countp = 0;
	
	if(eng == NULL){
		return INVALID_ARGUMENT;
	}
	if(ids_out == NULL || count_out == NULL){
		return NULL_POINTER;
	}
	if(eng->graph == NULL){
		return INTERNAL_ERROR;
	}

	GraphStatus st = graph_get_all_payloads(eng->graph, &payloads, &countp);
	if(st != GRAPH_STATUS_OK){
		return INTERNAL_ERROR;
	}

	int *id = malloc(sizeof(int) *countp);
	if(id == NULL){
		return NO_MEMORY;
	}

	for(int ii = 0; ii < countp; ii++){
		const Room *room = (const Room *)payloads[ii];
		id[ii] = room->id;
	}

	*ids_out = id;
	*count_out = countp;
	return OK;
}

void game_engine_free_string(void *ptr){
	if(ptr != NULL){
		free(ptr);
	}
}

static Room *find_room(const GameEngine *eng, int id){
	Room search;
	if(eng == NULL || eng->graph == NULL){
		return NULL;
	}
	search.id = id;
	return (Room *)graph_get_payload(eng->graph, &search);
}

static const Room *current_room(const GameEngine *eng){
	int curid = 0;
	if(eng == NULL || eng->player == NULL){
		return NULL;
	}

	curid = player_get_room(eng->player);
	return find_room(eng, curid);
}

static void calculate_next_pos(int x, int y, Direction dir, int *nextx, int *nexty){
	*nextx = x;
	*nexty = y;
	if(dir == DIR_NORTH){
		(*nexty)--;
	}
	else if(dir == DIR_SOUTH){
		(*nexty)++;
	}
	else if(dir == DIR_EAST){
		(*nextx)++;
	}
	else if(dir == DIR_WEST){
		(*nextx)--;
	}
}

Status game_engine_get_total_treasures(const GameEngine *eng, int *count_out){
	if(eng == NULL){
		return INVALID_ARGUMENT;
	}
	if(count_out == NULL){
		return NULL_POINTER;
	}
	if(eng->graph == NULL){
		return INTERNAL_ERROR;
	}

	int node_count = 0;
	const void *const *payloads = NULL;
	GraphStatus st = graph_get_all_payloads(eng->graph, &payloads, &node_count);
	if(st != GRAPH_STATUS_OK){
		return INTERNAL_ERROR;
	}

	int total = 0;
	for (int i = 0; i < node_count; i++){
		const Room *r = (const Room *)payloads[i];
		if(r != NULL){
			total += r->treasure_count;
		}
	}
	*count_out = total;
	return OK;
}

Status game_engine_player_move_on_portal(GameEngine *eng, Direction dir){
	int curx = 0;
	int cury = 0;
	int nextx = 0;
	int nexty = 0;
	int id = -1;
	
	if(eng == NULL || eng->player == NULL){
		return INVALID_ARGUMENT;
	}
	if(dir != DIR_NORTH && dir != DIR_SOUTH && dir != DIR_EAST && dir != DIR_WEST){
		return INVALID_ARGUMENT;
	}

	int cur_id = player_get_room(eng->player);
	Room *curroom = find_room(eng, cur_id);
	if(curroom == NULL){
		return GE_NO_SUCH_ROOM;
	}

	Status st = player_get_position(eng->player, &curx, &cury);
	if(st != OK){
		return INTERNAL_ERROR;
	}

	calculate_next_pos(curx, cury, dir, &nextx, &nexty);
	RoomTileType tile = room_classify_tile(curroom, nextx, nexty, &id);

	if(tile == ROOM_TILE_WALL || tile == ROOM_TILE_INVALID){
		return ROOM_IMPASSABLE;
	}
	if(tile == ROOM_TILE_PUSHABLE){
		st = room_try_push(curroom, id, dir);
		if(st != OK){
			return ROOM_IMPASSABLE;
		}
	}
	if(tile == ROOM_TILE_TREASURE){
		Treasure *t = NULL;
		st = room_pick_up_treasure(curroom, id, &t);
		if(st == OK && t != NULL){
			t->collected = false;
			player_try_collect(eng->player, t);
		}
		return OK;
	}
	if(tile == ROOM_TILE_PORTAL){
		st = player_set_position(eng->player, nextx, nexty);
		if(st != OK){
			return INTERNAL_ERROR;
		}
		return OK;
	}

	st = player_set_position(eng->player, nextx, nexty);
	if(st != OK){
		return INTERNAL_ERROR;
	}
	return OK;
}

Status game_engine_use_portal(GameEngine *eng){
	int playerx = 0;
	int playery = 0;
	int destx = 0;
	int desty = 0;
	
	if(eng == NULL || eng->player == NULL){
		return INVALID_ARGUMENT;
	}

	Status st = player_get_position(eng->player, &playerx, &playery);
	if(st != OK){
		return INTERNAL_ERROR;
	}

	int cur_id = player_get_room(eng->player);
	Room cur_search;
	cur_search.id = cur_id;
	const Room *curroom = (const Room *)graph_get_payload(eng->graph, &cur_search);
	if(curroom == NULL){
		return GE_NO_SUCH_ROOM;
	}

	int dest_id = room_get_portal_destination(curroom, playerx, playery);
	if(dest_id < 0){
		return ROOM_NO_PORTAL;
	}

	Room dest_search;
	dest_search.id = dest_id;
	Room *destroom = (Room *)graph_get_payload(eng->graph, &dest_search);
	if(destroom == NULL){
		return GE_NO_SUCH_ROOM;
	}

	st = room_get_start_position(destroom, &destx, &desty);
	if(st != OK){
		return INTERNAL_ERROR;
	}

	st = player_move_to_room(eng->player, dest_id);
	if(st != OK){
		return INTERNAL_ERROR;
	}

	st = player_set_position(eng->player, destx, desty);
	if(st != OK){
		return INTERNAL_ERROR;
	}
	return OK;
}

static UndoMove *get_undo(void){
	static UndoMove undo = {0, 0, 0, -1, -1, 0, 0, 0, false};
	return &undo;
}

Status game_engine_player_move_with_undo(GameEngine *eng, Direction dir){
	int curx = 0;
	int cury = 0;
	int nextx = 0;
	int nexty = 0;
	int id = -1;
	
	if(eng == NULL || eng->player == NULL){
		return INVALID_ARGUMENT;
	}
	if(dir != DIR_NORTH && dir != DIR_SOUTH && dir != DIR_EAST && dir != DIR_WEST){
		return INVALID_ARGUMENT;
	}

	int cur_id = player_get_room(eng->player);
	Room *curroom = find_room(eng, cur_id);
	if(curroom == NULL){
		return GE_NO_SUCH_ROOM;
	}

	Status st = player_get_position(eng->player, &curx, &cury);
	if(st != OK){
		return INTERNAL_ERROR;
	}

	calculate_next_pos(curx, cury, dir, &nextx, &nexty);
	RoomTileType tile = room_classify_tile(curroom, nextx, nexty, &id);

	if(tile == ROOM_TILE_WALL || tile == ROOM_TILE_INVALID){
		return ROOM_IMPASSABLE;
	}

	UndoMove *undo = get_undo();
	undo->has_undo = false;
	undo->treasure_id_collected = -1;
	undo->pushable_idx = -1;

	undo->player_x = curx;
	undo->player_y = cury;
	undo->player_room_id = cur_id;
	undo->has_undo = true;

	if(tile == ROOM_TILE_PUSHABLE){
		undo->pushable_idx = id;
		undo->pushable_old_x = curroom->pushables[id].x;
		undo->pushable_old_y = curroom->pushables[id].y;
		undo->pushable_room_id = cur_id;
		st = room_try_push(curroom, id, dir);
		if(st != OK){
			undo->has_undo = false;
			undo->treasure_id_collected = -1;
			undo->pushable_idx = -1;
			return ROOM_IMPASSABLE;
		}
	}
	if(tile == ROOM_TILE_TREASURE){
		Treasure *t = NULL;
		st = room_pick_up_treasure(curroom, id, &t);
		if(st == OK && t != NULL){
			t->collected = false;
			player_try_collect(eng->player, t);
			undo->treasure_id_collected = id;
		}
		return OK;
	}
	if(tile == ROOM_TILE_PORTAL){
		st = player_set_position(eng->player, nextx, nexty);
		if(st != OK){
			undo->has_undo = false;
			undo->treasure_id_collected = -1;
			undo->pushable_idx = -1;
			return INTERNAL_ERROR;
		}
		return OK;
	}

	st = player_set_position(eng->player, nextx, nexty);
	if(st != OK){
		undo->has_undo = false;
		undo->treasure_id_collected = -1;
		undo->pushable_idx = -1;
		return INTERNAL_ERROR;
	}
	return OK;
}

Status game_engine_undo_move(GameEngine *eng){
	UndoMove *undo = get_undo();

	if(eng == NULL || eng->player == NULL || !undo->has_undo){
		return INVALID_ARGUMENT;
	}

	player_set_position(eng->player, undo->player_x, undo->player_y);
	player_move_to_room(eng->player, undo->player_room_id);
	if(undo->treasure_id_collected >= 0){
		uncollect_treasure(eng->player, undo->treasure_id_collected);
	}

	if(undo->pushable_idx >= 0){
		Room *r = find_room(eng, undo->pushable_room_id);
		if(r != NULL && undo->pushable_idx < r->pushable_count){
			r->pushables[undo->pushable_idx].x = undo->pushable_old_x;
			r->pushables[undo->pushable_idx].y = undo->pushable_old_y;
		}
	}

	undo->has_undo = false;
	undo->treasure_id_collected = -1;
	undo->pushable_idx = -1;
	return OK;
}

Status game_engine_reset_current_room(GameEngine *eng){
	int startx = 0;
	int starty = 0;

	if(eng == NULL || eng->player == NULL){
		return INVALID_ARGUMENT;
	}

	int cur_id = player_get_room(eng->player);
	Room *curroom = find_room(eng, cur_id);
	if(curroom == NULL){
		return GE_NO_SUCH_ROOM;
	}

	for(int i = 0; i < curroom->treasure_count; i++){
		if(curroom->treasures[i].collected){
			uncollect_treasure(eng->player, curroom->treasures[i].id);
			curroom->treasures[i].collected = false;
		}
		curroom->treasures[i].x = curroom->treasures[i].initial_x;
		curroom->treasures[i].y = curroom->treasures[i].initial_y;
	}

	for(int i = 0; i < curroom->pushable_count; i++){
		curroom->pushables[i].x = curroom->pushables[i].initial_x;
		curroom->pushables[i].y = curroom->pushables[i].initial_y;
	}


	Status st = room_get_start_position(curroom, &startx, &starty);
	if(st != OK){
		return INTERNAL_ERROR;
	}

	st = player_set_position(eng->player, startx, starty);
	if(st != OK){
		return INTERNAL_ERROR;
	}

	UndoMove *undo = get_undo();
	undo->has_undo = false;
	undo->treasure_id_collected = -1;
	undo->pushable_idx = -1;
	return OK;
}

static void uncollect_treasure(Player *p, int id){
	if(p != NULL && id >= 0){
		for(int i = 0; i < p->collected_count; i++){
			if(p->collected_treasures[i] != NULL && p->collected_treasures[i]->id == id){
				p->collected_treasures[i]->collected = false;
				for(int j = i; j < p->collected_count - 1; j++){
					p->collected_treasures[j] = p->collected_treasures[j + 1];
				}
				p->collected_count--;
				return;
			}
		}
	}
}