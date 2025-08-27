#include <memory/cord_memory.h>
#include <cord_error.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

/**
 * @file cord_memory.c
 * @brief High-performance memory allocation implementation for CORD-FLOW
 * 
 * Provides optimized memory allocation with huge page support for maximum
 * performance in networking applications.
 */

// =============================================================================
// HUGE PAGE MEMORY ALLOCATION
// =============================================================================

void* cord_huge_pages_malloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    // Calculate real allocation size with metadata page
    size_t real_size = CORD_ALIGN_TO_HUGE_PAGE(size + CORD_HUGE_PAGE_SIZE);
    
    // Attempt huge page allocation first
    void* ptr = mmap(NULL, real_size, 
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB,
                     -1, 0);
    
    if (ptr == MAP_FAILED) {
        // Huge page allocation failed, fallback to regular malloc
        ptr = malloc(real_size);
        if (ptr == NULL) {
            return NULL;
        }
        // Mark as malloc allocation by setting size to 0
        *((size_t*)ptr) = 0;
    } else {
        // Mark as mmap allocation by storing actual size
        *((size_t*)ptr) = real_size;
    }
    
    // Return pointer after metadata page
    return (uint8_t*)ptr + CORD_HUGE_PAGE_SIZE;
}

void cord_huge_pages_free(void* ptr)
{
    if (ptr == NULL) {
        return;
    }
    
    // Get pointer to metadata page
    void* real_ptr = (uint8_t*)ptr - CORD_HUGE_PAGE_SIZE;
    
    // Read allocation metadata
    size_t real_size = *((size_t*)real_ptr);
    
    if (real_size == 0) {
        // Was allocated with malloc, use free
        free(real_ptr);
    } else {
        // Was allocated with mmap, use munmap
        assert(real_size % CORD_HUGE_PAGE_SIZE == 0);
        int result = munmap(real_ptr, real_size);
        if (result != 0) {
            CORD_ERROR("Failed to munmap huge page memory");
        }
    }
}

size_t cord_huge_pages_get_size(const void* ptr)
{
    if (ptr == NULL) {
        return 0;
    }
    
    // Get pointer to metadata page
    const void* real_ptr = (const uint8_t*)ptr - CORD_HUGE_PAGE_SIZE;
    
    // Read stored size
    size_t real_size = *((const size_t*)real_ptr);
    
    if (real_size == 0) {
        // malloc allocation - size not tracked
        return 0;
    }
    
    // Return usable size (minus metadata page)
    return real_size - CORD_HUGE_PAGE_SIZE;
}

bool cord_huge_pages_available(void)
{
    // Try to allocate a minimal huge page to test availability
    void* test_ptr = mmap(NULL, CORD_HUGE_PAGE_SIZE,
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                          -1, 0);
    
    if (test_ptr == MAP_FAILED) {
        return false;
    }
    
    // Clean up test allocation
    munmap(test_ptr, CORD_HUGE_PAGE_SIZE);
    return true;
}

// =============================================================================
// CACHE-ALIGNED MEMORY ALLOCATION  
// =============================================================================

void* cord_cache_aligned_malloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    void* ptr;
    int result = posix_memalign(&ptr, CORD_CACHE_LINE_SIZE, size);
    
    if (result != 0) {
        return NULL;
    }
    
    return ptr;
}

void cord_cache_aligned_free(void* ptr)
{
    if (ptr != NULL) {
        free(ptr);
    }
}