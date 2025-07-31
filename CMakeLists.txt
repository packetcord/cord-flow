cmake_minimum_required(VERSION 3.16)
project(cord_flow C)

set(CMAKE_C_STANDARD 23)

# ---------------------------------------------------------------------
# collect sources and build the static library
# ---------------------------------------------------------------------
file(GLOB_RECURSE SOURCES "src/*.c")
add_library(cord_flow STATIC ${SOURCES})

# ---------------------------------------------------------------------
# public include paths
#   • build tree : <repo>/include/cord_flow
#   • install    : <prefix>/include/cord_flow
# ---------------------------------------------------------------------
target_include_directories(cord_flow
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/cord_flow>
        $<INSTALL_INTERFACE:include/cord_flow>
)

# ---------------------------------------------------------------------
# install rules
# ---------------------------------------------------------------------
install(TARGETS cord_flow DESTINATION lib)
install(DIRECTORY include/cord_flow DESTINATION include)
