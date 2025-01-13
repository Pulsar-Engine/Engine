import os, sys, shutil

def execute(command):
    code = os.system(command)
    if code != 0:
        sys.exit(code)

if len(sys.argv) == 1 or sys.argv[1] not in ["Debug", "Release"]:
    print("Usage: python3 build.py <Debug|Release>")
    sys.exit(1)

vscode = os.path.exists(".vscode")

if vscode:
    shutil.move(".vscode", "b.vscode")
execute("git clean -Xfd")
if vscode:
    shutil.move("b.vscode", ".vscode")
execute("conan profile detect -f")
execute("conan install . --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --settings=build_type=" + sys.argv[1])
execute("cmake . -DCMAKE_BUILD_TYPE=" + sys.argv[1])
execute("cmake --build . --config " + sys.argv[1])