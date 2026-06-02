#include "nchess.h"
#include "memory.h"

void
NCH_Init(){
    // Initialize debug memory tracker only in debug builds
#ifndef NDEBUG
    NCH_MemoryTracker_Init();
#endif
    NCH_InitTables();
    NCH_InitBitboards();
}
