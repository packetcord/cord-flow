cmake_minimum_required(VERSION 3.16)
project(cord_flow C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------
# DPDK support option
# ---------------------------------------------------------------------
option(ENABLE_DPDK_DATAPLANE "Enable DPDK dataplane support" OFF)
option(ENABLE_XDP_DATAPLANE "Enable AF_XDP dataplane support" OFF)

if(ENABLE_DPDK_DATAPLANE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(DPDK REQUIRED libdpdk)

    if(NOT DPDK_FOUND)
        message(FATAL_ERROR "DPDK not found. Please install DPDK or disable ENABLE_DPDK_DATAPLANE")
    endif()

    message(STATUS "DPDK found: ${DPDK_VERSION}")
    message(STATUS "DPDK include dirs: ${DPDK_INCLUDE_DIRS}")
    message(STATUS "DPDK library dirs: ${DPDK_LIBRARY_DIRS}")
    message(STATUS "DPDK libraries: ${DPDK_LIBRARIES}")
endif()

if(ENABLE_XDP_DATAPLANE)
    find_library(XDP_LIBRARY NAMES xdp REQUIRED)
    find_library(BPF_LIBRARY NAMES bpf REQUIRED)

    if(NOT XDP_LIBRARY OR NOT BPF_LIBRARY)
        message(FATAL_ERROR "libxdp or libbpf not found. Please install libxdp-dev and libbpf-dev or disable ENABLE_XDP_DATAPLANE")
    endif()

    message(STATUS "AF_XDP support enabled")
    message(STATUS "libxdp: ${XDP_LIBRARY}")
    message(STATUS "libbpf: ${BPF_LIBRARY}")
endif()

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
# DPDK linking and compile definitions
# ---------------------------------------------------------------------
if(ENABLE_DPDK_DATAPLANE)
    target_compile_definitions(cord_flow PUBLIC ENABLE_DPDK_DATAPLANE)
    target_compile_options(cord_flow PRIVATE ${DPDK_CFLAGS})
    target_include_directories(cord_flow PRIVATE ${DPDK_INCLUDE_DIRS})
    target_link_directories(cord_flow PRIVATE ${DPDK_LIBRARY_DIRS})
    target_link_libraries(cord_flow PRIVATE ${DPDK_LIBRARIES})
endif()

if(ENABLE_XDP_DATAPLANE)
    target_compile_definitions(cord_flow PUBLIC ENABLE_XDP_DATAPLANE)
    target_link_libraries(cord_flow PRIVATE ${XDP_LIBRARY} ${BPF_LIBRARY})
endif()

# ---------------------------------------------------------------------
# install rules
# ---------------------------------------------------------------------
install(TARGETS cord_flow DESTINATION lib)
install(DIRECTORY include/cord_flow DESTINATION include)
