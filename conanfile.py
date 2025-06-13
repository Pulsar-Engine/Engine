from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain
import os
import shutil


class MyProjectConan(ConanFile):
    name = "my_project"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "vulkan-headers/1.3.290.0",
        "vulkan-loader/1.3.290.0",
        "glfw/3.4",
        "glm/0.9.9.8"
    )
    default_options = {
        "*:shared": True
    }

    def layout(self):
        self.folders.build = "build"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()
        os.makedirs(self.folders.build, exist_ok=True)

        for dep_name, dep in self.dependencies.host.items():
            dep_bin_folder = dep.package_folder
            dll_paths = [os.path.join(dep_bin_folder, "bin"), os.path.join(dep_bin_folder, "lib")]
            for path in dll_paths:
                if os.path.isdir(path):
                    for root, dirs, files in os.walk(path):
                        for file in files:
                            if file.endswith((".dll", ".so", ".dylib", ".a", ".lib")):
                                src_file = os.path.join(root, file)
                                dst_file = os.path.join(self.folders.build, file)
                                print(f"Copying {src_file} to {dst_file}")
                                shutil.copy(src_file, dst_file)