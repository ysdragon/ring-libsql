cmake_minimum_required(VERSION 3.16)
project(RingLibSQL LANGUAGES C)

include(FetchContent)

set(RING_ROOT_DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
if(DEFINED ENV{RING})
    set(RING_ROOT_DEFAULT "$ENV{RING}")
endif()
set(RING_ROOT "${RING_ROOT_DEFAULT}" CACHE PATH "Path to the Ring project root directory.")
message(STATUS "Using Ring from: ${RING_ROOT}")

if(NOT TARGET Ring::Ring)
    add_library(Ring::Ring UNKNOWN IMPORTED)
    find_path(RING_INCLUDE_DIR ring.h PATHS "${RING_ROOT}/language/include")
    find_library(RING_LIBRARY_PATH NAMES ring PATHS "${RING_ROOT}/lib")

    if(NOT RING_INCLUDE_DIR OR NOT RING_LIBRARY_PATH)
        message(FATAL_ERROR "Could not find Ring headers or library in ${RING_ROOT}. "
                            "Please set the RING_ROOT cache variable correctly.")
    endif()

    set_target_properties(Ring::Ring PROPERTIES
        IMPORTED_LOCATION "${RING_LIBRARY_PATH}"
        INTERFACE_INCLUDE_DIRECTORIES "${RING_INCLUDE_DIR}"
    )
endif()

# Set paths
set(RING_INCLUDE "${RING_ROOT}/language/include")
set(RING_LIB "${RING_ROOT}/lib")
set(RING_BIN "${RING_ROOT}/bin")

# Determine OS and Architecture specific paths
string(TOLOWER "${CMAKE_SYSTEM_NAME}" OS_DIR)
if(OS_DIR STREQUAL "darwin")
    set(OS_DIR "macos")
endif()

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCH_DIR_RAW)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND DEFINED CMAKE_GENERATOR_PLATFORM)
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(ARCH_DIR_RAW "i386")
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        set(ARCH_DIR_RAW "amd64")
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
        set(ARCH_DIR_RAW "arm64")
    else()
        message(WARNING "Unsupported CMAKE_GENERATOR_PLATFORM: ${CMAKE_GENERATOR_PLATFORM}, falling back to CMAKE_SYSTEM_PROCESSOR")
    endif()
endif()

if(NOT DEFINED ARCH_DIR)
    if(ARCH_DIR_RAW MATCHES "x86_64|amd64")
        set(ARCH_DIR "amd64")
    elseif(ARCH_DIR_RAW MATCHES "aarch64|arm64")
        set(ARCH_DIR "arm64")
    elseif(ARCH_DIR_RAW MATCHES "riscv64")
        set(ARCH_DIR "riscv64")
    elseif(ARCH_DIR_RAW MATCHES "i386|i686|x86")
        set(ARCH_DIR "i386")
    else()
        set(ARCH_DIR "${ARCH_DIR_RAW}")
        message(WARNING "Unsupported architecture: ${ARCH_DIR_RAW}. Using as directory name.")
    endif()
endif()

# Set the destination directory for the built library
set(LIB_DEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/${OS_DIR}/${ARCH_DIR}")

# Fetch libsql from GitHub if not already present
set(LIBSQL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/c_src/libsql")
if(NOT EXISTS "${LIBSQL_DIR}/.git")
	message(STATUS "Fetching libsql from GitHub...")
	FetchContent_Declare(
		libsql
		GIT_REPOSITORY https://github.com/tursodatabase/libsql.git
		GIT_TAG        libsql-0.9.29
		GIT_SHALLOW    TRUE
		SOURCE_DIR     "${LIBSQL_DIR}"
	)
	FetchContent_MakeAvailable(libsql)
	message(STATUS "Successfully fetched libsql repository")
else()
	message(STATUS "Found existing libsql repository at ${LIBSQL_DIR}")
endif()

# Set libsql paths
set(LIBSQL_C_BINDINGS_DIR "${LIBSQL_DIR}/bindings/c")
set(LIBSQL_INCLUDE_DIR "${LIBSQL_C_BINDINGS_DIR}/include")
set(LIBSQL_TARGET_DIR "${LIBSQL_DIR}/target/release")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(LIBSQL_STATIC_LIB "${LIBSQL_TARGET_DIR}/sql_experimental.lib")
else()
	set(LIBSQL_STATIC_LIB "${LIBSQL_TARGET_DIR}/libsql_experimental.a")
endif()

# Build libsql using Cargo if not already built
if(NOT EXISTS "${LIBSQL_STATIC_LIB}")
	message(STATUS "Building libsql from Rust sources with optimizations...")
	execute_process(
		COMMAND cargo build --release
		WORKING_DIRECTORY "${LIBSQL_C_BINDINGS_DIR}"
		RESULT_VARIABLE CARGO_BUILD_RESULT
		COMMAND_ECHO STDOUT
	)
	if(NOT CARGO_BUILD_RESULT EQUAL 0)
		message(FATAL_ERROR "Failed to build libsql. Make sure Rust/Cargo is installed.")
	endif()
	
	# Strip the static library to reduce size (Unix only)
	if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
		message(STATUS "Stripping debug symbols from libsql...")
		execute_process(
			COMMAND strip --strip-debug "${LIBSQL_STATIC_LIB}"
			RESULT_VARIABLE STRIP_RESULT
		)
		if(STRIP_RESULT EQUAL 0)
			message(STATUS "Successfully stripped libsql static library")
		else()
			message(WARNING "Failed to strip libsql, but continuing...")
		endif()
	endif()
else()
	message(STATUS "Found libsql static library at ${LIBSQL_STATIC_LIB}")
endif()

# Create the Ring libsql extension shared library
add_library(ring_libsql SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/src/c_src/ring_libsql.c
)

# Include directories
target_include_directories(ring_libsql PRIVATE
	${RING_INCLUDE}
	${LIBSQL_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(ring_libsql PRIVATE
	Ring::Ring
	${LIBSQL_STATIC_LIB}
)

# Add system libraries required by libsql (Rust dependencies)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(ring_libsql PRIVATE
		-lpthread
		-ldl
		-lm
	)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	target_link_libraries(ring_libsql PRIVATE
		"-framework Security"
		"-framework CoreServices"
	)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(ring_libsql PRIVATE
		ws2_32
		userenv
		bcrypt
		ntdll
		crypt32
		secur32
		ncrypt
	)
endif()

target_compile_options(ring_libsql PRIVATE
    $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:
        $<$<C_COMPILER_ID:MSVC>:/O2>
        $<$<NOT:$<C_COMPILER_ID:MSVC>>:-O3>
        -DNDEBUG
    >
)

# Link-time optimization and strip for release builds
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	set_target_properties(ring_libsql PROPERTIES
		INTERPROCEDURAL_OPTIMIZATION TRUE
	)
	# Strip symbols for release builds (platform-specific)
	if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		target_link_options(ring_libsql PRIVATE -Wl,--strip-all)
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		target_link_options(ring_libsql PRIVATE -Wl,-dead_strip)
	endif()
endif()

# Set target properties for output name and prefix
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_target_properties(ring_libsql PROPERTIES
		OUTPUT_NAME "ring_libsql"
	)
endif()

# Move built library to lib directory and strip it
add_custom_command(
	TARGET ring_libsql
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_DEST_DIR}
	COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE:ring_libsql> ${LIB_DEST_DIR}/$<TARGET_FILE_NAME:ring_libsql>
	COMMENT "Moving built library to ${LIB_DEST_DIR}"
	VERBATIM
)

# Additional strip command for the final shared library
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_custom_command(
		TARGET ring_libsql
		POST_BUILD
		COMMAND strip --strip-unneeded ${LIB_DEST_DIR}/$<TARGET_FILE_NAME:ring_libsql> || true
		COMMENT "Stripping final shared library"
		VERBATIM
	)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	add_custom_command(
		TARGET ring_libsql
		POST_BUILD
		COMMAND strip -x ${LIB_DEST_DIR}/$<TARGET_FILE_NAME:ring_libsql> || true
		COMMENT "Stripping final shared library"
		VERBATIM
	)
endif()

# Copy to bin directory on Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_custom_command(
		TARGET ring_libsql
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${RING_BIN}
		COMMAND ${CMAKE_COMMAND} -E copy ${LIB_DEST_DIR}/$<TARGET_FILE_NAME:ring_libsql> ${RING_BIN}/$<TARGET_FILE_NAME:ring_libsql>
		COMMENT "Copying built library to bin directory"
		VERBATIM
	)
endif()

install(TARGETS ring_libsql
    LIBRARY DESTINATION "${RING_ROOT}/lib"
)

message(STATUS "Ring LibSQL Extension Configuration:")
message(STATUS "  - Ring Include Dir: ${RING_INCLUDE_DIR}")
message(STATUS "  - Ring Library Path: ${RING_LIBRARY_PATH}")