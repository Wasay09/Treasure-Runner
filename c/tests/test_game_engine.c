#include <check.h>
#include <stdlib.h>
#include "game_engine.h"
#include "player.h"
#include "helper.h"

START_TEST(test_game_engine_create_null)
{
	GameEngine *engine = NULL;
	ck_assert_int_eq(game_engine_create(NULL, &engine), INVALID_ARGUMENT);
	ck_assert_int_eq(game_engine_create("../assets/starter.ini", NULL), INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_game_engine_create_invalid_config)
{
	GameEngine *engine = NULL;
	ck_assert_int_eq(game_engine_create("nonexistent.ini", &engine), WL_ERR_CONFIG);
}
END_TEST

START_TEST(test_game_engine_create_success)
{
	GameEngine *engine = NULL;
	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &engine), OK);
	ck_assert_ptr_nonnull(engine);
	ck_assert_ptr_nonnull(engine->graph);
	ck_assert_ptr_nonnull(engine->player);
	ck_assert(engine->room_count > 0);

	game_engine_destroy(engine);
}
END_TEST

START_TEST(test_game_engine_create_charset_loaded)
{
	GameEngine *engine = NULL;
	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &engine), OK);
	ck_assert(engine->charset.wall != '\0');
	ck_assert(engine->charset.floor != '\0');
	ck_assert(engine->charset.player != '\0');
	ck_assert(engine->charset.treasure != '\0');
	ck_assert(engine->charset.portal != '\0');

	game_engine_destroy(engine);
}
END_TEST

START_TEST(test_game_engine_destroy_null)
{
	game_engine_destroy(NULL);
}
END_TEST

START_TEST(test_game_engine_destroy_success)
{
	GameEngine *engine = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &engine), OK);
	game_engine_destroy(engine);
}
END_TEST

START_TEST(test_game_engine_get_player_NULL)
{
	ck_assert_ptr_eq(game_engine_get_player(NULL), NULL);

}
END_TEST

START_TEST(test_game_engine_get_player_success)
{
	GameEngine *eng = NULL;
	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_ptr_eq(game_engine_get_player(eng), eng->player);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_move_player_NULL)
{
	GameEngine *eng = NULL;
	Direction dir = 0;
	ck_assert_int_eq(game_engine_move_player(eng, dir), INVALID_ARGUMENT);
	ck_assert_int_eq(game_engine_move_player(NULL, dir), INVALID_ARGUMENT);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_move_player_north_success)
{
	GameEngine *eng = NULL;
	int x = 0;
	int y = 0;
	Status st;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	player_get_position(eng->player, &x, &y);

	st = game_engine_move_player(eng, DIR_NORTH);
	ck_assert(st == OK || st == ROOM_IMPASSABLE);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_move_player_south_success)
{
	GameEngine *eng = NULL;
	Status st;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	st = game_engine_move_player(eng, DIR_SOUTH);
	ck_assert(st == OK || st == ROOM_IMPASSABLE);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_move_player_east_success)
{
	GameEngine *eng = NULL;
	Status st;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	st = game_engine_move_player(eng, DIR_EAST);
	ck_assert(st == OK || st == ROOM_IMPASSABLE);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_move_player_west_success)
{
	GameEngine *eng = NULL;
	Status st;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	st = game_engine_move_player(eng, DIR_WEST);
	ck_assert(st == OK || st == ROOM_IMPASSABLE);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_move_player_multiple_moves)
{
	GameEngine *eng = NULL;
	Status st1;
	Status st2;
	Status st3;
	Status st4;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	st1 = game_engine_move_player(eng, DIR_NORTH);
	st2 = game_engine_move_player(eng, DIR_SOUTH);
	st3 = game_engine_move_player(eng, DIR_EAST);
	st4 = game_engine_move_player(eng, DIR_WEST);
	ck_assert(st1 == OK || st1 == ROOM_IMPASSABLE);
	ck_assert(st2 == OK || st2 == ROOM_IMPASSABLE);
	ck_assert(st3 == OK || st3 == ROOM_IMPASSABLE);
	ck_assert(st4 == OK || st4 == ROOM_IMPASSABLE);
	ck_assert_ptr_nonnull(eng);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_move_player_check_position)
{
	GameEngine *eng = NULL;
	int x1 = 0;
	int y1 = 0;
	int x2 = 0;
	int y2 = 0;
	Status st;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	player_get_position(eng->player, &x1, &y1);

	st = game_engine_move_player(eng, DIR_WEST);
	if(st == OK){
		player_get_position(eng->player, &x2, &y2);
		ck_assert(x1 != x2);
		ck_assert(y1 != y2);
	}
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_get_room_count_NULL)
{
	GameEngine *eng = NULL;
	int count = 0;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_get_room_count(NULL, &count), INVALID_ARGUMENT);
	ck_assert_int_eq(game_engine_get_room_count(eng, NULL), NULL_POINTER);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_get_room_count_success)
{
	GameEngine *eng = NULL;
	int count = 0;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_get_room_count(eng, &count), OK);
	ck_assert_int_eq(count, eng->room_count);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_get_room_dimentsions_NULL)
{
	GameEngine *eng = NULL;
	int height = 0;
	int width = 0;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_get_room_dimensions(NULL, &width, &height), INVALID_ARGUMENT);
	ck_assert_int_eq(game_engine_get_room_dimensions(eng, NULL, &height), NULL_POINTER);
	ck_assert_int_eq(game_engine_get_room_dimensions(eng, &width, NULL), NULL_POINTER);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_get_room_dimentsions_success)
{
	GameEngine *eng = NULL;
	int height = 0;
	int width = 0;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_get_room_dimensions(eng, &width, &height), OK);
	ck_assert(width > 0);
	ck_assert(height > 0);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_reset_NULL)
{
	ck_assert_int_eq(game_engine_reset(NULL), INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_game_engine_reset_success)
{
	GameEngine *eng = NULL;
	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	int room1 = player_get_room(eng->player);
	int x1 = 0;
	int y1 = 0;
	player_get_position(eng->player, &x1, &y1);

	game_engine_move_player(eng, DIR_NORTH);
	game_engine_move_player(eng, DIR_EAST);

	ck_assert_int_eq(game_engine_reset(eng), OK);

	int room2 = player_get_room(eng->player);
	int x2 = 0;
	int y2 = 0;
	player_get_position(eng->player, &x2, &y2);

	ck_assert_int_eq(room2, room1);
	ck_assert_int_eq(x2, x1);
	ck_assert_int_eq(y2, y1);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_reset_multiple_resests)
{
	GameEngine *eng = NULL;
	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	ck_assert_int_eq(game_engine_reset(eng), OK);
	ck_assert_int_eq(game_engine_reset(eng), OK);
	ck_assert_int_eq(game_engine_reset(eng), OK);
	ck_assert_int_eq(game_engine_reset(eng), OK);

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_render_current_room_NULL)
{
	GameEngine *eng = NULL;
	char *str = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_render_current_room(NULL, &str), INVALID_ARGUMENT);
	ck_assert_int_eq(game_engine_render_current_room(eng, NULL), INVALID_ARGUMENT);

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_render_current_room_success)
{
	GameEngine *eng = NULL;
	char *str = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_render_current_room(eng, &str), OK);
	ck_assert_ptr_nonnull(str);
	ck_assert(strlen(str) > 0);

	free(str);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_render_current_room_newline)
{
	GameEngine *eng = NULL;
	char *str = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_render_current_room(eng, &str), OK);
	ck_assert_ptr_nonnull(str);
	
	int newline = 0;
	for(int i = 0; str[i] != '\0'; i++){
		if(str[i] == '\n'){
			newline = 1;
			break;
		}
	}
	ck_assert(newline);

	free(str);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_render_current_room_player)
{
	GameEngine *eng = NULL;
	char *str = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_render_current_room(eng, &str), OK);
	ck_assert_ptr_nonnull(str);
	
	int p = 0;
	for(int i = 0; str[i] != '\0'; i++){
		if(str[i] == eng->charset.player){
			p = 1;
			break;
		}
	}
	ck_assert(p);

	free(str);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_render_room_NULL)
{
	GameEngine *eng = NULL;
	char *str = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_render_room(NULL, 1, &str), INVALID_ARGUMENT);
	ck_assert_int_eq(game_engine_render_room(eng, 1, NULL), NULL_POINTER);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_render_room_invalid_id)
{
	GameEngine *eng = NULL;
	char *str = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_render_room(eng, 10000, &str), GE_NO_SUCH_ROOM);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_render_room_success)
{
	GameEngine *eng = NULL;
	int id = 0;
	char *str = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	id = player_get_room(eng->player);
	ck_assert_int_eq(game_engine_render_room(eng, id, &str), OK);
	ck_assert_ptr_nonnull(str);
	ck_assert(strlen(str) > 0);

	free(str);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_render_room_no_player)
{
	GameEngine *eng = NULL;
	int id = 0;
	char *str = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	
	id = player_get_room(eng->player);
	ck_assert_int_eq(game_engine_render_room(eng, id, &str), OK);
	
	int count = 0;
	for(int i = 0; str[i] != '\0'; i++){
		if(str[i] == eng->charset.player){
			count++;
		}
	}
	ck_assert_int_eq(count, 0);

	free(str);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_get_room_ids_NULL)
{
	GameEngine *eng = NULL;
	int *id = NULL;
	int count = 0;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	
	ck_assert_int_eq(game_engine_get_room_ids(NULL, &id, &count), INVALID_ARGUMENT);
	ck_assert_int_eq(game_engine_get_room_ids(eng, NULL, &count), NULL_POINTER);
	ck_assert_int_eq(game_engine_get_room_ids(eng, &id, NULL), NULL_POINTER);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_get_room_ids_invalid_graph)
{
	GameEngine *eng = NULL;
	int *id = NULL;
	int count = 0;
	Graph *graph = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	
	graph = eng->graph;
	eng->graph = NULL;

	ck_assert_int_eq(game_engine_get_room_ids(eng, &id, &count), INTERNAL_ERROR);
	eng->graph = graph;
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_get_room_ids_success)
{
	GameEngine *eng = NULL;
	int *id = NULL;
	int count = 0;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	
	ck_assert_int_eq(game_engine_get_room_ids(eng, &id, &count), OK);
	ck_assert_ptr_nonnull(id);
	ck_assert_int_eq(count, eng->room_count);

	free(id);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_get_room_ids_has_room)
{
	GameEngine *eng = NULL;
	int *id = NULL;
	int count = 0;
	int has = 0;
	int found = 0;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	
	has = player_get_room(eng->player);

	ck_assert_int_eq(game_engine_get_room_ids(eng, &id, &count), OK);
	
	for(int i = 0; i < count; i++){
		if(id[i] == has){
			found = 1;
			break;
		}
	}
	ck_assert(found);
	free(id);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_free_string_null)
{
	game_engine_free_string(NULL);
}
END_TEST

START_TEST(test_game_engine_free_string_success)
{
	GameEngine *eng = NULL;
	char *s = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_render_current_room(eng, &s), OK);
	ck_assert_ptr_nonnull(s);
	game_engine_free_string(s);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_player_move_on_portal_NULL)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_player_move_on_portal(NULL, DIR_NORTH), INVALID_ARGUMENT);
	ck_assert_int_eq(game_engine_player_move_on_portal(eng, DIR_SOUTH), INVALID_ARGUMENT);
}
END_TEST


START_TEST(test_game_engine_player_move_on_portal_valid_position_changes)
{
	GameEngine *eng = NULL;
	int x1 = 0, y1 = 0;
	int x2 = 0, y2 = 0;
	Status st;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	player_get_position(eng->player, &x1, &y1);

	st = game_engine_player_move_on_portal(eng, DIR_EAST);

	if(st == OK){
		player_get_position(eng->player, &x2, &y2);
		ck_assert(x1 != x2 || y1 != y2);
	}

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_player_move_on_portal_invalid_direction)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	ck_assert_int_eq(game_engine_player_move_on_portal(eng, 999), INVALID_ARGUMENT);
	ck_assert_int_eq(game_engine_player_move_on_portal(eng, -1), INVALID_ARGUMENT);

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_use_portal_NULL)
{
	ck_assert_int_eq(game_engine_use_portal(NULL), INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_game_engine_use_portal_not_on_portal)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	Status st = game_engine_use_portal(eng);
	ck_assert(st == OK ||st == ROOM_NO_PORTAL ||st == GE_NO_SUCH_ROOM);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_use_portal_room_success)
{
	GameEngine *eng = NULL;
	int room_before = 0;
	int room_after = 0;
	Status st;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	room_before = player_get_room(eng->player);

	st = game_engine_use_portal(eng);

	if(st == OK){
		room_after = player_get_room(eng->player);
		ck_assert_int_ne(room_before, room_after);
	}

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_player_move_on_portal_null_player)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	Player *saved = eng->player;
	eng->player = NULL;

	ck_assert_int_eq(game_engine_player_move_on_portal(eng, DIR_NORTH), INVALID_ARGUMENT);

	eng->player = saved;
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_player_move_on_portal_hits_wall)
{
	GameEngine *eng = NULL;
	Status st;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	for(int i = 0; i < 10; i++){
		st = game_engine_player_move_on_portal(eng, DIR_NORTH);
		if(st == ROOM_IMPASSABLE){
			break;
		}
	}

	ck_assert(st == ROOM_IMPASSABLE || st == OK);

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_use_portal_null_player)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	Player *saved = eng->player;
	eng->player = NULL;

	ck_assert_int_eq(game_engine_use_portal(eng), INVALID_ARGUMENT);

	eng->player = saved;
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_use_portal_position_valid)
{
	GameEngine *eng = NULL;
	Status st;
	int x = 0;
	int y = 0;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	st = game_engine_use_portal(eng);

	if(st == OK){
		ck_assert_int_eq(player_get_position(eng->player, &x, &y), OK);
	}

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_use_portal_stress)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	for(int i = 0; i < 20; i++){
		game_engine_use_portal(eng);
	}

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_player_move_on_portal_invalid_room)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	player_move_to_room(eng->player, 999999);

	ck_assert_int_eq(game_engine_player_move_on_portal(eng, DIR_NORTH),GE_NO_SUCH_ROOM);

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_player_move_on_portal_pushable)
{
	GameEngine *eng = NULL;
	Status st = OK;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	for(int i = 0; i < 50; i++){
		st = game_engine_player_move_on_portal(eng, DIR_WEST);
		if(st == OK){
			break;
		}
	}

	ck_assert(st == OK || st == ROOM_IMPASSABLE);

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_player_move_on_portal_internal_error)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	Player *saved = eng->player;
	eng->player = NULL;

	ck_assert_int_eq(
		game_engine_player_move_on_portal(eng, DIR_NORTH),
		INVALID_ARGUMENT
	);

	eng->player = saved;
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_use_portal_invalid_graph)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	Graph *saved = eng->graph;
	eng->graph = NULL;

	ck_assert_int_eq(
		game_engine_use_portal(eng),
		GE_NO_SUCH_ROOM
	);

	eng->graph = saved;
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_use_portal_no_position_change)
{
	GameEngine *eng = NULL;
	int x1 = 0;
	int y1 = 0;
	int x2 = 0;
	int y2 = 0;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	player_get_position(eng->player, &x1, &y1);

	Status st = game_engine_use_portal(eng);

	if(st != OK){
		player_get_position(eng->player, &x2, &y2);
		ck_assert_int_eq(x1, x2);
		ck_assert_int_eq(y1, y2);
	}

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_move_player_invalid_direction)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	ck_assert_int_eq(game_engine_move_player(eng, 999), INVALID_ARGUMENT);
	ck_assert_int_eq(game_engine_move_player(eng, -10), INVALID_ARGUMENT);

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_move_player_null_player)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	Player *saved = eng->player;
	eng->player = NULL;

	ck_assert_int_eq(game_engine_move_player(eng, DIR_NORTH), INVALID_ARGUMENT);

	eng->player = saved;
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_get_room_dimensions_after_reset)
{
	GameEngine *eng = NULL;
	int w1 = 0, h1 = 0;
	int w2 = 0, h2 = 0;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	ck_assert_int_eq(game_engine_get_room_dimensions(eng, &w1, &h1), OK);

	game_engine_reset(eng);

	ck_assert_int_eq(game_engine_get_room_dimensions(eng, &w2, &h2), OK);

	ck_assert_int_eq(w1, w2);
	ck_assert_int_eq(h1, h2);

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_render_room_multiple_times)
{
	GameEngine *eng = NULL;
	char *s1 = NULL;
	char *s2 = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	int id = player_get_room(eng->player);

	ck_assert_int_eq(game_engine_render_room(eng, id, &s1), OK);
	ck_assert_int_eq(game_engine_render_room(eng, id, &s2), OK);

	ck_assert_ptr_nonnull(s1);
	ck_assert_ptr_nonnull(s2);

	free(s1);
	free(s2);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_move_player_stress)
{
	GameEngine *eng = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	for(int i = 0; i < 50; i++){
		game_engine_move_player(eng, DIR_NORTH);
		game_engine_move_player(eng, DIR_SOUTH);
		game_engine_move_player(eng, DIR_EAST);
		game_engine_move_player(eng, DIR_WEST);
	}

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_get_room_ids_multiple_calls)
{
	GameEngine *eng = NULL;
	int *id1 = NULL;
	int *id2 = NULL;
	int c1 = 0, c2 = 0;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

	ck_assert_int_eq(game_engine_get_room_ids(eng, &id1, &c1), OK);
	ck_assert_int_eq(game_engine_get_room_ids(eng, &id2, &c2), OK);

	ck_assert_int_eq(c1, c2);

	free(id1);
	free(id2);
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_free_string_double_call)
{
	GameEngine *eng = NULL;
	char *s = NULL;

	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
	ck_assert_int_eq(game_engine_render_current_room(eng, &s), OK);

	game_engine_free_string(s);
	game_engine_free_string(NULL);

	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_treasure_count_after_move)
{
    GameEngine *eng = NULL;
    int before = 0;
    int after = 0;

    ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

    ck_assert_int_eq(game_engine_get_total_treasures(eng, &before), OK);

    game_engine_move_player(eng, DIR_NORTH);
    game_engine_move_player(eng, DIR_SOUTH);

    ck_assert_int_eq(game_engine_get_total_treasures(eng, &after), OK);

    ck_assert_int_eq(before, after);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_portal_invalid_direction_does_not_corrupt_state)
{
    GameEngine *eng = NULL;

    ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

    int before = player_get_room(eng->player);

    game_engine_player_move_on_portal(eng, 999);

    int after = player_get_room(eng->player);

    ck_assert_int_eq(before, after);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_room_count_after_portal)
{
    GameEngine *eng = NULL;
    int before = 0;
    int after = 0;

    ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

    ck_assert_int_eq(game_engine_get_room_count(eng, &before), OK);

    game_engine_use_portal(eng);

    ck_assert_int_eq(game_engine_get_room_count(eng, &after), OK);

    ck_assert_int_eq(before, after);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_move_with_undo_null_engine)
{
    ck_assert_int_eq(game_engine_player_move_with_undo(NULL, DIR_NORTH), INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_move_with_undo_invalid_direction)
{
    GameEngine *eng = NULL;
    ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

    ck_assert_int_eq(game_engine_player_move_with_undo(eng, 999),INVALID_ARGUMENT);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_move_with_undo_success)
{
    GameEngine *eng = NULL;
    ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

    const Player *p = game_engine_get_player(eng);

    int x1, y1;
    player_get_position((Player*)p, &x1, &y1);

    if (game_engine_player_move_with_undo(eng, DIR_SOUTH) == OK){
        ck_assert_int_eq(game_engine_undo_move(eng), OK);

        int x2, y2;
        player_get_position((Player*)p, &x2, &y2);

        ck_assert_int_eq(x1, x2);
        ck_assert_int_eq(y1, y2);
    }

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_undo_treasure_collection)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    int before = player_get_collected_count(game_engine_get_player(eng));

    if (game_engine_player_move_with_undo(eng, DIR_EAST) == OK){
        game_engine_undo_move(eng);

        int after = player_get_collected_count(game_engine_get_player(eng));

        ck_assert_int_eq(before, after);
    }

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_undo_pushable)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    if (game_engine_player_move_with_undo(eng, DIR_EAST) == OK){
        ck_assert_int_eq(game_engine_undo_move(eng), OK);
    }

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_undo_without_move)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    ck_assert_int_eq(game_engine_undo_move(eng), INVALID_ARGUMENT);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_reset_current_room_null)
{
    ck_assert_int_eq(game_engine_reset_current_room(NULL), INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_reset_clears_undo)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    game_engine_player_move_with_undo(eng, DIR_SOUTH);

    game_engine_reset_current_room(eng);

    ck_assert_int_eq(game_engine_undo_move(eng), INVALID_ARGUMENT);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_move_portal_tile_with_undo)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    if (game_engine_player_move_with_undo(eng, DIR_EAST) == OK){
        ck_assert_int_eq(game_engine_undo_move(eng), OK);
    }

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_undo_after_full_reset_fails)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    game_engine_player_move_with_undo(eng, DIR_SOUTH);

    game_engine_reset(eng);

    ck_assert_int_eq(game_engine_undo_move(eng), INVALID_ARGUMENT);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_reset_current_room_restores_position)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    int startx = 0;
	int starty = 0;
    player_get_position(game_engine_get_player(eng), &startx, &starty);

    game_engine_player_move_with_undo(eng, DIR_SOUTH);

    game_engine_reset_current_room(eng);

    int newx = 0;
	int newy = 0;
    player_get_position(game_engine_get_player(eng), &newx, &newy);

    ck_assert_int_eq(startx, newx);
    ck_assert_int_eq(starty, newy);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_undo_after_different_move_types)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    game_engine_player_move_with_undo(eng, DIR_EAST);
    game_engine_player_move_with_undo(eng, DIR_SOUTH);

    ck_assert_int_eq(game_engine_undo_move(eng), OK);

    ck_assert_int_eq(game_engine_undo_move(eng), INVALID_ARGUMENT);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_undo_null_engine)
{
    ck_assert_int_eq(game_engine_undo_move(NULL), INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_reset_current_room_invalidates_undo)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    game_engine_player_move_with_undo(eng, DIR_EAST);

    game_engine_reset_current_room(eng);

    ck_assert_int_eq(game_engine_undo_move(eng), INVALID_ARGUMENT);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_undo_overwrite_after_room_change)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    game_engine_player_move_with_undo(eng, DIR_EAST);
	ck_assert_int_eq(game_engine_undo_move(eng), OK);
    
	game_engine_player_move_with_undo(eng, DIR_SOUTH);
    ck_assert_int_eq(game_engine_undo_move(eng), INVALID_ARGUMENT);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_undo_does_not_modify_other_rooms)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    int initial_room = player_get_room(game_engine_get_player(eng));

    game_engine_player_move_with_undo(eng, DIR_EAST);
    game_engine_undo_move(eng);

    int final_room = player_get_room(game_engine_get_player(eng));

    ck_assert_int_eq(initial_room, final_room);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_reset_restores_pushables_without_using_undo)
{
    GameEngine *eng = NULL;
    game_engine_create("../assets/starter.ini", &eng);

    game_engine_player_move_with_undo(eng, DIR_EAST);

    game_engine_reset_current_room(eng);

    ck_assert_int_eq(game_engine_undo_move(eng), INVALID_ARGUMENT);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_double_undo)
{
    GameEngine *eng = NULL;
    ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

    ck_assert_int_eq(game_engine_player_move_with_undo(eng, DIR_EAST), OK);
    ck_assert_int_eq(game_engine_undo_move(eng), OK);

    ck_assert_int_eq(game_engine_undo_move(eng), INVALID_ARGUMENT);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_game_engine_move_after_undo_replaces_history)
{
    GameEngine *eng = NULL;
    ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);

    ck_assert_int_eq(game_engine_player_move_with_undo(eng, DIR_EAST), OK);
    ck_assert_int_eq(game_engine_undo_move(eng), OK);

    ck_assert_int_eq(game_engine_player_move_with_undo(eng, DIR_EAST), OK);
	ck_assert_int_eq(game_engine_player_move_with_undo(eng, DIR_NORTH), OK);

    ck_assert_int_eq(game_engine_undo_move(eng), OK);
    ck_assert_int_eq(game_engine_undo_move(eng), INVALID_ARGUMENT);

    game_engine_destroy(eng);
}
END_TEST

START_TEST(test_move_with_undo_null_player)
{
	GameEngine *eng = NULL;
	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
 
	Player *saved = eng->player;
	eng->player = NULL;
 
	ck_assert_int_eq(game_engine_player_move_with_undo(eng, DIR_NORTH), INVALID_ARGUMENT);
 
	eng->player = saved;
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_reset_current_room_preserves_room_count)
{
	GameEngine *eng = NULL;
	int before = 0;
	int after = 0;
 
	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
 
	ck_assert_int_eq(game_engine_get_room_count(eng, &before), OK);
 
	game_engine_player_move_with_undo(eng, DIR_SOUTH);
	game_engine_reset_current_room(eng);
 
	ck_assert_int_eq(game_engine_get_room_count(eng, &after), OK);
	ck_assert_int_eq(before, after);
 
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_reset_current_room_multiple_times)
{
	GameEngine *eng = NULL;
	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
 
	ck_assert_int_eq(game_engine_reset_current_room(eng), OK);
	ck_assert_int_eq(game_engine_reset_current_room(eng), OK);
	ck_assert_int_eq(game_engine_reset_current_room(eng), OK);
 
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_reset_current_room_null_player)
{
	GameEngine *eng = NULL;
	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
 
	Player *saved = eng->player;
	eng->player = NULL;
 
	ck_assert_int_eq(game_engine_reset_current_room(eng), INVALID_ARGUMENT);
 
	eng->player = saved;
	game_engine_destroy(eng);
}
END_TEST

START_TEST(test_reset_then_move_with_undo)
{
	GameEngine *eng = NULL;
	ck_assert_int_eq(game_engine_create("../assets/starter.ini", &eng), OK);
 
	game_engine_reset_current_room(eng);
 
	Status st = game_engine_player_move_with_undo(eng, DIR_SOUTH);
	if (st == OK) {
		ck_assert_int_eq(game_engine_undo_move(eng), OK);
	}
 
	game_engine_destroy(eng);
}
END_TEST

Suite *game_engine_suite(void)
{
	// Create a new test suite with a descriptive name
	Suite *s = suite_create("GameEngineFunctions");
		
	// Create a test case to group related tests
	TCase *tc = tcase_create("GameEngine");

	// Add individual test functions to the test case
	tcase_add_test(tc, test_game_engine_create_null);
	tcase_add_test(tc, test_game_engine_create_invalid_config);
	tcase_add_test(tc, test_game_engine_create_success);
	tcase_add_test(tc, test_game_engine_create_charset_loaded);
	tcase_add_test(tc, test_game_engine_destroy_null);
	tcase_add_test(tc, test_game_engine_destroy_success);
	tcase_add_test(tc, test_game_engine_get_player_NULL);
	tcase_add_test(tc, test_game_engine_get_player_success);
	tcase_add_test(tc, test_game_engine_move_player_NULL);
	tcase_add_test(tc, test_game_engine_move_player_north_success);
	tcase_add_test(tc, test_game_engine_move_player_south_success);
	tcase_add_test(tc, test_game_engine_move_player_east_success);
	tcase_add_test(tc, test_game_engine_move_player_west_success);
	tcase_add_test(tc, test_game_engine_move_player_multiple_moves);
	tcase_add_test(tc, test_game_engine_move_player_check_position);
	tcase_add_test(tc, test_game_engine_get_room_count_NULL);
	tcase_add_test(tc, test_game_engine_get_room_count_success);
	tcase_add_test(tc, test_game_engine_get_room_dimentsions_NULL);
	tcase_add_test(tc, test_game_engine_get_room_dimentsions_success);
	tcase_add_test(tc, test_game_engine_reset_NULL);
	tcase_add_test(tc, test_game_engine_reset_success);
	tcase_add_test(tc, test_game_engine_reset_multiple_resests);
	tcase_add_test(tc, test_game_engine_render_current_room_NULL);
	tcase_add_test(tc, test_game_engine_render_current_room_success);
	tcase_add_test(tc, test_game_engine_render_current_room_newline);
	tcase_add_test(tc, test_game_engine_render_current_room_player);
	tcase_add_test(tc, test_game_engine_render_room_NULL);
	tcase_add_test(tc, test_game_engine_render_room_invalid_id);
	tcase_add_test(tc, test_game_engine_render_room_success);
	tcase_add_test(tc, test_game_engine_render_room_no_player);
	tcase_add_test(tc, test_game_engine_get_room_ids_NULL);
	tcase_add_test(tc, test_game_engine_get_room_ids_invalid_graph);
	tcase_add_test(tc, test_game_engine_get_room_ids_success);
	tcase_add_test(tc, test_game_engine_get_room_ids_has_room);
	tcase_add_test(tc, test_game_engine_free_string_null);
	tcase_add_test(tc, test_game_engine_free_string_success);
	tcase_add_test(tc, test_game_engine_player_move_on_portal_NULL);
	tcase_add_test(tc, test_game_engine_player_move_on_portal_valid_position_changes);
	tcase_add_test(tc, test_game_engine_player_move_on_portal_invalid_direction);
	tcase_add_test(tc, test_game_engine_use_portal_NULL);
	tcase_add_test(tc, test_game_engine_use_portal_not_on_portal);
	tcase_add_test(tc, test_game_engine_use_portal_room_success);
	tcase_add_test(tc, test_game_engine_player_move_on_portal_null_player);
	tcase_add_test(tc, test_game_engine_player_move_on_portal_hits_wall);
	tcase_add_test(tc, test_game_engine_use_portal_null_player);
	tcase_add_test(tc, test_game_engine_use_portal_position_valid);
	tcase_add_test(tc, test_game_engine_use_portal_stress);
	tcase_add_test(tc, test_game_engine_player_move_on_portal_invalid_room);
	tcase_add_test(tc, test_game_engine_player_move_on_portal_pushable);
	tcase_add_test(tc, test_game_engine_player_move_on_portal_internal_error);
	tcase_add_test(tc, test_game_engine_use_portal_invalid_graph);
	tcase_add_test(tc, test_game_engine_use_portal_no_position_change);
	tcase_add_test(tc, test_game_engine_move_player_invalid_direction);
	tcase_add_test(tc, test_game_engine_move_player_null_player);
	tcase_add_test(tc, test_game_engine_get_room_dimensions_after_reset);
	tcase_add_test(tc, test_game_engine_render_room_multiple_times);
	tcase_add_test(tc, test_game_engine_move_player_stress);
	tcase_add_test(tc, test_game_engine_get_room_ids_multiple_calls);
	tcase_add_test(tc, test_game_engine_free_string_double_call);
	tcase_add_test(tc, test_treasure_count_after_move);
	tcase_add_test(tc, test_room_count_after_portal);
	tcase_add_test(tc, test_portal_invalid_direction_does_not_corrupt_state);
	tcase_add_test(tc, test_move_with_undo_null_engine);
	tcase_add_test(tc, test_move_with_undo_invalid_direction);
	tcase_add_test(tc, test_move_with_undo_success);
	tcase_add_test(tc, test_undo_treasure_collection);
	tcase_add_test(tc, test_undo_pushable);
	tcase_add_test(tc, test_undo_without_move);
	tcase_add_test(tc, test_reset_current_room_null);
	tcase_add_test(tc, test_reset_clears_undo);
	tcase_add_test(tc, test_move_portal_tile_with_undo);
	tcase_add_test(tc, test_undo_after_full_reset_fails);
	tcase_add_test(tc, test_reset_current_room_restores_position);
	tcase_add_test(tc, test_undo_after_different_move_types);
	tcase_add_test(tc, test_undo_null_engine);
	tcase_add_test(tc, test_reset_current_room_invalidates_undo);
	tcase_add_test(tc, test_undo_overwrite_after_room_change);
	tcase_add_test(tc, test_undo_does_not_modify_other_rooms);
	tcase_add_test(tc, test_reset_restores_pushables_without_using_undo);
	tcase_add_test(tc, test_double_undo);
	tcase_add_test(tc, test_game_engine_move_after_undo_replaces_history);
	tcase_add_test(tc, test_move_with_undo_null_player);
	tcase_add_test(tc, test_reset_current_room_preserves_room_count);
	tcase_add_test(tc, test_reset_current_room_multiple_times);
	tcase_add_test(tc, test_reset_current_room_null_player);
	tcase_add_test(tc, test_reset_then_move_with_undo);

	// Add the test case to the suite
	suite_add_tcase(s, tc);
		
	// Return the complete suite so main() can run it
	return s;
}