#include <check.h>
#include <stdlib.h>
#include "player.h"

/* ============================================================
 * Test 1: Regular Test
 * Tests that player_create works perfectly
 * ============================================================ */

START_TEST(test_create_success)
{
	Player *p = NULL;
	Status s = player_create(1, 2, 3, &p);

	ck_assert_int_eq(s, OK);
	player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 2: NULL
 * Tests that player_create checks for NULL player
 * ============================================================ */

START_TEST(test_create_NULL)
{
	Status s = player_create(1, 2, 3, NULL);
	ck_assert_int_eq(s, INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 3: NULL
 * Tests that player_destroy checks for NULL
 * ============================================================ */

START_TEST(test_destroy_NULL)
{
	player_destroy(NULL);
	ck_assert(true);
}
END_TEST

/* ============================================================
 * Test 4: Player destroy success
 * Tests that player_destroy checks for success
 * ============================================================ */

START_TEST(test_destroy_success)
{
	Player *p = NULL;
	Status s = player_create(1, 2, 3, &p);

	ck_assert_int_eq(s, OK);

	player_destroy(p);
	ck_assert(true);
}
END_TEST

/* ============================================================
 * Test 5: Get room success
 * Tests that player_get_room checks for success
 * ============================================================ */

START_TEST(test_get_room_success)
{
	Player *p = NULL;
	Status s = player_create(1, 2, 3, &p);
	ck_assert_int_eq(s, OK);

	ck_assert_int_eq(player_get_room(p), 1);
	player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 6: Get room NULL
 * Tests that player_get_room checks for success
 * ============================================================ */

START_TEST(test_get_room_NULL)
{
	ck_assert_int_eq(player_get_room(NULL), -1);
}
END_TEST

/* ============================================================
 * Test 7: Get position success
 * Tests that player_get_position checks for success
 * ============================================================ */

START_TEST(test_get_position_success)
{
	Player *p = NULL;
	int x;
	int y;
	Status s = player_create(1, 2, 3, &p);
	ck_assert_int_eq(s, OK);

	ck_assert_int_eq(player_get_position(p, &x, &y), OK);
	ck_assert_int_eq(x, 2);
	ck_assert_int_eq(y, 3);
	player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 8: Get position NULL
 * Tests that player_get_position checks for NULL
 * ============================================================ */

START_TEST(test_get_position_NULL)
{
	int x;
	int y;
	Player *p = NULL;
	player_create(1, 2, 3, &p);
	ck_assert_int_eq(player_get_position(NULL, &x, &y), INVALID_ARGUMENT);
	ck_assert_int_eq(player_get_position(p, NULL, &y), INVALID_ARGUMENT);
	ck_assert_int_eq(player_get_position(p, &x, NULL), INVALID_ARGUMENT);
	ck_assert_int_eq(player_get_position(NULL, NULL, NULL), INVALID_ARGUMENT);
	player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 9: Set position NULL
 * Tests that player_set_position checks for NULL
 * ============================================================ */

START_TEST(test_set_position_NULL)
{
	ck_assert_int_eq(player_set_position(NULL, 5, 5), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 10: Set position success
 * Tests that player_set_position checks for success
 * ============================================================ */

START_TEST(test_set_position_success)
{
	Player *p = NULL;
	int x;
	int y;
	player_create(1, 2, 3, &p);
	ck_assert_int_eq(player_set_position(p, 5, 10), OK);
	ck_assert_int_eq(player_get_position(p, &x, &y), OK);
	ck_assert_int_eq(x, 5);
	ck_assert_int_eq(y, 10);
	player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 11: move_to_room success
 * Tests that player_move_to_room checks for success
 * ============================================================ */

START_TEST(test_move_to_room_success)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);
	ck_assert_int_eq(player_move_to_room(p, 5), OK);
	ck_assert_int_eq(player_get_room(p), 5);
	player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 12: move_to_room NULL
 * Tests that player_move_to_room checks for NULL
 * ============================================================ */

START_TEST(test_move_to_room_NULL)
{
	ck_assert_int_eq(player_move_to_room(NULL, 1), INVALID_ARGUMENT);
}
END_TEST

/* ============================================================
 * Test 13: reset_to_start success
 * Tests that player_reset_to_start checks for success
 * ============================================================ */

START_TEST(test_reset_to_start_success)
{
	Player *p = NULL;
	int x;
	int y;
	player_create(5, 10, 15, &p);
	ck_assert_int_eq(player_reset_to_start(p, 1, 2, 3), OK);
	ck_assert_int_eq(player_get_room(p), 1);
	ck_assert_int_eq(player_get_position(p, &x, &y), OK);
	ck_assert_int_eq(x, 2);
	ck_assert_int_eq(y, 3);
	player_destroy(p);
}
END_TEST

/* ============================================================
 * Test 14: reset_to_start NULL
 * Tests that player_reset_to_start checks for NULL
 * ============================================================ */

START_TEST(test_reset_to_start_NULL)
{
	ck_assert_int_eq(player_reset_to_start(NULL, 1, 2, 3), INVALID_ARGUMENT);
}
END_TEST

START_TEST(test_try_collect_null)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);
	Treasure *t = malloc(sizeof(Treasure));
	t->id = 1;
	t->x = 2;
	t->y = 2;
	t->name = NULL;
	t->collected = false;

	ck_assert_int_eq(player_try_collect(NULL, t), NULL_POINTER);
	ck_assert_int_eq(player_try_collect(p, NULL), NULL_POINTER);
	player_destroy(p);
	free(t);
}
END_TEST

START_TEST(test_try_collect_success)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);
	Treasure *t = malloc(sizeof(Treasure));
	t->id = 1;
	t->x = 1;
	t->y = 1;
	t->name = NULL;
	t->collected = false;

	ck_assert_int_eq(player_try_collect(p, t), OK);
	ck_assert_int_eq(p->collected_count, 1);
	ck_assert(t->collected == true);
	player_destroy(p);
	free(t);
}
END_TEST

START_TEST(test_try_collect_multiple)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);
	Treasure *t = malloc(sizeof(Treasure));
	t->id = 1;
	t->x = 1;
	t->y = 1;
	t->name = NULL;
	t->collected = false;
	Treasure *t2 = malloc(sizeof(Treasure));
	t2->id = 2;
	t2->x = 1;
	t2->y = 1;
	t2->name = NULL;
	t2->collected = false;
	Treasure *t3 = malloc(sizeof(Treasure));
	t3->id = 3;
	t3->x = 1;
	t3->y = 1;
	t2->name = NULL;
	t3->collected = false;

	ck_assert_int_eq(player_try_collect(p, t), OK);
	ck_assert_int_eq(player_try_collect(p, t2), OK);
	ck_assert_int_eq(player_try_collect(p, t3), OK);
	ck_assert_int_eq(p->collected_count, 3);
	player_destroy(p);
	free(t);
	free(t2);
	free(t3);
}
END_TEST

START_TEST(test_try_collect_collected)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);
	Treasure *t = malloc(sizeof(Treasure));
	t->id = 1;
	t->x = 1;
	t->y = 1;
	t->name = NULL;
	t->collected = true;

	ck_assert_int_eq(player_try_collect(p, t), INVALID_ARGUMENT);
	player_destroy(p);
	free(t);
}
END_TEST

START_TEST(test_has_collected_treasure_false)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);

	ck_assert(player_has_collected_treasure(p, 5) == false);
	player_destroy(p);
}
END_TEST

START_TEST(test_has_collected_treasure_null)
{
	ck_assert(player_has_collected_treasure(NULL, 1) == false);
}
END_TEST

START_TEST(test_has_collected_treasure_negative_id)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);

	ck_assert(player_has_collected_treasure(p, -1) == false);
	player_destroy(p);
}
END_TEST

START_TEST(test_has_collected_treasure_success)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);
	Treasure *t = malloc(sizeof(Treasure));
	t->id = 2;
	t->x = 1;
	t->y = 1;
	t->name = NULL;
	t->collected = false;

	ck_assert_int_eq(player_try_collect(p, t), OK);
	ck_assert(player_has_collected_treasure(p, 2) == true);
	player_destroy(p);
	free(t);
}
END_TEST

START_TEST(test_get_collected_count_zero)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);

	ck_assert_int_eq(player_get_collected_count(p), 0);
	player_destroy(p);
}
END_TEST

START_TEST(test_get_collected_count_null)
{
	ck_assert_int_eq(player_get_collected_count(NULL), 0);
}
END_TEST

START_TEST(test_get_collected_count_success)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);
	Treasure *t = malloc(sizeof(Treasure));
	t->id = 1;
	t->x = 1;
	t->y = 1;
	t->name = NULL;
	t->collected = false;
	Treasure *t2 = malloc(sizeof(Treasure));
	t2->id = 2;
	t2->x = 1;
	t2->y = 1;
	t2->name = NULL;
	t2->collected = false;

	ck_assert_int_eq(player_try_collect(p, t), OK);
	ck_assert_int_eq(player_get_collected_count(p), 1);
	ck_assert_int_eq(player_try_collect(p, t2), OK);
	ck_assert_int_eq(player_get_collected_count(p), 2);
	player_destroy(p);
	free(t);
	free(t2);
}
END_TEST

START_TEST(test_get_collected_treasures_null)
{
	int count = 0;
	Player *p = NULL;
	player_create(1, 2, 3, &p);
	ck_assert_ptr_null(player_get_collected_treasures(NULL, &count));
	ck_assert_ptr_null(player_get_collected_treasures(p, NULL));
	player_destroy(p);
}
END_TEST

START_TEST(test_get_collected_treasures_empty)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);
	int count = -1;

	const Treasure * const *arr = player_get_collected_treasures(p, &count);
	ck_assert_ptr_null(arr);
	ck_assert_int_eq(count, 0);
	player_destroy(p);
}
END_TEST

START_TEST(test_get_collected_treasures_success)
{
	Player *p = NULL;
	player_create(1, 2, 3, &p);
	Treasure *t = malloc(sizeof(Treasure));
	t->id = 1;
	t->x = 1;
	t->y = 1;
	t->name = NULL;
	t->collected = false;
	Treasure *t2 = malloc(sizeof(Treasure));
	t2->id = 2;
	t2->x = 1;
	t2->y = 1;
	t2->name = NULL;
	t2->collected = false;
	int count = 0;

	ck_assert_int_eq(player_try_collect(p, t), OK);
	ck_assert_int_eq(player_try_collect(p, t2), OK);

	const Treasure * const *arr = player_get_collected_treasures(p, &count);
	ck_assert_int_eq(count, 2);
	ck_assert_ptr_nonnull(arr);
	ck_assert_int_eq(arr[0]->id, 1);
	ck_assert_int_eq(arr[1]->id, 2);
	player_destroy(p);
	free(t);
	free(t2);
}
END_TEST

START_TEST(test_reset_clears_collected)
{
    Player *p = NULL;
    player_create(1, 1, 1, &p);

    Treasure *t = malloc(sizeof(Treasure));
    t->id = 1;
    t->x = 1;
    t->y = 1;
    t->name = NULL;
    t->collected = false;

    player_try_collect(p, t);
    ck_assert_int_eq(player_get_collected_count(p), 1);

    player_reset_to_start(p, 2, 3, 4);

    ck_assert_int_eq(player_get_collected_count(p), 0);

    player_destroy(p);
    free(t);
}
END_TEST

START_TEST(test_move_room_does_not_change_position)
{
    Player *p = NULL;
    int x, y;

    player_create(1, 5, 9, &p);

    player_move_to_room(p, 5);

    player_get_position(p, &x, &y);

    ck_assert_int_eq(x, 5);
    ck_assert_int_eq(y, 9);

    player_destroy(p);
}
END_TEST

START_TEST(test_reset_then_get_position)
{
    Player *p = NULL;
    int x, y;

    player_create(5, 10, 10, &p);
    player_reset_to_start(p, 2, 4, 8);

    ck_assert_int_eq(player_get_position(p, &x, &y), OK);
    ck_assert_int_eq(x, 4);
    ck_assert_int_eq(y, 8);

    player_destroy(p);
}
END_TEST

Suite *player_suite(void)
{
	// Create a new test suite with a descriptive name
	Suite *s = suite_create("PlayerFunctions");
		
	// Create a test case to group related tests
	TCase *tc = tcase_create("Player");

	// Add individual test functions to the test case
	tcase_add_test(tc, test_create_success);
	tcase_add_test(tc, test_create_NULL);
	tcase_add_test(tc, test_destroy_NULL);
	tcase_add_test(tc, test_destroy_success);
	tcase_add_test(tc, test_get_room_success);
	tcase_add_test(tc, test_get_room_NULL);
	tcase_add_test(tc, test_get_position_success);
	tcase_add_test(tc, test_get_position_NULL);
	tcase_add_test(tc, test_set_position_NULL);
	tcase_add_test(tc, test_set_position_success);
	tcase_add_test(tc, test_move_to_room_success);
	tcase_add_test(tc, test_move_to_room_NULL);
	tcase_add_test(tc, test_reset_to_start_success);
	tcase_add_test(tc, test_reset_to_start_NULL);
	tcase_add_test(tc, test_try_collect_null);
	tcase_add_test(tc, test_try_collect_success);
	tcase_add_test(tc, test_try_collect_multiple);
	tcase_add_test(tc, test_try_collect_collected);
	tcase_add_test(tc, test_has_collected_treasure_false);
	tcase_add_test(tc, test_has_collected_treasure_null);
	tcase_add_test(tc, test_has_collected_treasure_negative_id);
	tcase_add_test(tc, test_has_collected_treasure_success);
	tcase_add_test(tc, test_get_collected_count_zero);
	tcase_add_test(tc, test_get_collected_count_null);
	tcase_add_test(tc, test_get_collected_count_success);
	tcase_add_test(tc, test_get_collected_treasures_null);
	tcase_add_test(tc, test_get_collected_treasures_empty);
	tcase_add_test(tc, test_get_collected_treasures_success);
	tcase_add_test(tc, test_reset_clears_collected);
	tcase_add_test(tc, test_move_room_does_not_change_position);
	tcase_add_test(tc, test_reset_then_get_position);

	// Add the test case to the suite
	suite_add_tcase(s, tc);
		
	// Return the complete suite so main() can run it
	return s;
}
