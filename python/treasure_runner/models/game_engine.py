import ctypes
from ..bindings import lib, Status, Direction, GameEngine as CGameEngine
from .exceptions import status_to_exception
from .player import Player

class GameEngine:
    def __init__(self, config_path: str):
        ptr = CGameEngine(None)
        status_out = lib.game_engine_create(config_path.encode("utf-8"), ctypes.byref(ptr))
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_create failed: {status_out}")
        self._eng: CGameEngine = ptr
        self._destroy: bool = False
        cur_player = lib.game_engine_get_player(self._eng)
        if cur_player is None:
            raise RuntimeError("game_engine_get_player is NULL")
        self._player = Player(cur_player)

    @property
    def player(self) -> Player:
        return self._player

    def destroy(self) -> None:
        if not self._destroy:
            lib.game_engine_destroy(self._eng)
            self._destroy = True

    def move_player(self, direction: Direction) -> None:
        status_out = lib.game_engine_move_player(self._eng, int(direction))
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_move_player failed: {status_out}")

    def render_current_room(self) -> str:
        str_out = ctypes.c_char_p(None)
        status_out = lib.game_engine_render_current_room(self._eng, ctypes.byref(str_out))
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_render_current_room failed: {status_out}")
        if str_out.value is not None:
            string = str_out.value.decode("utf-8")
        else:
            string = ""
        lib.game_engine_free_string(str_out)
        return string

    def get_room_count(self) -> int:
        count = ctypes.c_int(0)
        status_out = lib.game_engine_get_room_count(self._eng, ctypes.byref(count))
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_get_room_count failed: {status_out}")
        return count.value

    def get_room_dimensions(self) -> tuple[int, int]:
        room_width = ctypes.c_int(0)
        room_height = ctypes.c_int(0)
        lib.game_engine_get_room_dimensions(self._eng, ctypes.byref(room_width), ctypes.byref(room_height))
        return (room_width.value, room_height.value)

    def get_room_ids(self) -> list[int]:
        ptr = ctypes.POINTER(ctypes.c_int)()
        count = ctypes.c_int(0)
        status_out = lib.game_engine_get_room_ids(self._eng, ctypes.byref(ptr), ctypes.byref(count))
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_get_room_ids failed: {status_out}")
        ids = []
        for i in range(count.value):
            ids.append(ptr[i])
        lib.game_engine_free_string(ptr)
        return ids

    def reset(self) -> None:
        status_out = lib.game_engine_reset(self._eng)
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_reset failed: {status_out}")

    def get_total_treasures(self):
        total = ctypes.c_int(0)
        status_out = lib.game_engine_get_total_treasures(self._eng, ctypes.byref(total))
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_get_total_treasures failed: {status_out}")
        return total.value

    def use_portal(self) -> None:
        status_out = lib.game_engine_use_portal(self._eng)
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_use_portal failed: {status_out}")

    def player_move_on_portal(self, direction) -> None:
        status_out = lib.game_engine_player_move_on_portal(self._eng, int(direction))
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_player_move_on_portal failed: {status_out}")

    def player_move_with_undo(self, direction) -> None:
        status_out = lib.game_engine_player_move_with_undo(self._eng, int(direction))
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_player_move_with_undo failed: {status_out}")

    def undo_move(self) -> None:
        status_out = lib.game_engine_undo_move(self._eng)
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_undo_move failed: {status_out}")

    def reset_current_room(self) -> None:
        status_out = lib.game_engine_reset_current_room(self._eng)
        if status_out != Status.OK:
            raise status_to_exception(status_out, f"game_engine_reset_current_room failed: {status_out}")
