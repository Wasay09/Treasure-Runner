#include <stdlib.h>
#include <string.h>
#include "room.h"

static void overlay_tiles(const Room *r, const Charset *charset, char *buffer);

Room *room_create(int id, const char *name, int width, int height){
	Room *room = malloc(sizeof(Room));
	if(room == NULL){
		return NULL;
	}
	room->name = NULL;
	room->floor_grid = NULL;
	room->portals = NULL;
	room->portal_count = 0;
	room->treasures = NULL;
	room->treasure_count = 0;
	room->pushables = NULL;
	room->pushable_count = 0;
	room->switches = NULL;
	room->switch_count = 0;

	if(name != NULL){
		room->name = malloc(sizeof(char) * (strlen(name) + 1));
		if(room->name == NULL){
			free(room);
			return NULL;
		}
		strcpy(room->name, name);
	}
	room->id = id;

	if(width <= 0){
		room->width = 1;
	}
	else{
		room->width = width;
	}

	if(height <= 0){
		room->height = 1;
	}
	else{
		room->height = height;
	}

	return room;
}

int room_get_width(const Room *r){
	if(r == NULL){
		return 0;
	}
	return r->width;
}

int room_get_height(const Room *r){
	if(r == NULL){
		return 0;
	}
	return r->height;
}

Status room_set_floor_grid(Room *r, bool *floor_grid){
	if(r == NULL){
		return INVALID_ARGUMENT;
	}
	free(r->floor_grid);
	r->floor_grid = floor_grid;
	return OK;
}

Status room_set_portals(Room *r, Portal *portals, int portal_count){
	if(r == NULL){
		return INVALID_ARGUMENT;
	}

	if(portal_count > 0 && portals == NULL){
		return INVALID_ARGUMENT;
	}

	if(r->portals != NULL){
		for(int jj = 0; jj < r->portal_count; jj++){
			free(r->portals[jj].name);
		}
		free(r->portals);
		r->portals = NULL;
		r->portal_count = 0;
	}

	if(portal_count == 0){
		return OK;
	}

	r->portals = portals;
	r->portal_count = portal_count;
	return OK;
}

Status room_set_treasures(Room *r, Treasure *treasures, int treasure_count){
	if(r == NULL){
		return INVALID_ARGUMENT;
	}

	if(treasure_count > 0 && treasures == NULL){
		return INVALID_ARGUMENT;
	}

	if(r->treasures != NULL){
		for(int jj = 0; jj < r->treasure_count; jj++){
			free(r->treasures[jj].name);
		}
		free(r->treasures);
		r->treasures = NULL;
		r->treasure_count = 0;
	}

	if(treasure_count == 0){
		return OK;
	}

	for(int i = 0; i < r->treasure_count; i++){
		treasures[i].collected = false;
	}

	r->treasures = treasures;
	r->treasure_count = treasure_count;
	return OK;
}

Status room_place_treasure(Room *r, const Treasure *treasure){
	if(r == NULL || treasure == NULL){
		return INVALID_ARGUMENT;
	}

	Treasure *new = malloc(sizeof(Treasure) * (r->treasure_count + 1));
	if(new == NULL){
		return NO_MEMORY;
	}
	for(int ii = 0; ii < r->treasure_count; ii++){
		new[ii] = r->treasures[ii];
	}
	new[r->treasure_count] = *treasure;
	new[r->treasure_count].collected = false;
	free(r->treasures);

	r->treasures = new;
	r->treasure_count++;
	return OK;
}

int room_get_treasure_at(const Room *r, int x, int y){
	if(r == NULL){
		return -1;
	}

	for(int ii = 0; ii < r->treasure_count; ii++){
		if(r->treasures[ii].x == x && r->treasures[ii]. y == y && r->treasures[ii].collected == false){
			return r->treasures[ii].id;
		}
	}
	return -1;
}

int room_get_portal_destination(const Room *r, int x, int y){
	if(r == NULL){
		return -1;
	}
	
	for(int ii = 0; ii < r->portal_count; ii++){
		if(r->portals[ii].x == x && r->portals[ii]. y == y && r->portals[ii].target_room_id >= 0){
			return r->portals[ii].target_room_id;
		}
	}
	return -1;
}

bool room_is_walkable(const Room *r, int x, int y){
	if(r == NULL || x < 0 || y < 0 || x >= r->width || y >= r->height){
		return false;
	}

	if(room_has_pushable_at(r, x, y, NULL) == true){
		return false;
	}

	if(room_get_treasure_at(r, x, y) != -1){
		return false;
	}

	if(room_get_portal_destination(r, x, y) != -1){
		return false;
	}

	if(r->floor_grid == NULL){
		if(x == 0 || y == 0 || x == r->width - 1 || y == r->height - 1){
			return false;
		}
		return true;
	}
	return r->floor_grid[y * r->width + x];
}

RoomTileType room_classify_tile(const Room *r, int x, int y, int *out_id){
	int id = 0;
	if(r == NULL || x < 0 || y < 0 || x >= r->width || y >= r->height){
		return ROOM_TILE_INVALID;
	}

	if(room_has_pushable_at(r, x, y, &id) == true){
		if(out_id != NULL){
			*out_id = id;
		}
		return ROOM_TILE_PUSHABLE;
	}

	id = room_get_portal_destination(r, x, y);
	if(id != -1){
		if(out_id != NULL){
			*out_id = id;
		}
		return ROOM_TILE_PORTAL;
	}

	id = room_get_treasure_at(r, x, y);
	if(id != -1){
		if(out_id != NULL){
			*out_id = id;
		}
		return ROOM_TILE_TREASURE;
	}

	if(room_is_walkable(r, x, y) == true){
		return ROOM_TILE_FLOOR;
	}
	return ROOM_TILE_WALL;
}

static void overlay_tiles(const Room *r, const Charset *charset, char *buffer){
	for(int ii = 0; ii < r->treasure_count; ii++){
		if(r->treasures[ii].collected == false){
			int xx = r->treasures[ii].x;
			int yy = r->treasures[ii].y;
			if(xx >= 0 && xx < r->width && yy >= 0 && yy < r->height){
				buffer[yy * r->width + xx] = charset->treasure;
			}
		}
	}

	for(int ii = 0; ii < r->portal_count; ii++){
		int xx = r->portals[ii].x;
		int yy = r->portals[ii].y;
		if(xx >= 0 && xx < r->width && yy >= 0 && yy < r->height){
			buffer[yy * r->width + xx] = charset->portal;
		}
	}

	for(int ii = 0; ii < r->pushable_count; ii++){
		int xx = r->pushables[ii].x;
		int yy = r->pushables[ii].y;
		if(xx >= 0 && xx < r->width && yy >= 0 && yy < r->height){
			buffer[yy * r->width + xx] = charset->pushable;
		}
	}
}

Status room_render(const Room *r, const Charset *charset, char *buffer, int buffer_width, int buffer_height){
	char ch = 0;
	if(r == NULL || charset == NULL || buffer == NULL){
		return INVALID_ARGUMENT;
	}

	if(buffer_height != r->height || buffer_width != r->width){
		return INVALID_ARGUMENT;
	}

	for(int ii = 0; ii < r->height; ii++){
		for(int jj = 0; jj < r->width; jj++){
			if(room_is_walkable(r, jj, ii) == true){
				ch = charset->floor;
			}
			else{
				ch = charset->wall;
			}
			buffer[ii * r->width + jj] = ch;
		}
	}

	overlay_tiles(r, charset, buffer);
	return OK;
}

Status room_get_start_position(const Room *r, int *x_out,int *y_out){
	if(r == NULL || x_out == NULL || y_out == NULL){
		return INVALID_ARGUMENT;
	}

	if(r->portal_count > 0){
		*x_out = r->portals[0].x;
		*y_out = r->portals[0].y;
		return OK;
	}

	for(int ii = 0; ii < r->height; ii++){
		for(int jj = 0; jj < r->width; jj++){
			if(room_is_walkable(r, jj, ii) == true){
				*x_out = jj;
				*y_out = ii;
				return OK;
			}
		}
	}
	
	return ROOM_NOT_FOUND;
}

void room_destroy(Room *r){
	if(r != NULL){
		free(r->name);
		free(r->floor_grid);

		if(r->portals != NULL){
			for(int ii = 0; ii < r->portal_count; ii++){
				free(r->portals[ii].name);
			}
			free(r->portals);
		}

		if(r->treasures != NULL){
			for(int ii = 0; ii < r->treasure_count; ii++){
				free(r->treasures[ii].name);
			}
			free(r->treasures);
		}

		if(r->pushables != NULL){
			for(int ii = 0; ii < r->pushable_count; ii++){
				free(r->pushables[ii].name);
			}
			free(r->pushables);
		}
		free(r->switches);
		free(r);
	}
}

int room_get_id(const Room *r){
	if(r == NULL){
		return -1;
	}
	return r->id;
}

Status room_pick_up_treasure(Room *r, int treasure_id, Treasure **treasure_out){
	if(r == NULL || treasure_out == NULL){
		return INVALID_ARGUMENT;
	}
	if(treasure_id < 0){
		return INVALID_ARGUMENT;
	}

	for(int i = 0; i < r->treasure_count; i++){
		if(r->treasures[i].id == treasure_id){
			if(r->treasures[i].collected == true){
				return INVALID_ARGUMENT;
			}
			r->treasures[i].collected = true;
			*treasure_out = &r->treasures[i];
			return OK;
		}
	}
	return ROOM_NOT_FOUND;
}

void destroy_treasure(Treasure *t){
	if(t != NULL){
		free(t->name);
		free(t);
	}
}

bool room_has_pushable_at(const Room *r, int x, int y, int *pushable_idx_out){
	if(r == NULL){
		return false;
	}

	for(int i = 0; i < r->pushable_count; i++){
		if(r->pushables[i].x == x && r->pushables[i].y == y){
			if(pushable_idx_out != NULL){
				*pushable_idx_out = i;
			}
			return true;
		}
	}
	return false;
}

Status room_try_push(Room *r, int pushable_idx, Direction dir){
	if(r == NULL || pushable_idx < 0 || pushable_idx >= r->pushable_count){
		return INVALID_ARGUMENT;
	}
	if(dir != DIR_NORTH && dir != DIR_SOUTH && dir != DIR_EAST && dir != DIR_WEST){
		return INVALID_ARGUMENT;
	}

	int nextx = r->pushables[pushable_idx].x;
	int nexty = r->pushables[pushable_idx].y;

	if(dir == DIR_NORTH){
		nexty--;
	}
	else if(dir == DIR_SOUTH){
		nexty++;
	}
	else if(dir == DIR_EAST){
		nextx++;
	}
	else if(dir == DIR_WEST){
		nextx--;
	}

	if(nextx < 0 || nexty < 0 || nextx >= r->width || nexty >= r->height){
		return ROOM_IMPASSABLE;
	}
	if(r->floor_grid != NULL){
		if(!r->floor_grid[nexty * r->width + nextx]){
			return ROOM_IMPASSABLE;
		}
	}
	else{
		if(nextx == 0 || nexty == 0 || nextx == r->width - 1 || nexty == r->height - 1){
			return ROOM_IMPASSABLE;
		}
	}
	if(room_has_pushable_at(r, nextx, nexty, NULL)){
		return ROOM_IMPASSABLE;
	}
	if(room_get_portal_destination(r, nextx, nexty) != -1){
		return ROOM_IMPASSABLE;
	}
	if(room_get_treasure_at(r, nextx, nexty) != -1){
		return ROOM_IMPASSABLE;
	}
	
	r->pushables[pushable_idx].x = nextx;
	r->pushables[pushable_idx].y = nexty;
	return OK;
}