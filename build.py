import os, sys

if len(sys.argv) == 1 or sys.argv[1] not in ["Debug", "Release"]:
    print("Usage: python3 build.py <Debug|Release>")
    sys.exit(1)

os.system("git clean -Xfd")
os.system("conan profile detect -f")
os.system("conan install . --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --settings=build_type=" + sys.argv[1])
os.system("cmake . -DCMAKE_BUILD_TYPE=" + sys.argv[1])
os.system("cmake --build . --config " + sys.argv[1])