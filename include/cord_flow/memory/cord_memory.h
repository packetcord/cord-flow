#ifndef CORD_MEMORY_H
#define CORD_MEMORY_H

#include <cord_type.h>
#include <stddef.h>

/**
 * @file cord_memory.h
 * @brief High-performance memory allocation functions for CORD-FLOW
 * 
 * This module provides optimized memory allocation with support for:
 * - Huge page allocation for high-performance networking
 * - Aligned memory buffers for cache efficiency  
 * - Zero-copy memory management
 * - Fallback to standard allocation when huge pages unavailable
 */

// Memory alignment and huge page constants
#define CORD_HUGE_PAGE_2MB       (2 * 1024 * 1024)
#define CORD_HUGE_PAGE_1GB       (1024 * 1024 * 1024)
#define CORD_HUGE_PAGE_SIZE      CORD_HUGE_PAGE_2MB

#define CORD_CACHE_LINE_SIZE     64
#define CORD_PAGE_SIZE           4096

// Memory alignment macros
#define CORD_ALIGN_TO_HUGE_PAGE(x) \
    (((x) + CORD_HUGE_PAGE_SIZE - 1) / CORD_HUGE_PAGE_SIZE * CORD_HUGE_PAGE_SIZE)

#define CORD_ALIGN_TO_CACHE_LINE(x) \
    (((x) + CORD_CACHE_LINE_SIZE - 1) / CORD_CACHE_LINE_SIZE * CORD_CACHE_LINE_SIZE)

#define CORD_ALIGN_TO_PAGE(x) \
    (((x) + CORD_PAGE_SIZE - 1) / CORD_PAGE_SIZE * CORD_PAGE_SIZE)

// Cache-aligned buffer macro
#define CORD_BUFFER(name, size) uint8_t name[size] __attribute__ ((aligned (CORD_CACHE_LINE_SIZE)))

// =============================================================================
// HIGH-PERFORMANCE MEMORY ALLOCATION FUNCTIONS
// =============================================================================

/**
 * @brief Allocate memory using huge pages for maximum performance
 * 
 * Attempts to allocate memory using huge pages for optimal performance in
 * high-throughput networking applications. Falls back to standard malloc
 * if huge pages are not available.
 * 
 * @param size Size in bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 * 
 * @note The returned memory is automatically aligned to huge page boundaries
 * @note Always pair with cord_huge_pages_free() for proper cleanup
 */
void* cord_huge_pages_malloc(size_t size);

/**
 * @brief Free memory allocated with cord_huge_pages_malloc
 * 
 * Properly deallocates memory that was allocated with huge pages,
 * automatically detecting whether it was allocated via mmap() or malloc().
 * 
 * @param ptr Pointer to memory allocated with cord_huge_pages_malloc()
 * 
 * @note Safe to call with NULL pointer
 */
void cord_huge_pages_free(void* ptr);

/**
 * @brief Allocate cache-aligned memory for optimal performance
 * 
 * Allocates memory aligned to cache line boundaries to prevent false
 * sharing and improve cache performance in multi-threaded applications.
 * 
 * @param size Size in bytes to allocate
 * @return Pointer to cache-aligned memory, or NULL on failure
 */
void* cord_cache_aligned_malloc(size_t size);

/**
 * @brief Free cache-aligned memory
 * 
 * @param ptr Pointer to memory allocated with cord_cache_aligned_malloc()
 */
void cord_cache_aligned_free(void* ptr);

/**
 * @brief Get the actual allocation size for huge page memory
 * 
 * Returns the actual number of bytes allocated for a huge page allocation,
 * which may be larger than the requested size due to page alignment.
 * 
 * @param ptr Pointer to memory allocated with cord_huge_pages_malloc()
 * @return Actual allocated size in bytes, or 0 if invalid pointer
 */
size_t cord_huge_pages_get_size(const void* ptr);

/**
 * @brief Check if huge pages are available on the system
 * 
 * @return true if huge pages are available, false otherwise
 */
bool cord_huge_pages_available(void);

#endif // CORD_MEMORY_H
