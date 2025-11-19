aPackageInfo = [
	:name = "Ring LibSQL",
	:description = "LibSQL client library for Ring with SQLite compatibility and support for local, remote, and embedded replica databases.",
	:folder = "ring-libsql",
	:developer = "ysdragon",
	:email = "",
	:license = "MIT License",
	:version = "1.0.1",
	:ringversion = "1.24",
	:versions = 	[
		[
			:version = "1.0.1",
			:branch = "master"
		]
	],
	:libs = 	[
		[
			:name = "",
			:version = "",
			:providerusername = ""
		]
	],
	:files = 	[
		".clang-format",
		"CMakeLists.txt",
		"examples/01_local_in_memory.ring",
		"examples/02_local_file.ring",
		"examples/03_prepared_statements.ring",
		"examples/04_data_types.ring",
		"examples/05_remote_database.ring",
		"examples/06_embedded_replica.ring",
		"examples/07_encryption.ring",
		"LICENSE",
		"lib.ring",
		"main.ring",
		"README.md",
		"src/c_src/ring_libsql.c",
		"src/libsql.rh",
		"src/libsql.ring",
		"src/utils/color.ring",
		"src/utils/install.ring",
		"src/utils/uninstall.ring"
	],
	:ringfolderfiles = 	[

	],
	:windowsfiles = 	[
		"lib/windows/amd64/ring_libsql.dll"
	],
	:linuxfiles = 	[
		"lib/linux/amd64/libring_libsql.so",
		"lib/linux/arm64/libring_libsql.so"
	],
	:ubuntufiles = 	[

	],
	:fedorafiles = 	[

	],
	:macosfiles = 	[
		"lib/macos/amd64/libring_libsql.dylib",
		"lib/macos/arm64/libring_libsql.dylib"
	],
	:freebsdfiles = 	[
		"lib/freebsd/amd64/libring_libsql.so"
	],
	:windowsringfolderfiles = 	[

	],
	:linuxringfolderfiles = 	[

	],
	:ubunturingfolderfiles = 	[

	],
	:fedoraringfolderfiles = 	[

	],
	:freebsdringfolderfiles = 	[

	],
	:macosringfolderfiles = 	[

	],
	:run = "ring main.ring",
	:windowsrun = "",
	:linuxrun = "",
	:macosrun = "",
	:ubunturun = "",
	:fedorarun = "",
	:setup = "ring src/utils/install.ring",
	:windowssetup = "",
	:linuxsetup = "",
	:macossetup = "",
	:ubuntusetup = "",
	:fedorasetup = "",
	:remove = "ring src/utils/uninstall.ring",
	:windowsremove = "",
	:linuxremove = "",
	:macosremove = "",
	:ubunturemove = "",
	:fedoraremove = ""
]
