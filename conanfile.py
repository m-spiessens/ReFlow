from conans import ConanFile, CMake

class Flow(ConanFile):
	name = "Flow"
	version = "2.0"
	description = """Flow is a pipes and filters implementation tailored for microcontrollers. 
		It provides 3 base concepts: component, port and connection."""
	url = "https://gitlab.com/m-spiessens/Flow"
	license = "MIT"
	author = "Mathias Spiessens"
	build_policy = "missing"
	settings = { "arch": ["x86", "x86_64", "armv6", "armv7", "armv7hf", "armv8_32"], "os": ["none", "Linux"], "build_type": ["Release", "Debug"], "compiler": ["gcc"] }
	generators = "cmake"
	exports_sources = "include/*", "source/*", "CMakeLists.txt"

	def build(self):
		cmake = CMake(self)
		# cmake.verbose = True

		if self.settings.os == "none":
			cmake.definitions["CMAKE_TRY_COMPILE_TARGET_TYPE"] = "STATIC_LIBRARY"

		if self.settings.arch == "armv6":
			cmake.definitions["CMAKE_CXX_FLAGS"] = "-march=armv6-m -mthumb -fno-rtti -fno-exceptions -fdata-sections -ffunction-sections"
		elif self.settings.arch == "armv7":
			cmake.definitions["CMAKE_CXX_FLAGS"] = "-march=armv7e-m -mthumb -fno-rtti -fno-exceptions -fdata-sections -ffunction-sections"
		elif self.settings.arch == "armv7hf":
			cmake.definitions["CMAKE_CXX_FLAGS"] = "-march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-rtti -fno-exceptions -fdata-sections -ffunction-sections"
		elif self.settings.arch == "armv8_32":
			cmake.definitions["CMAKE_CXX_FLAGS"] = "-march=armv8-m.main -mthumb -fno-rtti -fno-exceptions -fdata-sections -ffunction-sections"

		cmake.configure()
		cmake.build()

	def package(self):
		self.copy("*.h", "include/flow/", "include/flow/")
		self.copy("libFlow.a", "library/", "lib/")

		if(self.settings.build_type == "Debug"):
			self.copy("components.cpp", "source/flow/", "source/flow/")
			self.copy("flow.cpp", "source/flow/", "source/flow/")
			self.copy("reactor.cpp", "source/flow/", "source/flow/")

		if self.settings.arch == "x86" or self.info.settings.arch == "x86_64":
			self.copy("platform_cpputest.cpp", "source/flow/", "source/flow/")
		elif self.settings.arch == "armv6" or self.settings.arch == "armv7" or self.settings.arch == "armv7hf" or self.settings.arch == "armv8_32":
			self.copy("platform_cortexm4.cpp", "source/flow/", "source/flow/")
		else:
			raise Exception("Invalid settings for this package.")

	def package_info(self):
		self.cpp_info.includedirs = ["include/"]
		self.cpp_info.libdirs = ["library/"]
		self.cpp_info.srcdirs = ["source/"]
		self.cpp_info.libs = ["Flow"]
