from conans import ConanFile, CMake
import os

class FlowBlinky(ConanFile):
   requires = "Flow/2.0@spiessensm/dev"
   settings = { "arch": ["armv7hf"], "os": ["none"], "build_type": ["Release", "Debug"], "compiler": ["gcc"] }
   generators = "cmake"

   def imports(self):
      self.copy("*.h")
      self.copy("platform*.cpp", "source/flow/", "source/flow/")

   def build(self):
      cmake = CMake(self, generator="Eclipse CDT4 - Unix Makefiles")
      # cmake.verbose = True

      cmake.definitions["CONAN_C_FLAGS"] = "-march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16"
      cmake.definitions["CONAN_CXX_FLAGS"] = "-march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-exceptions"
      cmake.definitions["DEFINE_PART"] = "$DEFINE_PART"

      cmake.configure()
      cmake.build()

      self.run("rm -f arm-none-eabi-gdb; ln -s `which arm-none-eabi-gdb`")