import os, sys, shutil


def execute(command, returnIfError=False):
    code = os.system(command)
    if code != 0:
        if returnIfError:
            return code
        sys.exit(code)
    if returnIfError:
        return code


if len(sys.argv) == 1 or sys.argv[1] not in ["Debug", "Release"]:
    print("Usage: python3 build.py <Debug|Release>")
    sys.exit(1)

vscode = os.path.exists(".vscode")
shaders = os.path.exists("shaders")
textures = os.path.exists("textures")

if vscode:
    shutil.move(".vscode", "b.vscode")
if shaders:
    shutil.move("shaders", "b.shaders")
if textures:
    shutil.move("textures", "b.textures")

result = execute("git clean -Xfd", True)

if vscode:
    shutil.move("b.vscode", ".vscode")
if shaders:
    shutil.move("b.shaders", "shaders")
if textures:
    shutil.move("b.textures", "textures")

if result != 0:
    print("Failed to clean the repository")
    sys.exit(result)

execute("conan profile detect -f")
execute("conan install . --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --settings=build_type=" + sys.argv[1])
execute("cmake . -DCMAKE_BUILD_TYPE=" + sys.argv[1])
execute("cmake --build . --config " + sys.argv[1])