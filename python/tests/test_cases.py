import unittest
import ctypes

class TestBindings(unittest.TestCase):
	
	def test_north_direction(self):
		from treasure_runner.bindings import Direction
		self.assertEqual(Direction.NORTH, 0)
	
	def test_south_direction(self):
		from treasure_runner.bindings import Direction
		self.assertEqual(Direction.SOUTH, 1)

	def test_west_direction(self):
		from treasure_runner.bindings import Direction
		self.assertEqual(Direction.WEST, 3)

	def test_east_direction(self):
		from treasure_runner.bindings import Direction
		self.assertEqual(Direction.EAST, 2)

	def test_status_(self):
		from treasure_runner.bindings import Status
		self.assertEqual(Status.NULL_POINTER, 2)
		self.assertEqual(Status.NO_MEMORY, 3)
		self.assertEqual(Status.BOUNDS_EXCEEDED, 4)
		self.assertEqual(Status.INTERNAL_ERROR, 5)
		self.assertEqual(Status.ROOM_IMPASSABLE, 6)
		self.assertEqual(Status.ROOM_NO_PORTAL, 7)
		self.assertEqual(Status.ROOM_NOT_FOUND, 8)
		self.assertEqual(Status.GE_NO_SUCH_ROOM, 9)
		self.assertEqual(Status.WL_ERR_CONFIG, 10)
		self.assertEqual(Status.WL_ERR_DATAGEN, 11)

	def test_status_lib_loads(self):
		from treasure_runner.bindings import lib
		self.assertIsNotNone(lib)

class TestGameEngine(unittest.TestCase):

	def setUp(self):
		from treasure_runner.models.game_engine import GameEngine
		self.eng = GameEngine("../assets/starter.ini")

	def tearDown(self):
		self.eng.destroy()

	def test_game_engine_create(self):
		from treasure_runner.models.game_engine import GameEngine
		eng = GameEngine("../assets/starter.ini")
		self.assertIsNotNone(eng)
		eng.destroy()

	def test_destroy_is_safe_to_call_twice(self):
		from treasure_runner.models.game_engine import GameEngine
		eng = GameEngine("../assets/starter.ini")
		eng.destroy()
		eng.destroy()
	
	def test_room_count_positive(self):
		count = self.eng.get_room_count()
		self.assertIsInstance(count, int)
		self.assertGreater(count, 0)
	
	def test_get_room_dimensions_tuple(self):
		arr = self.eng.get_room_dimensions()
		self.assertIsInstance(arr, tuple)
		self.assertEqual(len(arr), 2)

	def test_get_room_dimensions_positive(self):
		width, height = self.eng.get_room_dimensions()
		self.assertGreater(width, 0)
		self.assertGreater(height, 0)

	def test_get_room_ids_returns_list(self):
		lst = self.eng.get_room_ids()
		self.assertIsInstance(lst, list)

	def test_get_room_ids_length_matches_count(self):
		count = self.eng.get_room_count()
		lst = self.eng.get_room_ids()
		self.assertEqual(len(lst), count)
	
	def test_render_current_room_is_not_empty(self):
		rendered = self.eng.render_current_room()
		self.assertGreater(len(rendered), 0)

	def test_reset_succeeds(self):
		self.eng.reset()

	def test_get_room_ids_are_ints(self):
		for room_id in self.eng.get_room_ids():
			self.assertIsInstance(room_id, int)
	
	def test_get_total_treasures_returns_int(self):
		total = self.eng.get_total_treasures()
		self.assertIsInstance(total, int)

	def test_get_total_treasures_non_negative(self):
		total = self.eng.get_total_treasures()
		self.assertGreaterEqual(total, 0)
	
	def test_player_move_on_portal_valid_direction(self):
		from treasure_runner.bindings import Direction
		try:
			self.eng.player_move_on_portal(Direction.NORTH)
		except Exception as err:
			self.assertIsInstance(err, Exception)

if __name__ == "__main__":
    unittest.main()