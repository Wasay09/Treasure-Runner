import ctypes
from ..bindings import lib, Status
from .exceptions import status_to_exception

class Player:
    def __init__(self, ptr):
        self._ptr = ptr

    def get_room(self) -> int:
        return lib.player_get_room(self._ptr)

    def get_position(self) -> tuple[int, int]:
        x = ctypes.c_int()
        y = ctypes.c_int()
        lib.player_get_position(self._ptr, ctypes.byref(x), ctypes.byref(y))
        return x.value, y.value

    def get_collected_count(self) -> int:
        return lib.player_get_collected_count(self._ptr)

    def has_collected_treasure(self, treasure_id: int) -> bool:
        return bool(lib.player_has_collected_treasure(self._ptr, treasure_id))

    def get_collected_treasures(self) -> list[dict]:
        count = ctypes.c_int(0)
        ptr = lib.player_get_collected_treasures(self._ptr, ctypes.byref(count))

        lst = []
        for i in range(count.value):
            collect_treasure = ptr[i].contents
            if collect_treasure.name:
                name = collect_treasure.name.decode("utf-8")
            else:
                name = ""
            lst.append({"id": collect_treasure.id,
						"name": name,
						"starting_room_id": collect_treasure.starting_room_id,
						"initial_x": collect_treasure.initial_x,
                        "initial_y": collect_treasure.initial_y,
						"x": collect_treasure.x,
						"y": collect_treasure.y,
						"collected": collect_treasure.collected})
        return lst
