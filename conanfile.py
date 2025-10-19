from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy, rmdir
import os


required_conan_version = ">=2.0.9"


class EasyRTMPConan(ConanFile):
    name = "easyrtmp"
    description = "RTMP streaming library"
    license = "LGPL-3.0-only"
    homepage = "https://github.com/AlexanderBabansky/EasyRTMP"
    package_type = "library"
    settings = "os", "arch", "compiler", "build_type"
    version = "1.0"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_openssl": [True, False],
        "with_network": [True, False]
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_openssl": False,
        "with_network": True
    }
    implements = ["auto_shared_fpic"]

    def export_sources(self):
        copy(self, "CMakeLists.txt", self.recipe_folder,
             self.export_sources_folder)
        copy(self, "*.cpp", self.recipe_folder, self.export_sources_folder)
        copy(self, "*.h", self.recipe_folder, self.export_sources_folder)
        copy(self, "*.in", self.recipe_folder, self.export_sources_folder)
        copy(self, "*.cmake", self.recipe_folder, self.export_sources_folder)
        copy(self, "LICENSE", self.recipe_folder, self.export_sources_folder)
        rmdir(self, os.path.join(self.export_sources_folder, "test_package"))
        rmdir(self, os.path.join(self.export_sources_folder, ".git"))

    def layout(self):
        cmake_layout(self, src_folder=".")

    def requirements(self):
        if self.options.with_openssl:
            self.requires("openssl/[>=1.1 <4]")

    def validate(self):
        check_min_cppstd(self, 11)

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.16 <4]")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["USE_OPENSSL"] = self.options.with_openssl
        tc.cache_variables["BUILD_WITH_NETWORK"] = self.options.with_network
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "LICENSE", self.source_folder,
             os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()

        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        self.cpp_info.libs = ["easyrtmp"]
