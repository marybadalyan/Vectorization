cmake_minimum_required(VERSION 3.10)
project(Optimization_levels CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We only need ONE executable target. The configuration will change its properties.
add_executable(Vectorization main.cpp)

# Define the output directory for the analysis files
set(ANALYSIS_DIR "${CMAKE_BINARY_DIR}/analysis")
file(MAKE_DIRECTORY ${ANALYSIS_DIR})

if(MSVC)
    target_compile_options(Vectorization PRIVATE /O2 /arch:AVX2)
else()
    target_compile_options(Vectorization PRIVATE -O3 -mavx2)
endif()

if(MSVC)
    add_custom_command(
        TARGET Vectorization POST_BUILD
        # This command runs after the build for the given configuration
        COMMAND cmd /c "dumpbin /DISASM \"$<TARGET_OBJECTS:Vectorization>\" > \"${ANALYSIS_DIR}/assembly.txt\""
        COMMENT "Dumping assembly for $<CONFIG> build to analysis/assembly.txt"
    )
else() # For GCC/Clang
     add_custom_command(
        TARGET Vectorization POST_BUILD
        COMMAND objdump -d -M intel "$<TARGET_FILE:Vectorization>" > "${ANALYSIS_DIR}/assembly.txt"
        COMMENT "Dumping assembly for $<CONFIG> build to analysis/assembly.txt"
    )
endif()