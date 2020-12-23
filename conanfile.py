from conans import ConanFile, CMake


class ArgunaughtConan(ConanFile):
    name = "argunaught"
    version = "0.1"
    license = "MIT"
    author = "Jeff DeWall"
    url = "https://gitlab.com/sr.jilarious/argunaught"
    description = "A C++17 argument parser that handles sub commands."
    topics = ("C++17", "argument", "option", "cli")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "cmake"
    exports_sources = "argunaught/*"

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="argunaught")
        cmake.build()

        # Explicit way:
        # self.run('cmake %s/hello %s'
        #          % (self.source_folder, cmake.command_line))
        # self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        self.copy("*.h", dst="include/argunaught", src="argunaught/include/argunaught")
        self.copy("*.hpp", dst="include/argunaught", src="argunaught/include/argunaught")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["argunaught"]
