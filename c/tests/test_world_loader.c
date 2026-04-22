#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "world_loader.h"
#include "graph.h"
#include "room.h"
#include "datagen.h"

START_TEST(test_world_loader_NULL)
{
	Graph *graph = NULL;
	Room *room = NULL;
	int count = 0;
	Charset ch;

	ck_assert_int_eq(loader_load_world(NULL, &graph, &room, &count, &ch), INVALID_ARGUMENT);
	ck_assert_int_eq(loader_load_world("config.ini", NULL, &room, &count, &ch), INVALID_ARGUMENT);
	ck_assert_int_eq(loader_load_world("config.ini", &graph, NULL, &count, &ch), INVALID_ARGUMENT);
	ck_assert_int_eq(loader_load_world("config.ini", &graph, &room, NULL, &ch), INVALID_ARGUMENT);
	ck_assert_int_eq(loader_load_world("config.ini", &graph, &room, &count, NULL), INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_world_loader_valid_config)
{
	Graph *graph = NULL;
	Room *room = NULL;
	int count = 0;
	Charset ch;

	ck_assert_int_eq(loader_load_world("../assets/starter.ini", &graph, &room, &count, &ch), OK);
	ck_assert_ptr_nonnull(graph);
	ck_assert_ptr_nonnull(room);
	ck_assert(count > 0);

	graph_destroy(graph);
}
END_TEST

START_TEST(test_world_loader_first_room)
{
	Graph *graph = NULL;
	Room *firstroom = NULL;
	int count = 0;
	Charset ch;

	ck_assert_int_eq(loader_load_world("../assets/starter.ini", &graph, &firstroom, &count, &ch), OK);
	ck_assert_ptr_nonnull(graph);
	ck_assert_ptr_nonnull(firstroom);
	ck_assert(room_get_width(firstroom) > 0);
	ck_assert(room_get_height(firstroom) > 0);
	ck_assert(count > 0);

	graph_destroy(graph);
}
END_TEST

START_TEST(test_world_loader_count_equal_graph_size)
{
	Graph *graph = NULL;
	Room *firstroom = NULL;
	int count = 0;
	Charset ch;

	ck_assert_int_eq(loader_load_world("../assets/starter.ini", &graph, &firstroom, &count, &ch), OK);
	ck_assert_ptr_nonnull(graph);
	ck_assert_ptr_nonnull(firstroom);
	ck_assert_int_eq(count, graph_size(graph));

	graph_destroy(graph);
}
END_TEST

START_TEST(test_world_loader_graph_edges)
{
	Graph *graph = NULL;
	Room *firstroom = NULL;
	int count = 0;
	Charset ch;

	ck_assert_int_eq(loader_load_world("../assets/starter.ini", &graph, &firstroom, &count, &ch), OK);
	ck_assert_ptr_nonnull(graph);
	ck_assert_ptr_nonnull(firstroom);
	ck_assert(graph_edge_count(graph) > 0);

	graph_destroy(graph);
}
END_TEST

START_TEST(test_world_loader_id_saved)
{
	Graph *graph = NULL;
	Room *firstroom = NULL;
	int count = 0;
	Charset ch;

	ck_assert_int_eq(loader_load_world("../assets/starter.ini", &graph, &firstroom, &count, &ch), OK);
	ck_assert_ptr_nonnull(firstroom);
	ck_assert_int_eq(firstroom->id, 0);

	graph_destroy(graph);
}
END_TEST

START_TEST(test_world_loader_multiple_calls)
{
	Graph *graph = NULL;
	Room *firstroom = NULL;
	int count = 0;
	Charset ch;

	Graph *graph2 = NULL;
	Room *firstroom2 = NULL;
	int count2 = 0;
	Charset ch2;

	ck_assert_int_eq(loader_load_world("../assets/starter.ini", &graph, &firstroom, &count, &ch), OK);
	ck_assert_ptr_nonnull(graph);
	ck_assert_int_eq(loader_load_world("../assets/starter.ini", &graph2, &firstroom2, &count2, &ch2), OK);
	ck_assert_ptr_nonnull(graph2);
	ck_assert_int_eq(count, count2);

	graph_destroy(graph);
	graph_destroy(graph2);
}
END_TEST

START_TEST(test_world_loader_char_wall)
{
	Graph *graph = NULL;
	Room *firstroom = NULL;
	int count = 0;
	Charset ch;

	ck_assert_int_eq(loader_load_world("../assets/starter.ini", &graph, &firstroom, &count, &ch), OK);
	ck_assert(ch.wall == '#');

	graph_destroy(graph);
}
END_TEST

Suite *world_loader_suite(void)
{
	// Create a new test suite with a descriptive name
	Suite *s = suite_create("WorldLoaderFunctions");
		
	// Create a test case to group related tests
	TCase *tc = tcase_create("WorldLoader");

	// Add individual test functions to the test case
	tcase_add_test(tc, test_world_loader_NULL);
	tcase_add_test(tc, test_world_loader_valid_config);
	tcase_add_test(tc, test_world_loader_first_room);
	tcase_add_test(tc, test_world_loader_count_equal_graph_size);
	tcase_add_test(tc, test_world_loader_graph_edges);
	tcase_add_test(tc, test_world_loader_id_saved);
	tcase_add_test(tc, test_world_loader_multiple_calls);
	tcase_add_test(tc, test_world_loader_char_wall);

	// Add the test case to the suite
	suite_add_tcase(s, tc);
		
	// Return the complete suite so main() can run it
	return s;
}