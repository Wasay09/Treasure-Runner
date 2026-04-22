#include <stdlib.h>
#include <string.h>
#include "world_loader.h"
#include "graph.h"
#include "room.h"
#include "datagen.h"

static int compare_fn(const void *a, const void *b);
static void destroy_fn(void *payload);
static bool *copy_grid(const DG_Room *dgroom);
static Portal *copy_portals(const DG_Room *dgroom);
static Treasure *copy_treasures(const DG_Room *dgroom);
static Pushable *copy_pushables(const DG_Room *dgroom);
static Status load_rooms_graph(Graph *graph, Room **first_room_out, int *room_count_out);
static Status create_graph_edges(Graph *graph, int room_count);
static Status create_single_room(const DG_Room *dgroom, Room **room_out);

Status loader_load_world(const char *config_file, Graph **graph_out, Room **first_room_out, int  *num_rooms_out, Charset *charset_out){
	int status = 0;
	const DG_Charset *ch = NULL;
	Graph *graph = NULL;
	Room *firstroom = NULL;
	int roomcount = 0;

	if(config_file == NULL || graph_out == NULL || first_room_out == NULL || num_rooms_out == NULL || charset_out == NULL){
		return INVALID_ARGUMENT;
	}

	status = start_datagen(config_file);
	if(status == DG_ERR_CONFIG){
		return WL_ERR_CONFIG;
	}

	if(status != DG_OK){
		return WL_ERR_DATAGEN;
	}

	ch = dg_get_charset();
	if(ch == NULL){
		stop_datagen();
		return WL_ERR_DATAGEN;
	}

	charset_out->wall = ch->wall;
	charset_out->floor = ch->floor;
	charset_out->player = ch->player;
	charset_out->treasure = ch->treasure;
	charset_out->portal = ch->portal;
	charset_out->pushable = ch->pushable;

	GraphStatus graphstatus = graph_create(compare_fn, destroy_fn, &graph);
	if(graphstatus != GRAPH_STATUS_OK){
		stop_datagen();
		return NO_MEMORY;
	}

	Status loadstatus = load_rooms_graph(graph, &firstroom, &roomcount);
	if(loadstatus != OK){
		graph_destroy(graph);
		stop_datagen();
		return loadstatus;
	}

	Status edgestatus = create_graph_edges(graph, roomcount);
	if(edgestatus != OK){
		graph_destroy(graph);
		stop_datagen();
		return edgestatus;
	}

	stop_datagen();

	*graph_out = graph;
	*first_room_out = firstroom;
	*num_rooms_out = roomcount;

	return OK;
}

static int compare_fn(const void *a, const void *b){
	const Room *ra = (const Room *)a;
	const Room *rb = (const Room *)b;
	return ra->id - rb->id;
}

static void destroy_fn(void *payload){
	if(payload != NULL){
		room_destroy((Room *)payload);
	}
}

static Portal *copy_portals(const DG_Room *dgroom){
	if(dgroom->portal_count == 0){
		return NULL;
	}

	Portal *portal = malloc(sizeof(Portal) * dgroom->portal_count);
	if(portal == NULL){
		return NULL;
	}

	for(int ii = 0; ii < dgroom->portal_count; ii++){
		portal[ii].name = NULL;
		portal[ii].id = dgroom->portals[ii].id;
		portal[ii].x = dgroom->portals[ii].x;
		portal[ii].y = dgroom->portals[ii].y;
		portal[ii].target_room_id = dgroom->portals[ii].neighbor_id;
	}

	return portal;
}

static bool *copy_grid(const DG_Room *dgroom){
	if(dgroom->floor_grid == NULL){
		return NULL;
	}

	int size = dgroom->width * dgroom->height;
	bool *grid = malloc(sizeof(bool) * size);
	if(grid == NULL){
		return NULL;
	}

	for(int ii = 0; ii < size; ii++){
		grid[ii] = dgroom->floor_grid[ii];
	}

	return grid;
}

static Treasure *copy_treasures(const DG_Room *dgroom){
	if(dgroom->treasure_count == 0){
		return NULL;
	}

	Treasure *treasure = malloc(sizeof(Treasure) * dgroom->treasure_count);
	if(treasure == NULL){
		return NULL;
	}

	for(int ii = 0; ii < dgroom->treasure_count; ii++){
		treasure[ii].id = dgroom->treasures[ii].global_id;
		treasure[ii].x = dgroom->treasures[ii].x;
		treasure[ii].y = dgroom->treasures[ii].y;
		treasure[ii].starting_room_id = dgroom->id;
		treasure[ii].initial_x = dgroom->treasures[ii].x;
		treasure[ii].initial_y = dgroom->treasures[ii].y;
		treasure[ii].collected = false;

		if(dgroom->treasures[ii].name != NULL){
			treasure[ii].name = malloc(sizeof(char) * (strlen(dgroom->treasures[ii].name) + 1));
			if(treasure[ii].name == NULL){
				for(int jj = 0; jj < ii; jj++){
					free(treasure[jj].name);
				}
				free(treasure);
				return NULL;
			}
			strcpy(treasure[ii].name, dgroom->treasures[ii].name);
		}
		else{
			treasure[ii].name = NULL;
		}
	}

	return treasure;
}

static Pushable *copy_pushables(const DG_Room *dgroom){
	if(dgroom->pushable_count == 0){
		return NULL;
	}

	Pushable *pushable = malloc(sizeof(Pushable) * dgroom->pushable_count);
	if(pushable == NULL){
		return NULL;
	}

	for(int ii = 0; ii < dgroom->pushable_count; ii++){
		pushable[ii].id = dgroom->pushables[ii].id;
		pushable[ii].x = dgroom->pushables[ii].x;
		pushable[ii].y = dgroom->pushables[ii].y;
		pushable[ii].initial_x = dgroom->pushables[ii].x;
		pushable[ii].initial_y = dgroom->pushables[ii].y;

		if(dgroom->pushables[ii].name != NULL){
			pushable[ii].name = malloc(sizeof(char) * (strlen(dgroom->pushables[ii].name) + 1));
			if(pushable[ii].name == NULL){
				for(int jj = 0; jj < ii; jj++){
					free(pushable[jj].name);
				}
				free(pushable);
				return NULL;
			}
			strcpy(pushable[ii].name, dgroom->pushables[ii].name);
		}
		else{
			pushable[ii].name = NULL;
		}
	}

	return pushable;
}

static Status load_rooms_graph(Graph *graph, Room **first_room_out, int *room_count_out){
	Room *firstroom = NULL;
	int roomcount = 0;

	while(has_more_rooms()){
		DG_Room dgroom = get_next_room();
		Room *room = NULL;

		Status status = create_single_room(&dgroom, &room);
		if(status != OK){
			return status;
		}

		GraphStatus graphstatus = graph_insert(graph, room);
		if(graphstatus != GRAPH_STATUS_OK){
			room_destroy(room);
			if(graphstatus == GRAPH_STATUS_NO_MEMORY){
				return NO_MEMORY;
			}
			return INTERNAL_ERROR;
		}

		if(roomcount == 0){
			firstroom = room;
		}
		roomcount++;
	}
	*first_room_out = firstroom;
	*room_count_out = roomcount;
	return OK;
}

static Status create_graph_edges(Graph *graph, int room_count){
	Room search;

	for(int ii = 0; ii < room_count; ii++){
		const DG_Room *dgroom = get_room_by_index(ii);
		if(dgroom == NULL){
			return WL_ERR_DATAGEN;
		}

		
		search.id = dgroom->id;
		const Room *source = (const Room*)graph_get_payload(graph, &search);
		if(source == NULL){
			return INTERNAL_ERROR;
		}

		for(int jj = 0; jj < dgroom->portal_count; jj++){
			int getid = dgroom->portals[jj].neighbor_id;
			if(getid >= 0){
				Room targetkey;
				targetkey.id = getid;
				const Room *targetroom = (const Room*)graph_get_payload(graph, &targetkey);
				if(targetroom != NULL){
					GraphStatus graphstatus = graph_connect(graph, source, targetroom);
					if(graphstatus != GRAPH_STATUS_OK && graphstatus != GRAPH_STATUS_DUPLICATE_EDGE){
						return INTERNAL_ERROR;
					}
				}
			}
		}
	}
	return OK;
}

static Status create_single_room(const DG_Room *dgroom, Room **room_out){
	Room *room = room_create(dgroom->id, NULL, dgroom->width, dgroom->height);
	if(room == NULL){
		return NO_MEMORY;
	}

	bool *grid = copy_grid(dgroom);
	if(dgroom->floor_grid != NULL && grid == NULL){
		room_destroy(room);
		return NO_MEMORY;
	}

	if(room_set_floor_grid(room, grid) != OK){
		free(grid);
		room_destroy(room);
		return NO_MEMORY;
	}

	Portal *portal = copy_portals(dgroom);
	if(dgroom->portal_count > 0 && portal == NULL){
		room_destroy(room);
		return NO_MEMORY;
	}

	if(room_set_portals(room, portal, dgroom->portal_count) != OK){
		free(portal);
		room_destroy(room);
		return NO_MEMORY;
	}

	Treasure *treasure = copy_treasures(dgroom);
	if(dgroom->treasure_count > 0 && treasure == NULL){
		room_destroy(room);
		return NO_MEMORY;
	}

	if(room_set_treasures(room, treasure, dgroom->treasure_count) != OK){
		if(treasure != NULL){
			for(int jj = 0; jj < dgroom->treasure_count; jj++){
				free(treasure[jj].name);
			}
			free(treasure);
		}
		room_destroy(room);
		return NO_MEMORY;
	}

	Pushable *pushable = copy_pushables(dgroom);
	if(dgroom->pushable_count > 0 && pushable == NULL){
		room_destroy(room);
		return NO_MEMORY;
	}
	room->pushables = pushable;
	room->pushable_count = dgroom->pushable_count;

	*room_out = room;
	return OK;
}