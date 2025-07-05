from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain

class ColibriConan(ConanFile):
    name = "colibri"
    version = "0.15.0"
    description = "Colibri library"
    license = "MIT"
    url = "https://github.com/your-repo/colibri"
    homepage = "https://github.com/your-repo/colibri"
    topics = ("library", "data-structures")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "threads": [True, False],
    }
    default_options = {
        "shared": False,
        "threads": True,
    }
    generators = "CMakeDeps"

    def requirements(self):
        self.requires("picotest/1.4.2")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["USE_THREADS"] = self.options.threads
        tc.generate()
