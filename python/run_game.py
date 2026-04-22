import os
import curses
import argparse
import sys
import json
from treasure_runner.models.game_engine import GameEngine
from treasure_runner.ui.game_ui import GameUI

def parse_args():
    parse = argparse.ArgumentParser(description="Treasure Runner")
    parse.add_argument("--config", required = True, help = "Path to .ini file")
    parse.add_argument("--profile", required = True, help = "Path to player profile JSON file")
    return parse.parse_args()

def load_profile(profile_path):
    if os.path.exists(profile_path):
        with open(profile_path, "r", encoding="utf-8") as file:
            return json.load(file)
    return {"player_name": "",
            "games_played": 0,
            "max_treasure_collected": 0,
            "most_rooms_world_completed": 0,
            "timestamp_last_played": ""}

def save_profile(profile_path, profile):
    dr = os.path.dirname(profile_path)
    if dr:
        os.makedirs(dr, exist_ok=True)
    with open(profile_path, "w", encoding="utf-8") as file:
        json.dump(profile, file, indent=2)

def main():
    args = parse_args()
    here = os.path.dirname(os.path.abspath(__file__))
    if here not in sys.path:
        sys.path.insert(0, here)
    
    config_path = os.path.abspath(args.config)
    profile_path = os.path.abspath(args.profile)

    profile = load_profile(profile_path)
    if not profile.get("player_name"):
        name = ""
        while not name.strip():
            name = input("Enter player name: ")
        profile["player_name"] = name.strip()
    
    engine = GameEngine(config_path)

    def curses_main(stdscr):
        ui = GameUI(stdscr, engine, profile)
        ui.run()

    curses.wrapper(curses_main)
    save_profile(profile_path, profile)
    engine.destroy()

if __name__ == "__main__":
    main()