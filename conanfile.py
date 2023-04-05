from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout

class EasyRTMP(ConanFile):
    name = "easyrtmp"
    version = "0.2"

    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*"

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["easyrtmp"]
