/*
    memory.h

    Debug/production memory allocation macros and tracker API.
*/

#ifndef NCHESS_SRC_MEMORY_H
#define NCHESS_SRC_MEMORY_H

#include <stddef.h>

// Public tracker API (no-op in release builds)
void NCH_MemoryTracker_Init(void);
void NCH_MemoryTracker_DumpLeaks(void);
void NCH_MemoryTracker_Shutdown(void);
void NCH_MemoryTracker_EnableFailOnLeaks(int enable);
size_t NCH_MemoryTracker_GetOutstandingBytes(void);

#ifdef NDEBUG
    // Production: map macros to standard allocators
    #include <stdlib.h>
    #define NCH_MALLOC(sz)            malloc((sz))
    #define NCH_CALLOC(n, sz)         calloc((n), (sz))
    #define NCH_REALLOC(ptr, sz)      realloc((ptr), (sz))
    #define NCH_FREE(ptr)             free((ptr))
#else
    // Debug: route to tracking allocators with source info
    void* nch_dbg_malloc(size_t size, const char* file, int line, const char* func);
    void* nch_dbg_calloc(size_t count, size_t size, const char* file, int line, const char* func);
    void* nch_dbg_realloc(void* ptr, size_t size, const char* file, int line, const char* func);
    void  nch_dbg_free(void* ptr, const char* file, int line, const char* func);

    #define NCH_MALLOC(sz)            nch_dbg_malloc((sz), __FILE__, __LINE__, __func__)
    #define NCH_CALLOC(n, sz)         nch_dbg_calloc((n), (sz), __FILE__, __LINE__, __func__)
    #define NCH_REALLOC(ptr, sz)      nch_dbg_realloc((ptr), (sz), __FILE__, __LINE__, __func__)
    #define NCH_FREE(ptr)             nch_dbg_free((ptr), __FILE__, __LINE__, __func__)
#endif

#endif // NCHESS_SRC_MEMORY_H
