#!/usr/bin/env python3
"""Deterministic system integration test runner for Treasure Runner."""

# below are the imports that are in the instructors solution.
# you may need different ones.  Use them if you wish.
import os
import argparse
from treasure_runner.bindings import Direction
from treasure_runner.models.game_engine import GameEngine
from treasure_runner.models.exceptions import GameError, ImpassableError

DIRECTION_ORDER = [Direction.SOUTH, Direction.WEST, Direction.NORTH, Direction.EAST]
SWEEP_ORDER = [Direction.SOUTH, Direction.WEST, Direction.NORTH, Direction.EAST]

# put your functions here here
def build_player(player) -> str:
	room = player.get_room()
	x, y = player.get_position()
	c = player.get_collected_count()
	return f"room={room}|x={x}|y={y}|collected={c}"

def player_snapshot(player) -> tuple:
	room = player.get_room()
	x, y = player.get_position()
	c = player.get_collected_count()
	return (room, x, y, c)

def find_direction(eng, player):
	for d in DIRECTION_ORDER:
		eng.reset()
		before_r = player.get_room()
		before_p = player.get_position()
		try:
			eng.move_player(d)
		except ImpassableError:
			continue
		except GameError:
			eng.reset()
			continue
		after_r = player.get_room()
		after_p = player.get_position()
		if before_r == after_r and before_p != after_p:
			eng.reset()
			return d
	return None

def sweep(eng, player, sweep_direction, step, log):
	phase = f"SWEEP_{sweep_direction.name}"
	states_seen = []
	states_seen.append(player_snapshot(player))
	sweep_moves = 0
	reason = "BLOCKED"

	log.append(f"SWEEP_START|phase={phase}|dir={sweep_direction.name}")
	while True:
		step += 1
		before_state = build_player(player)
		before_c = player.get_collected_count()
		before_snap = player_snapshot(player)
		try:
			eng.move_player(sweep_direction)
		except ImpassableError:
			after_state = build_player(player)
			delta_collected = player.get_collected_count() - before_c
			log.append(f"MOVE|step={step}|phase={phase}|dir={sweep_direction.name}|result=BLOCKED|before={before_state}|after={after_state}|delta_collected={delta_collected}")
			reason = "BLOCKED"
			break
		except GameError:
			after_state = build_player(player)
			delta_collected = player.get_collected_count() - before_c
			if delta_collected > 0:
				log.append(f"MOVE|step={step}|phase={phase}|dir={sweep_direction.name}|result=OK|before={before_state}|after={after_state}|delta_collected={delta_collected}")
				sweep_moves += 1
				states_seen.append(player_snapshot(player))
			else:
				log.append(f"MOVE|step={step}|phase={phase}|dir={sweep_direction.name}|result=ERROR|before={before_state}|after={after_state}|delta_collected={delta_collected}")
				reason = "BLOCKED"
				break
		after_state = build_player(player)
		after_snap = player_snapshot(player)
		delta_collected = player.get_collected_count() - before_c
		if before_snap == after_snap:
			log.append(f"MOVE|step={step}|phase={phase}|dir={sweep_direction.name}|result=BLOCKED|before={before_state}|after={after_state}|delta_collected={delta_collected}")
			reason = "BLOCKED"
			break
		if after_snap in states_seen:
			log.append(f"MOVE|step={step}|phase={phase}|dir={sweep_direction.name}|result=OK|before={before_state}|after={after_state}|delta_collected={delta_collected}")
			reason = "CYCLE_DETECTED"
			sweep_moves += 1
			break
		states_seen.append(after_snap)
		log.append(f"MOVE|step={step}|phase={phase}|dir={sweep_direction.name}|result=OK|before={before_state}|after={after_state}|delta_collected={delta_collected}")
		sweep_moves += 1
	log.append(f"SWEEP_END|phase={phase}|reason={reason}|moves={sweep_moves}")
	return step

def integration(config_path, log_path):
	log = []
	eng = GameEngine(config_path)
	player = eng.player
	step = 0

	room_count = eng.get_room_count()
	width, height = eng.get_room_dimensions()
	log.append(f"RUN_START|config={config_path}|rooms={room_count}|room_width={width}|room_height={height}")
	log.append(f"STATE|step={step}|phase=SPAWN|state={build_player(player)}")
	direction = find_direction(eng, player)
	if direction is not None:
		dir_name = direction.name
	else:
		dir_name = "NONE"
	log.append(f"ENTRY|direction={dir_name}")
	eng.reset()
	step = 1
	before_state = build_player(player)
	before_c = player.get_collected_count()
	if direction is None:
		after_state = build_player(player)
		log.append(f"MOVE|step={step}|phase=ENTRY|dir=NONE|result=ERROR|before={before_state}|after={after_state}|delta_collected=0")
		log.append("TERMINATED: Initial Move Error")
		log.append(f"RUN_END|steps={step}|collected_total={player.get_collected_count()}")
		write_log(log_path, log)
		eng.destroy()
		return
	result = "OK"
	try:
		eng.move_player(direction)
		after_state = build_player(player)
		delta_collected = player.get_collected_count() - before_c
	except GameError:
		after_state = build_player(player)
		delta_collected = 0
		result = "ERROR"
	log.append(f"MOVE|step={step}|phase=ENTRY|dir={direction.name}|result={result}|before={before_state}|after={after_state}|delta_collected={delta_collected}")
	if result == "ERROR":
		log.append("TERMINATED: Initial Move Error")
		log.append(f"RUN_END|steps={step}|collected_total={player.get_collected_count()}")
		write_log(log_path, log)
		eng.destroy()
		return
	for sweep_direction in SWEEP_ORDER:
		step = sweep(eng, player, sweep_direction, step, log)
	log.append(f"STATE|step={step}|phase=FINAL|state={build_player(player)}")
	log.append(f"RUN_END|steps={step}|collected_total={player.get_collected_count()}")
	write_log(log_path, log)
	eng.destroy()

def write_log(log_path, log):
	with open(log_path, "w", encoding="utf-8") as f:
		for lines in log:
			f.write(lines + "\n")


def parse_args():
    parser = argparse.ArgumentParser(description="Treasure Runner integration test logger")
    parser.add_argument(
        "--config",
        required=True,
        help="Path to generator config file",
    )
    parser.add_argument(
        "--log",
        required=True,
        help="Output log path",
    )
    return parser.parse_args()


def main():
    args = parse_args()
    config_path = os.path.abspath(args.config)
    log_path = os.path.abspath(args.log)
    #call your main function here
    integration(config_path, log_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
