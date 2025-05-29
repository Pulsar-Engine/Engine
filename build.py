import os
import sys
import shutil

to_restore = []


def execute(command, returnIfError=False):
    code = os.system(command)
    if code != 0:
        if returnIfError:
            return code
        sys.exit(code)
    if returnIfError:
        return code


def backup_and_move(path, backup_path):
    global to_restore
    if os.path.exists(path):
        shutil.move(path, backup_path)
        to_restore.append((backup_path, path))


def restore_files():
    global to_restore
    for backup_path, original_path in to_restore:
        if os.path.exists(backup_path):
            shutil.move(backup_path, original_path)


if len(sys.argv) == 1 or sys.argv[1] not in ["Debug", "Release"]:
    print("Usage: python3 build.py <Debug|Release>")
    sys.exit(1)

backup_and_move(".vscode", "b.vscode")
backup_and_move("shaders", "b.shaders")
backup_and_move("textures", "b.textures")
backup_and_move("models", "b.models")

result = execute("git clean -Xfd", True)

restore_files()

if result != 0:
    print("Failed to clean the repository")
    sys.exit(result)

execute("conan profile detect -f")
execute("conan install . --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --settings=build_type=" + sys.argv[1])
execute("cmake . -DCMAKE_BUILD_TYPE=" + sys.argv[1])
execute("cmake --build . --config " + sys.argv[1])