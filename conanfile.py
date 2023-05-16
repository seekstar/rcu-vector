from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy

class rcu_vectorRecipe(ConanFile):
    name = "rcu-vector"
    version = "0.1.0"

    # Optional metadata
    license = "MPLv2"
    author = "Jiansheng Qiu jianshengqiu.cs@gmail.com"
    url = "https://github.com/seekstar/rcu_vector"
    description = "Dynamically resizable vector implemented with RCU (Read-Copy-Update)."
    topics = ("RCU", "C++", "vector")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "include/*"
    no_copy_source = True
    generators = "CMakeToolchain", "CMakeDeps"

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        # For header-only packages, libdirs and bindirs are not used
        # so it's necessary to set those as empty.
        self.cpp_info.components["rcu_vector_flavor"].set_property("cmake_target_name", "rcu_vector_flavor")
        self.cpp_info.components["rcu_vector_flavor"].libdirs = []
        self.cpp_info.components["rcu_vector_flavor"].bindirs = []

        self.cpp_info.components["rcu_vector"].system_libs = ["urcu"]
        self.cpp_info.components["rcu_vector"].requires = ["rcu_vector_flavor"]
        self.cpp_info.components["rcu_vector"].set_property("cmake_target_name", "rcu_vector")
        self.cpp_info.components["rcu_vector"].libdirs = []
        self.cpp_info.components["rcu_vector"].bindirs = []

        self.cpp_info.components["rcu_vector_bp"].system_libs = ["urcu-bp"]
        self.cpp_info.components["rcu_vector_bp"].requires = ["rcu_vector_flavor"]
        self.cpp_info.components["rcu_vector_bp"].set_property("cmake_target_name", "rcu_vector_bp")
        self.cpp_info.components["rcu_vector_bp"].libdirs = []
        self.cpp_info.components["rcu_vector_bp"].bindirs = []

    def package_id(self):
        self.info.clear()
