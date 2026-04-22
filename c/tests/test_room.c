#include <check.h>
#include <stdlib.h>
#include "room.h"

START_TEST(test_room_create_success)
{
	Room *r = NULL;
	r = room_create(1, "Room1", 5, 10);

	ck_assert_ptr_nonnull(r);
	ck_assert_int_eq(r->id, 1);
	ck_assert_str_eq(r->name, "Room1");
	ck_assert_int_eq(r->width, 5);
	ck_assert_int_eq(r->height, 10);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_get_width_success)
{
	Room *r = NULL;
	r = room_create(1, "Room1", 5, 10);

	ck_assert_int_eq(room_get_width(r), 5);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_get_width_NULL)
{
	ck_assert_int_eq(room_get_width(NULL), 0);
}
END_TEST

START_TEST(test_room_get_height_success)
{
	Room *r = NULL;
	r = room_create(1, "Room1", 5, 10);

	ck_assert_int_eq(room_get_height(r), 10);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_get_height_NULL)
{
	ck_assert_int_eq(room_get_height(NULL), 0);
}
END_TEST

START_TEST(test_room_set_floor_grid_success)
{
	Room *r = NULL;
	r = room_create(1, "Room1", 5, 10);
	bool *grid = malloc(sizeof(bool) * 2);
	grid[0] = true;
	grid[1] = false;

	ck_assert_int_eq(room_set_floor_grid(r, grid), OK);
	ck_assert_ptr_eq(r->floor_grid, grid);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_set_floor_grid_NULL)
{
	bool *grid = malloc(sizeof(bool) * 2);
	ck_assert_int_eq(room_set_floor_grid(NULL, grid), INVALID_ARGUMENT);
	free(grid);
}
END_TEST

START_TEST(test_room_set_portals_NULL)
{
	Room *r = NULL;
	r = room_create(1, "Room1", 5, 10);

	ck_assert_int_eq(room_set_portals(r, NULL, 5), INVALID_ARGUMENT);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_set_portals_success)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 10);
	Portal *p = malloc(sizeof(Portal));
	p[0].id = 1;
	p[0].x = 2;
	p[0].y = 3;
	p[0].target_room_id = 4;

	ck_assert_int_eq(room_set_portals(r, p, 1), OK);
	ck_assert_ptr_eq(r->portals, p);
	ck_assert_int_eq(r->portal_count, 1);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_set_treasures_success)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 10);
	Treasure *t = malloc(sizeof(Treasure));
	t[0].id = 1;
	t[0].x = 2;
	t[0].y = 3;

	ck_assert_int_eq(room_set_treasures(r, t, 1), OK);
	ck_assert_ptr_eq(r->treasures, t);
	ck_assert_int_eq(r->treasure_count, 1);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_set_treasures_NULL)
{
	Room *r = NULL;
	r = room_create(1, "Room1", 5, 10);

	ck_assert_int_eq(room_set_treasures(r, NULL, 5), INVALID_ARGUMENT);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_place_treasure_NULL)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 10);
	Treasure *t = malloc(sizeof(Treasure));
	t[0].id = 1;
	t[0].x = 2;
	t[0].y = 3;

	ck_assert_int_eq(room_place_treasure(r, NULL), INVALID_ARGUMENT);
	ck_assert_int_eq(room_place_treasure(NULL, t), INVALID_ARGUMENT);
	free(t);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_place_and_get_treasure_success)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 10);
	Treasure *t = malloc(sizeof(Treasure));
	t[0].id = 1;
	t[0].x = 2;
	t[0].y = 3;

	ck_assert_int_eq(room_place_treasure(r, t), OK);
	ck_assert_int_eq(room_get_treasure_at(r, 2, 3), 1);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_get_treasure_none)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 10);

	ck_assert_int_eq(room_get_treasure_at(r, 2, 3), -1);
	ck_assert_int_eq(room_get_treasure_at(NULL, 2, 3), -1);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_get_portal_destination_none)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 10);

	ck_assert_int_eq(room_get_portal_destination(r, 2, 3), -1);
	ck_assert_int_eq(room_get_portal_destination(NULL, 2, 3), -1);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_get_portal_destination_success)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 10);
	Portal *p = malloc(sizeof(Portal));
	p[0].id = 10;
	p[0].x = 1;
	p[0].y = 2;
	p[0].target_room_id = 20;

	room_set_portals(r, p, 1);

	ck_assert_int_eq(room_get_portal_destination(r, 1, 2), 20);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_is_walkable_success)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 5);

	ck_assert(room_is_walkable(r, 3, 3));
	ck_assert(room_is_walkable(r, 1, 2));
	room_destroy(r);
}
END_TEST

START_TEST(test_room_is_walkable_NULL)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 5);

	ck_assert(!room_is_walkable(r, 0, 0));
	ck_assert(!room_is_walkable(r, -1, -1));
	room_destroy(r);
}
END_TEST

START_TEST(test_room_classify_tile_floor)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 10);

	ck_assert_int_eq(room_classify_tile(r, 3, 7, NULL), ROOM_TILE_FLOOR);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_classify_tile_portal)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 10);
	Portal *p = malloc(sizeof(Portal));
	p[0].id = 10;
	p[0].x = 1;
	p[0].y = 2;
	p[0].target_room_id = 20;
	int out_id = 0;

	room_set_portals(r, p, 1);

	ck_assert_int_eq(room_classify_tile(r, 1, 2, &out_id), ROOM_TILE_PORTAL);
	ck_assert_int_eq(out_id, 20);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_classify_tile_treasure)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 10);
	Treasure *t = malloc(sizeof(Treasure));
	t[0].id = 1;
	t[0].x = 2;
	t[0].y = 3;
	int out_id = 0;

	room_place_treasure(r, t);

	ck_assert_int_eq(room_classify_tile(r, 2, 3, &out_id), ROOM_TILE_TREASURE);
	ck_assert_int_eq(out_id, 1);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_classify_tile_invalid)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 10);

	ck_assert_int_eq(room_classify_tile(r, -1, -1, NULL), ROOM_TILE_INVALID);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_get_start_position_success)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 5);
	int x = -1;
	int y = -1;

	ck_assert_int_eq(room_get_start_position(r, &x, &y), OK);
	ck_assert(x >= 0);
	ck_assert(y >= 0);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_get_start_position_NULL)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 3, 3);
	int x = -1;
	int y = -1;
	bool *grid = malloc(sizeof(bool) * 9);
	for (int i = 0; i < 9; i++) {
		grid[i] = false;
	}

	room_set_floor_grid(r, grid);

	ck_assert_int_eq(room_get_start_position(r, NULL, &y), INVALID_ARGUMENT);
	ck_assert_int_eq(room_get_start_position(r, &x, NULL), INVALID_ARGUMENT);
	ck_assert_int_eq(room_get_start_position(NULL, &x, &y), INVALID_ARGUMENT);
	ck_assert_int_eq(room_get_start_position(r, &x, &y), ROOM_NOT_FOUND);

	room_destroy(r);
}
END_TEST

START_TEST(test_room_destroy_NULL)
{
	room_destroy(NULL);
}
END_TEST

START_TEST(test_room_render_invalid)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 5);
	char b[4];
	Charset *ch = NULL;

	ck_assert_int_eq(room_render(NULL, ch, b, 1, 2), INVALID_ARGUMENT);
	ck_assert_int_eq(room_render(r, NULL, b, 1, 2), INVALID_ARGUMENT);
	ck_assert_int_eq(room_render(r, ch, NULL, 1, 2), INVALID_ARGUMENT);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_render_success)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 2, 2);
	char b[4];
	Charset ch = {'#', '.', '@', 'T', 'P', '*', '0', '1'};
	bool *grid = malloc(sizeof(bool) * 4);
	for(int i = 0; i < 4; i++){
		grid[i] = true;
	}

	room_set_floor_grid(r, grid);

	ck_assert_int_eq(room_render(r, &ch, b, 2, 2), OK);
	
	for(int i = 0; i < 4; i++){
		ck_assert(b[i] == '.');
	}
	room_destroy(r);
}
END_TEST

START_TEST(test_room_render_walls)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 2, 2);
	char b[4];
	Charset ch = {'#', '.', '@', 'T', 'P', '*', '0', '1'};

	ck_assert_int_eq(room_render(r, &ch, b, 2, 2), OK);
	
	ck_assert(b[0] == '#');
	ck_assert(b[1] == '#');
	ck_assert(b[2] == '#');
	ck_assert(b[3] == '#');
	room_destroy(r);
}
END_TEST

START_TEST(test_room_render_floor_layer)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 5, 5);
	char b[6];
	Charset ch = {'#', '.', '@', '*', 'T', 'P', '0', '1'};
	bool *grid = malloc(sizeof(bool) * 6);
	grid[0] = false;
	grid[1] = true;
	grid[2] = true;
	grid[3] = false;
	grid[4] = true;
	grid[5] = true;

	room_set_floor_grid(r, grid);

	ck_assert_int_eq(room_render(r, &ch, b, 5, 5), OK);
	
	ck_assert(b[0] == '#');
	ck_assert(b[1] == '.');
	ck_assert(b[2] == '.');
	ck_assert(b[3] == '#');
	ck_assert(b[4] == '.');
	ck_assert(b[5] == '.');
	room_destroy(r);
}
END_TEST

START_TEST(test_room_render_portal_overrides)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 2, 2);
	char b[4];
	Charset ch = {'#', '.', '@', '*', 'T', 'P', '0', '1'};
	Portal *p = malloc(sizeof(Portal));
	p[0].id = 10;
	p[0].x = 1;
	p[0].y = 1;
	p[0].target_room_id = 20;

	room_set_portals(r, p, 1);

	ck_assert_int_eq(room_render(r, &ch, b, 2, 2), OK);
	
	ck_assert(b[3] == 'P');
	room_destroy(r);
}
END_TEST

START_TEST(test_room_render_treasure_overrides)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 2, 2);
	char b[6];
	Charset ch = {'#', '.', '@', '*', 'T', 'P', '0', '1'};
	Treasure *t = malloc(sizeof(Treasure));
	t[0].id = 10;
	t[0].x = 1;
	t[0].y = 1;
	t[0].name = NULL;

	room_place_treasure(r, t);

	ck_assert_int_eq(room_render(r, &ch, b, 2, 2), OK);
	
	ck_assert(b[3] == 'T');
	room_destroy(r);
}
END_TEST

START_TEST(test_room_render_smallest_room)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 1, 1);
	char b[1];
	Charset ch = {'#', '.', '@', '*', 'T', 'P', '0', '1'};

	ck_assert_int_eq(room_render(r, &ch, b, 1, 1), OK);
	
	ck_assert(b[0] == '#');
	room_destroy(r);
}
END_TEST

START_TEST(test_room_render_portal_overrides_treasure)
{
	Room *r = NULL;
	r = room_create(1, "Room 1", 2, 2);
	char b[4];
	Charset ch = {'#', '.', '@', '*', 'T', 'P', '0', '1'};
	Portal *p = malloc(sizeof(Portal));
	p[0].id = 10;
	p[0].x = 1;
	p[0].y = 1;
	p[0].target_room_id = 20;
	Treasure *t = malloc(sizeof(Treasure));
	t[0].id = 10;
	t[0].x = 1;
	t[0].y = 1;
	t[0].name = NULL;

	room_place_treasure(r, t);
	room_set_portals(r, p, 1);

	ck_assert_int_eq(room_render(r, &ch, b, 2, 2), OK);
	
	ck_assert(b[3] == 'P');
	room_destroy(r);
}
END_TEST

START_TEST(test_room_get_id_null)
{
	ck_assert_int_eq(room_get_id(NULL), -1);
}
END_TEST

START_TEST(test_room_get_id_success)
{
	Room *r = NULL;
	r = room_create(10, "Room 1", 2, 2);
	ck_assert_int_eq(room_get_id(r), 10);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_pick_up_treasure_null)
{
	Room *r = NULL;
	r = room_create(5, "Room 1", 2, 2);
	Treasure *t = NULL;
	ck_assert_int_eq(room_pick_up_treasure(r, 10, NULL), INVALID_ARGUMENT);
	ck_assert_int_eq(room_pick_up_treasure(NULL, 10, &t), INVALID_ARGUMENT);
	ck_assert_int_eq(room_pick_up_treasure(r, -10, &t), INVALID_ARGUMENT);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_pick_up_treasure_room_not_found)
{
	Room *r = NULL;
	r = room_create(5, "Room 1", 2, 2);
	Treasure *t = NULL;
	ck_assert_int_eq(room_pick_up_treasure(r, 10, &t), ROOM_NOT_FOUND);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_pick_up_treasure_room_success)
{
	Room *r = NULL;
	r = room_create(5, "Room 1", 2, 2);
	Treasure *t = malloc(sizeof(Treasure));
	t->id = 3;
	t->x = 1;
	t->y = 1;
	t->name = NULL;
	t->collected = false;
	Treasure *t2 = NULL;
	room_place_treasure(r, t);
	ck_assert_int_eq(room_pick_up_treasure(r, 3, &t2), OK);
	ck_assert_ptr_nonnull(t2);
	ck_assert_int_eq(t2->id, 3);
	ck_assert(t2->collected == true);
	free(t);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_pick_up_treasure_room_collected)
{
	Room *r = NULL;
	r = room_create(5, "Room 1", 2, 2);
	Treasure *t = malloc(sizeof(Treasure));
	t->id = 3;
	t->x = 1;
	t->y = 1;
	t->name = NULL;
	t->collected = false;
	Treasure *t2 = NULL;
	room_place_treasure(r, t);
	ck_assert_int_eq(room_pick_up_treasure(r, 3, &t2), OK);
	ck_assert_int_eq(room_pick_up_treasure(r, 3, &t2), INVALID_ARGUMENT);
	free(t);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_has_pushable_at_null)
{
	int *out = NULL;
	ck_assert(!room_has_pushable_at(NULL, 2, 2, out));
}
END_TEST

START_TEST(test_room_has_pushable_at_success)
{
	int out = -1;
	Room *r = NULL;
	r = room_create(5, "Room 1", 2, 2);
	Pushable *p = malloc(sizeof(Pushable) * 3);
	p[0].x = 2;
	p[0].y = 2;
	p[1].x = 3;
	p[1].y = 3;
	p[2].x = 4;
	p[2].y = 4;
	r->pushables = p;
	r->pushable_count = 3;
	ck_assert(room_has_pushable_at(r, 3, 3, &out));
	ck_assert_int_eq(out, 1);
	ck_assert(room_has_pushable_at(r, 2, 2, &out));
	ck_assert_int_eq(out, 0);
	ck_assert(!room_has_pushable_at(r, 1, 1, NULL));
	room_destroy(r);
}
END_TEST

START_TEST(test_room_has_pushable_at_none)
{
	int *out = NULL;
	Room *r = NULL;
	r = room_create(5, "Room 1", 2, 2);
	ck_assert(!room_has_pushable_at(r, 3, 3, out));
	room_destroy(r);
}
END_TEST

START_TEST(test_room_try_push_null)
{
	Room *r = NULL;
	r = room_create(5, "Room 1", 2, 2);
	ck_assert_int_eq(room_try_push(NULL, 1, DIR_EAST), INVALID_ARGUMENT);
	ck_assert_int_eq(room_try_push(r, 1, DIR_EAST), INVALID_ARGUMENT);
	ck_assert_int_eq(room_try_push(r, -1, DIR_EAST), INVALID_ARGUMENT);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_try_push_out_of_bounds)
{
	Room *r = NULL;
	r = room_create(5, "Room 1", 2, 2);
	Pushable *p = malloc(sizeof(Pushable) * 3);
	p[0].x = 2;
	p[0].y = 2;
	r->pushables = p;
	r->pushable_count = 1;
	ck_assert_int_eq(room_try_push(r, 1, DIR_EAST), INVALID_ARGUMENT);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_try_push_success)
{
	Room *r = NULL;
	r = room_create(5, "Room 1", 5, 5);
	Pushable *p = malloc(sizeof(Pushable));
	p[0].x = 2;
	p[0].y = 2;
	r->pushables = p;
	r->pushable_count = 1;
	ck_assert_int_eq(room_try_push(r, 0, DIR_NORTH), OK);
	ck_assert_int_eq(r->pushables[0].x, 2);
	ck_assert_int_eq(r->pushables[0].y, 1);
	ck_assert_int_eq(room_try_push(r, 0, DIR_SOUTH), OK);
	ck_assert_int_eq(r->pushables[0].x, 2);
	ck_assert_int_eq(r->pushables[0].y, 2);
	ck_assert_int_eq(room_try_push(r, 0, DIR_EAST), OK);
	ck_assert_int_eq(r->pushables[0].x, 3);
	ck_assert_int_eq(r->pushables[0].y, 2);
	ck_assert_int_eq(room_try_push(r, 0, DIR_WEST), OK);
	ck_assert_int_eq(r->pushables[0].x, 2);
	ck_assert_int_eq(r->pushables[0].y, 2);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_try_push_into_portal)
{
	Room *r = NULL;
	r = room_create(5, "Room 1", 5, 5);
	Portal *portal = malloc(sizeof(Portal));
	portal[0].id = 1;
	portal[0].x = 2;
	portal[0].y = 1;
	portal[0].target_room_id = 2;
	portal[0].name = NULL;
	room_set_portals(r, portal, 1);

	Pushable *p = malloc(sizeof(Pushable));
	p[0].x = 2;
	p[0].y = 2;
	r->pushables = p;
	r->pushable_count = 1;
	ck_assert_int_eq(room_try_push(r, 0, DIR_NORTH), ROOM_IMPASSABLE);
	ck_assert_int_eq(r->pushables[0].x, 2);
	ck_assert_int_eq(r->pushables[0].y, 2);
	room_destroy(r);
}
END_TEST

START_TEST(test_room_try_push_into_wall)
{
	Room *r = NULL;
	r = room_create(5, "Room 1", 5, 5);
	Pushable *p = malloc(sizeof(Pushable));
	p[0].x = 1;
	p[0].y = 2;
	r->pushables = p;
	r->pushable_count = 1;
	ck_assert_int_eq(room_try_push(r, 0, DIR_WEST), ROOM_IMPASSABLE);
	ck_assert_int_eq(r->pushables[0].x, 1);
	ck_assert_int_eq(r->pushables[0].y, 2);
	room_destroy(r);
}
END_TEST

Suite *room_suite(void)
{
	// Create a new test suite with a descriptive name
	Suite *s = suite_create("RoomFunctions");
		
	// Create a test case to group related tests
	TCase *tc = tcase_create("Room");

	// Add individual test functions to the test case
	tcase_add_test(tc, test_room_create_success);
	tcase_add_test(tc, test_room_get_width_success);
	tcase_add_test(tc, test_room_get_width_NULL);
	tcase_add_test(tc, test_room_get_height_success);
	tcase_add_test(tc, test_room_get_height_NULL);
	tcase_add_test(tc, test_room_set_floor_grid_success);
	tcase_add_test(tc, test_room_set_floor_grid_NULL);
	tcase_add_test(tc, test_room_set_portals_NULL);
	tcase_add_test(tc, test_room_set_portals_success);
	tcase_add_test(tc, test_room_set_treasures_success);
	tcase_add_test(tc, test_room_set_treasures_NULL);
	tcase_add_test(tc, test_room_place_treasure_NULL);
	tcase_add_test(tc, test_room_place_and_get_treasure_success);
	tcase_add_test(tc, test_room_get_treasure_none);
	tcase_add_test(tc, test_room_get_portal_destination_none);
	tcase_add_test(tc, test_room_get_portal_destination_success);
	tcase_add_test(tc, test_room_is_walkable_success);
	tcase_add_test(tc, test_room_is_walkable_NULL);
	tcase_add_test(tc, test_room_classify_tile_floor);
	tcase_add_test(tc, test_room_classify_tile_portal);
	tcase_add_test(tc, test_room_classify_tile_treasure);
	tcase_add_test(tc, test_room_classify_tile_invalid);
	tcase_add_test(tc, test_room_get_start_position_success);
	tcase_add_test(tc, test_room_get_start_position_NULL);
	tcase_add_test(tc, test_room_destroy_NULL);
	tcase_add_test(tc, test_room_render_invalid);
	tcase_add_test(tc, test_room_render_success);
	tcase_add_test(tc, test_room_render_walls);
	tcase_add_test(tc, test_room_render_floor_layer);
	tcase_add_test(tc, test_room_render_portal_overrides);
	tcase_add_test(tc, test_room_render_treasure_overrides);
	tcase_add_test(tc, test_room_render_smallest_room);
	tcase_add_test(tc, test_room_render_portal_overrides_treasure);
	tcase_add_test(tc, test_room_get_id_null);
	tcase_add_test(tc, test_room_get_id_success);
	tcase_add_test(tc, test_room_pick_up_treasure_null);
	tcase_add_test(tc, test_room_pick_up_treasure_room_not_found);
	tcase_add_test(tc, test_room_pick_up_treasure_room_success);
	tcase_add_test(tc, test_room_pick_up_treasure_room_collected);
	tcase_add_test(tc, test_room_has_pushable_at_null);
	tcase_add_test(tc, test_room_has_pushable_at_success);
	tcase_add_test(tc, test_room_has_pushable_at_none);
	tcase_add_test(tc, test_room_try_push_null);
	tcase_add_test(tc, test_room_try_push_out_of_bounds);
	tcase_add_test(tc, test_room_try_push_success);
	tcase_add_test(tc, test_room_try_push_into_portal);
	tcase_add_test(tc, test_room_try_push_into_wall);

	// Add the test case to the suite
	suite_add_tcase(s, tc);
		
	// Return the complete suite so main() can run it
	return s;
}