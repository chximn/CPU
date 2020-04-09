
# This file specifies how the project should be built, using CMake.
# If you are unfamiliar with CMake, don't worry about all the details.
# The sections you might want to edit are marked as such, and
# the comments should hopefully make most of it clear.
#
# For many purposes, you may not need to change anything about this file.

cmake_minimum_required(VERSION 3.8)

# Set project name, version and laguages here. (change as needed)
# Version numbers are available by including "exampleConfig.h" in
# the source. See exampleConfig.h.in for some more details.
project(APP VERSION 0.0.1 LANGUAGES CXX)


# Include stuff. No change needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(ConfigSafeGuards)
include(Colors)


# --------------------------------------------------------------------------------
#                         Locate files (change as needed).
# --------------------------------------------------------------------------------
set(SOURCES
    src/app/registers.cc
)

set(TESTFILES
    tests/main.cc
    tests/registers.test.cc
)

# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------
# Compile all sources into a library. Called engine here (change if you wish).
add_library(engine ${SOURCES})

# Engine needs its header files, and users of the library must also see these (PUBLIC). (No change needed)
target_include_directories(engine
    PUBLIC ${PROJECT_SOURCE_DIR}/src/headers
)

# Add an executable for the file main.cpp, here called app.
# If you add more executables, copy these lines accordingly.
add_executable(app src/main.cc)   # Name of exec. and location of file.
target_link_libraries(app PRIVATE engine) # Link the executable to `engine` (if it uses it).

# Set the compile options you want, possibly depending on compiler (change as needed).
# Do similar for the executables if you wish to set options for them as well.
target_compile_options(engine PRIVATE
    # clang/GCC warnings
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall
    -Wextra # reasonable and standard
    -Wshadow # warn the user if a variable declaration shadows one from a
    # parent context
    -Wnon-virtual-dtor # warn the user if a class with virtual functions has a
    # non-virtual destructor. This helps catch hard to
    # track down memory errors
    -Wold-style-cast # warn for c-style casts
    -Wcast-align # warn for potential performance problem casts
    -Wunused # warn on anything being unused
    -Woverloaded-virtual # warn if you overload (not override) a virtual
    # function
    -Wpedantic # warn if non-standard C++ is used
    -Wconversion # warn on type conversions that may lose data
    -Wsign-conversion # warn on sign conversions
    -Wmisleading-indentation # warn if identation implies blocks where blocks
    # do not exist
    -Wduplicated-cond # warn if if / else chain has duplicated conditions
    -Wduplicated-branches # warn if if / else branches have duplicated code
    -Wlogical-op # warn about logical operations being used where bitwise were
    # probably wanted
    -Wnull-dereference # warn if a null dereference is detected
    -Wuseless-cast # warn if you perform a cast to the same type
    -Wdouble-promotion # warn if float is implicit promoted to double
    -Wformat=2 # warn on security issues around functions that format output
    # (ie printf)
    >
    # MSVC warnings
    $<$<CXX_COMPILER_ID:MSVC>:
    "/Wall" >
    )

# Set the properties you require, e.g. what C++ standard to use (change as needed).
set_target_properties(engine app PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# --------------------------------------------------------------------------------
#                         Make Tests (no change needed).
# --------------------------------------------------------------------------------\
set(TEST_MAIN unit_tests) # Default name for test executable (change if you wish).


# add_subdirectory(external/Catch2)

add_library(Catch2 INTERFACE)
target_include_directories(Catch2 INTERFACE lib/)

add_executable(${TEST_MAIN} ${TESTFILES})
target_link_libraries(${TEST_MAIN} Catch2 engine pthread)
add_custom_target(catch
    COMMAND "${PROJECT_BINARY_DIR}/${TEST_MAIN}"
    DEPENDS ${TEST_MAIN})

include(Catch)
catch_discover_tests(${TEST_MAIN})
