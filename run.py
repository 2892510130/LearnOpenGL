import os
import argparse
import platform
import subprocess

parser = argparse.ArgumentParser(description="Run the exe")
parser.add_argument(
    "--config_type",
    default="Release",
    choices=["Release", "Debug", "RelWithDebInfo", "MinSizeRel"],
    help="Specify the build type (default: Release)"
)

args = parser.parse_args()
config_type = args.config_type

system_name = platform.machine()

folder_path = f"build/{system_name}/{config_type}/bin"

exe_files, idx = [], 0
for root, dirs, files in os.walk(folder_path):
    for file in files:
        if file.endswith('.exe'):
            exe_files.append(os.path.join(root, file))
            print(f"  {idx}: {file}")
            idx += 1

print("Please choose one test: ")
choice = int(input())

exe_path = exe_files[choice]

print(f"Running the {exe_path}.")

subprocess.run([exe_path], check=True)
