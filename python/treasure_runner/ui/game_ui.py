import os
import curses
from datetime import datetime, timezone
from treasure_runner.bindings import Direction
from treasure_runner.models.exceptions import GameError, ImpassableError, NoPortalError

class GameUI:

    MOVEKEYS = {curses.KEY_UP: Direction.NORTH,
                curses.KEY_DOWN: Direction.SOUTH,
                curses.KEY_LEFT: Direction.WEST,
                curses.KEY_RIGHT: Direction.EAST,
                ord("w"): Direction.NORTH,
                ord("W"): Direction.NORTH,
                ord("s"): Direction.SOUTH,
                ord("S"): Direction.SOUTH,
                ord("a"): Direction.WEST,
                ord("A"): Direction.WEST,
                ord("d"): Direction.EAST,
                ord("D"): Direction.EAST}

    LEGEND = ["Game Elements:",
              "@ - player",
              "# - wall",
              "$ - treasure",
              ". - floor",
              "X - portal",
			  "O - pushable"]

    def __init__(self, stdscr, engine, profile):
        self.stdscr = stdscr
        self.engine = engine
        self.profile = profile
        self.message = "Welcome to Treasure Runner!!! Use WASD or arrow keys to move."
        self.rooms_visited = set()
        self.running = True
        self.won = False
        self.rooms_visited.add(self.engine.player.get_room())
        curses.curs_set(0)
        self.stdscr.keypad(True)

    def run(self):
        self._terminal_size()
        self._start_screen()

        while self.running:
            self.draw()
            key = self.stdscr.getch()
            self.handle_input(key)

        self._update_profile()
        if self.won:
            self._victory_screen()
        else:
            self._end_screen()

    def draw(self):
        self._terminal_size()
        self.stdscr.clear()

        height, width = self.stdscr.getmaxyx()
        row = 0
        row = self._message_bar(row, width)
        row = self._room_header(row, width)
        row = self._main(row, height, width)
        row = self._controls(row, height, width)
        self._status_bar(row, height, width)
        self._footer(height, width)
        self.stdscr.refresh()

    def _message_bar(self, row, width):
        msg = self.message
        self._addstr(row, 0, msg[:width-1])
        return row + 1

    def _room_header(self, row, width):
        room_id = self.engine.player.get_room()
        header = "Room " + str(room_id)
        self._addstr(row, 0, header[:width-1])
        return row + 1

    def _main(self, row, height, width):
        room_str = self.engine.render_current_room()
        lines = room_str.strip().split("\n")
        display_width = 0

        for line in lines:
            if len(line) > display_width:
                display_width = len(line)

        col = display_width + 3

        for i, line in enumerate(lines):
            if row >= height - 4:
                break
            self._addstr(row, 0, line[:width-1])
            if i < len(self.LEGEND):
                if col + len(self.LEGEND[i]) < width:
                    self._addstr(row, col, self.LEGEND[i])
            row += 1
        return row + 1

    def _controls(self, row, height, width):
        controls = "Game Controls: WASD/Arrows to move | > to go through a portal | u to undo | c to reset current room | r to reset | q to quit"
        if row < height - 3:
            self._addstr(row, 0, controls[:width-1])
            return row + 1
        return row

    def _status_bar(self, row, height, width):
        if row >= height - 1:
            return
        player = self.engine.player
        collected = player.get_collected_count()
        total_treasures = self.engine.get_total_treasures()
        room_count = self.engine.get_room_count()
        name = self.profile.get("player_name", "")

        status = ("Player: " + name + " | Treasures: " + str(collected) + "/" + str(total_treasures) + " | Rooms: " + str(len(self.rooms_visited)) + "/" + str(room_count))
        self._addstr(row, 0, status[:width-1])

    def _footer(self, height, width):
        title = "Treasure Runner"
        email = "wsyed01@uoguelph.ca"
        row_footer = height - 1
        self._addstr(row_footer, 0, title)

        email_col = width - len(email) - 1
        if email_col > len(title) + 1:
            self._addstr(row_footer, email_col, email)

    def handle_input(self, key):
        if key == ord("q") or key == ord("Q"):
            self.running = False
            return
        if key == ord("r") or key == ord("R"):
            self.engine.reset()
            self.rooms_visited.clear()
            self.rooms_visited.add(self.engine.player.get_room())
            self.message = "Game Reset"
            return
        if key == ord(">"):
            self._handle_portal()
            return
        if key == ord("u") or key == ord("U"):
            self._undo_move()
            return
        if key == ord("c") or key == ord("C"):
            self._current_room_reset()
            return
        move_key = self.MOVEKEYS.get(key)
        if move_key is not None:
            self._movement(move_key)

    def _undo_move(self):
        try:
            self.engine.undo_move()
            self.message = "Move Undone"
        except GameError:
            self.message = "Unable to Undo"

    def _current_room_reset(self):
        try:
            self.engine.reset_current_room()
            self.message = "Reset Current Room to Starting State"
        except GameError as err:
            self.message = "Error: " + str(err)

    def _handle_portal(self):
        try:
            self.engine.use_portal()
        except NoPortalError:
            self.message = "No portal. Walk onto a portal (X)."
            return
        except GameError as err:
            self.message = "Error" + str(err)
            return
        self.rooms_visited.add(self.engine.player.get_room())
        self.message = "Entered Room " + str(self.engine.player.get_room())

    def _movement(self, moves):
        before_room = self.engine.player.get_room()
        before_collected = self.engine.player.get_collected_count()
        try:
            self.engine.player_move_with_undo(moves)
        except ImpassableError:
            self.message = "You can't go that way!!!"
            return
        except GameError as err:
            self.message = "Error" + str(err)
            return
        after_room = self.engine.player.get_room()
        after_collected = self.engine.player.get_collected_count()
        total_treasures = self.engine.get_total_treasures()

        if after_collected > before_collected:
            dif = after_collected - before_collected
            self.message = "Picked up " + str(dif) + " treasure " " (" + str(after_collected) + "/" + str(total_treasures) + " treasures collected)"
            self._check_victory(after_collected, total_treasures)
        elif after_room != before_room:
            self.rooms_visited.add(after_room)
            self.message = "Entered Room " + str(after_room)
        else:
            self.message = ""

    def _check_victory(self, collected, total):
        if collected == total:
            self.won = True
            self.running = False

    def _start_screen(self):
        self.stdscr.clear()
        height, width = self.stdscr.getmaxyx()
        display_profile = self.profile

        start_lines = [
            "=============================",
            "TREASURE RUNNER",
            "=============================",
            "",
            "  Player:          " + str(display_profile["player_name"]),
            "  Games Played:    " + str(display_profile["games_played"]),
            "  Best Treasure:   " + str(display_profile["max_treasure_collected"]),
            "  Best Rooms:      " + str(display_profile["most_rooms_world_completed"]),
            "          Last Played:     " + str(display_profile["timestamp_last_played"] or "Never"),
            "",
            "  Press any key to start..."
            ]
        start_row = max(0, (height - len(start_lines)) // 2)
        for i, line in enumerate(start_lines):
            col = max(0, (width - len(line)) // 2)
            if start_row + i < height:
                self._addstr(start_row + i, col, line[:width - 1])
        self.stdscr.refresh()
        self.stdscr.getch()

    def _end_screen(self):
        self.stdscr.clear()
        height, width = self.stdscr.getmaxyx()
        display_profile = self.profile
        vis = len(self.rooms_visited)
        collected = self.engine.player.get_collected_count()

        end_lines = [
            " =============================",
            " GAME OVER",
            " =============================",
            "",
            "  This Run:",
            "    Treasure Collected: " + str(collected),
            "    Rooms Visited:      " + str(vis),
            "",
            " Profile Summary:",
            "    Player:          " + str(display_profile["player_name"]),
            "    Games Played:    " + str(display_profile["games_played"]),
            "    Best Treasure:   " + str(display_profile["max_treasure_collected"]),
            "    Best Rooms:      " + str(display_profile["most_rooms_world_completed"]),
            "    Last Played:     " + str(display_profile["timestamp_last_played"]),
            "",
            "  Press any key to exit..."
            ]
        start_row = max(0, (height - len(end_lines)) // 2)
        for i, line in enumerate(end_lines):
            if start_row + i < height:
                self._addstr(start_row + i, max(0, (width - len(line)) // 2), line[:width - 1])
        self.stdscr.refresh()
        self.stdscr.getch()

    def _victory_screen(self):
        self.stdscr.clear()
        height, width = self.stdscr.getmaxyx()
        vis = len(self.rooms_visited)
        collected = self.engine.player.get_collected_count()
        display_profile = self.profile

        victory_lines = [
            " =============================",
            " YOU WIN!!!",
            " =============================",
            "",
			"  All Treasures Collected!!!",
			"",
            "  This Run:",
            "    Treasure Collected: " + str(collected),
            "    Rooms Visited:      " + str(vis),
            "",
            " Profile Summary:",
            "    Player:          " + str(display_profile["player_name"]),
            "    Games Played:    " + str(display_profile["games_played"]),
            "    Best Treasure:   " + str(display_profile["max_treasure_collected"]),
            "    Best Rooms:      " + str(display_profile["most_rooms_world_completed"]),
            "    Last Played:     " + str(display_profile["timestamp_last_played"]),
            "",
            "  Press any key to exit..."
            ]
        start_row = max(0, (height - len(victory_lines)) // 2)
        for i, line in enumerate(victory_lines):
            if start_row + i < height:
                self._addstr(start_row + i, max(0, (width - len(line)) // 2), line[:width - 1])
        self.stdscr.refresh()
        self.stdscr.getch()

    def _update_profile(self):
        vis = len(self.rooms_visited)
        collected = self.engine.player.get_collected_count()
        self.profile["games_played"] += 1
        if collected > self.profile["max_treasure_collected"]:
            self.profile["max_treasure_collected"] = collected
        if vis > self.profile["most_rooms_world_completed"]:
            self.profile["most_rooms_world_completed"] = vis
        self.profile["timestamp_last_played"] = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")

    def _terminal_size(self):
        height, width = self.stdscr.getmaxyx()
        temp_width, temp_height = self.engine.get_room_dimensions()
        min_height = temp_height + 7
        min_width = temp_width + 25
        if height < min_height or width < min_width:
            raise RuntimeError("Terminal too small!!!")

    def _addstr(self, y, x, text, atr=0):
        try:
            self.stdscr.addstr(y, x, text, atr)
        except curses.error:
            pass
