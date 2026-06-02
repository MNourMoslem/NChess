/*
    memory.c

    Debug memory tracker and allocator implementations.
    In release builds (NDEBUG defined), this file provides no-op stubs.
*/

#include "memory.h"

#ifndef NDEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct MemRecord {
    void* ptr;
    size_t size;
    const char* file;
    const char* func;
    int line;
    struct MemRecord* next;
} MemRecord;

static MemRecord* g_head = NULL;
static size_t g_total_allocs = 0;
static size_t g_total_frees = 0;
static size_t g_bytes_outstanding = 0;
static int g_fail_on_leaks = 0;

static void tracker_add(void* ptr, size_t size, const char* file, int line, const char* func) {
    MemRecord* rec = (MemRecord*)malloc(sizeof(MemRecord));
    if (!rec) return; // best-effort
    rec->ptr = ptr;
    rec->size = size;
    rec->file = file;
    rec->func = func;
    rec->line = line;
    rec->next = g_head;
    g_head = rec;
    g_total_allocs++;
    g_bytes_outstanding += size;
}

static MemRecord* tracker_find(void* ptr, MemRecord** prev_out) {
    MemRecord* prev = NULL;
    MemRecord* cur = g_head;
    while (cur) {
        if (cur->ptr == ptr) {
            if (prev_out) *prev_out = prev;
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }
    return NULL;
}

static int tracker_remove(void* ptr) {
    MemRecord* prev = NULL;
    MemRecord* rec = tracker_find(ptr, &prev);
    if (!rec) {
        fprintf(stderr, "[NCH DBG] free of unknown ptr=%p\n", ptr);
        return 0;
    }
    if (prev) prev->next = rec->next; else g_head = rec->next;
    g_total_frees++;
    if (g_bytes_outstanding >= rec->size) g_bytes_outstanding -= rec->size; else g_bytes_outstanding = 0;
    free(rec);
    return 1;
}

void NCH_MemoryTracker_Init(void) {
    g_head = NULL;
    g_total_allocs = 0;
    g_total_frees = 0;
    g_bytes_outstanding = 0;
    const char* env = getenv("NCH_FAIL_ON_LEAKS");
    g_fail_on_leaks = (env && env[0] != '\0' && env[0] != '0') ? 1 : 0;
    atexit(NCH_MemoryTracker_Shutdown);
}

void NCH_MemoryTracker_DumpLeaks(void) {
    if (!g_head) {
        fprintf(stderr, "[NCH DBG] No memory leaks detected. allocs=%zu frees=%zu outstanding=%zu bytes\n",
                g_total_allocs, g_total_frees, g_bytes_outstanding);
        return;
    }
    fprintf(stderr, "[NCH DBG] Leaks detected! allocs=%zu frees=%zu outstanding=%zu bytes\n",
            g_total_allocs, g_total_frees, g_bytes_outstanding);
    MemRecord* cur = g_head;
    while (cur) {
        fprintf(stderr, "  leak ptr=%p size=%zu at %s:%d (%s)\n",
                cur->ptr, cur->size, cur->file, cur->line, cur->func);
        cur = cur->next;
    }
}

void NCH_MemoryTracker_Shutdown(void) {
    NCH_MemoryTracker_DumpLeaks();
}

void NCH_MemoryTracker_EnableFailOnLeaks(int enable) {
    g_fail_on_leaks = enable ? 1 : 0;
}

size_t NCH_MemoryTracker_GetOutstandingBytes(void) {
    return g_bytes_outstanding;
}

void* nch_dbg_malloc(size_t size, const char* file, int line, const char* func) {
    void* p = malloc(size);
    if (p) tracker_add(p, size, file, line, func);
    return p;
}

void* nch_dbg_calloc(size_t count, size_t size, const char* file, int line, const char* func) {
    void* p = calloc(count, size);
    if (p) tracker_add(p, count * size, file, line, func);
    return p;
}

void* nch_dbg_realloc(void* ptr, size_t size, const char* file, int line, const char* func) {
    // Remove old record first (size changes); if not tracked, treat as fresh alloc
    if (ptr) (void)tracker_remove(ptr);
    void* np = realloc(ptr, size);
    if (np) tracker_add(np, size, file, line, func);
    return np;
}

void nch_dbg_free(void* ptr, const char* file, int line, const char* func) {
    (void)file; (void)line; (void)func;
    if (!ptr) return;
    if (tracker_remove(ptr)) {
        free(ptr);
    }
}

#else // NDEBUG

// Release stubs
void NCH_MemoryTracker_Init(void) {}
void NCH_MemoryTracker_DumpLeaks(void) {}

#endif // NDEBUG
