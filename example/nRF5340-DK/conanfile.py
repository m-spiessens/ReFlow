from conans import ConanFile, CMake
import os

class FlowBlinky(ConanFile):
   requires = "Flow/2.0@spiessensm/dev", "nrfx/2.4.0@nordic/stable"
   settings = { "arch": ["armv8_32"], "os": ["none"], "build_type": ["Release", "Debug"], "compiler": ["gcc"] }
   generators = "cmake"

   def imports(self):
      self.copy("*.h")
      self.copy("platform*.cpp", "source/flow/", "source/flow/")

   def build(self):
      cmake = CMake(self, generator="Eclipse CDT4 - Unix Makefiles")
      cmake.verbose = True

      cmake.definitions["CONAN_C_FLAGS"] = "-march=armv8-m.main -mthumb"
      cmake.definitions["CONAN_CXX_FLAGS"] = "-march=armv8-m.main -mthumb -fno-exceptions"
      cmake.definitions["DEFINE_PART"] = "$DEFINE_PART"

      cmake.configure()
      cmake.build()

      self.run("rm -f arm-none-eabi-gdb; ln -s `which arm-none-eabi-gdb`")