# src/CMakeLists.txt

# Set the source files for the project
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

# Include .h files in current directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Collect all sources
set(ALL_SOURCES
    ${SOURCE_FILES}
)

# Add subdirs
add_subdirectory(Core)
add_subdirectory(Game)
add_subdirectory(RenderEngine)

# Main executable
add_executable(worldStream ${ALL_SOURCES})

target_compile_options(worldStream PRIVATE
    -Wall
    -Wextra
    -pedantic
)

target_link_libraries(worldStream PRIVATE third_party)

# Set the output directory for the executable
set_target_properties(worldStream PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Custom target to clean files, build, and run the program
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E remove -f output error                 # Remove output and error files
    COMMAND ${CMAKE_BUILD_TOOL} -j                                      # Run the build with parallel jobs
    COMMAND ./worldStream 2> error                                           # Run the executable and redirect stderr to 'error'
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}                               # Set the working directory to the build directory
    DEPENDS worldStream                                                      # Ensure 'render' is built before running
    COMMENT "Running the worldStream executable..."
    VERBATIM
)
