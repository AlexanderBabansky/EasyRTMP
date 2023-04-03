from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout

class EasyRTMP(ConanFile):
    name = "easyrtmp"
    version = "0.2"

    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*"

    def layout(self):
        print("LAYOUT")
        cmake_layout(self)

    def generate(self):
        print("GENERATE")
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        print("BUILD")
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        print("PACKAGE")
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        print("PACKAGE INFO")
        self.cpp_info.libs = ["easyrtmp"]
