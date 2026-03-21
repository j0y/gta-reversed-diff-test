// test_CStreaming_Load.cpp — Diagnostic tests for CStreaming::LoadAllRequestedModels.
// Purpose: Pinpoint exactly where/why LoadAllRequestedModels crashes in headless mode.
// Tests go from safest to most dangerous, so the last PASS tells you how far the
// pipeline works.

#include "StdInc.h"
#include "TestFramework.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "ModelInfo.h"
#include "TxdStore.h"
#include "CdStreamInfo.h"

// ResumeOtherThreads is defined in TestFramework.h

// Helper: log to C:\streaming_diag.log for post-mortem
static void DiagLog(const char* fmt, ...) {
    FILE* f = fopen("C:\\streaming_diag.log", "a");
    if (!f) return;
    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);
    fprintf(f, "\n");
    fflush(f);
    fclose(f);
}

// ---------------------------------------------------------------------------
// Test 1: Verify streaming infrastructure state at state 9
// ---------------------------------------------------------------------------
GAME_TEST(CStreamingLoad, Diag_InfraState) {
    DiagLog("=== CStreamingLoad Diag: InfraState ===");

    bool isInit = CStreaming::ms_bIsInitialised;
    DiagLog("ms_bIsInitialised = %d", (int)isInit);
    EXPECT_TRUE(isInit);

    DiagLog("ms_memoryAvailable = %u", (unsigned)CStreaming::ms_memoryAvailable);
    DiagLog("ms_memoryUsedBytes = %u", (unsigned)CStreaming::ms_memoryUsedBytes);
    DiagLog("ms_streamingBufferSize = %u sectors", (unsigned)CStreaming::ms_streamingBufferSize);

    // Check channels are idle
    bool ch0idle = CStreaming::ms_channel[0].IsIdle();
    bool ch1idle = CStreaming::ms_channel[1].IsIdle();
    DiagLog("channel[0].IsIdle = %d", (int)ch0idle);
    DiagLog("channel[1].IsIdle = %d", (int)ch1idle);
    EXPECT_TRUE(ch0idle);
    EXPECT_TRUE(ch1idle);

    // Check request list
    bool reqEmpty = CStreaming::IsRequestListEmpty();
    DiagLog("IsRequestListEmpty = %d", (int)reqEmpty);
    EXPECT_TRUE(reqEmpty);

    // Check streaming buffer exists
    DiagLog("ms_pStreamingBuffer[0] = %p", CStreaming::ms_pStreamingBuffer[0]);
    DiagLog("ms_pStreamingBuffer[1] = %p", CStreaming::ms_pStreamingBuffer[1]);
    EXPECT_TRUE(CStreaming::ms_pStreamingBuffer[0] != nullptr);

    // Check CdStream state
    DiagLog("gStreamingInitialized = %d", gStreamingInitialized);
    DiagLog("gOpenStreamCount = %d", gOpenStreamCount);
    // Check actual file handles
    for (int i = 0; i < 8; i++) {
        HANDLE h = gStreamFileHandles[i];
        if (h && h != INVALID_HANDLE_VALUE) {
            DiagLog("  gStreamFileHandles[%d] = %p (%s)", i, h, gCdImageNames[i]);
        }
    }

    // Memory headroom
    int64 headroom = (int64)CStreaming::ms_memoryAvailable - (int64)CStreaming::ms_memoryUsedBytes;
    DiagLog("memory headroom = %lld bytes", headroom);
    EXPECT_GT(headroom, 0);

    DiagLog("--- InfraState DONE ---");
}

// ---------------------------------------------------------------------------
// Test 2: Find an unloaded model with a loaded TXD (safest load target)
// ---------------------------------------------------------------------------
static int32 FindUnloadedModelWithLoadedTxd() {
    // Search vehicle models 400-611
    for (int32 id = 400; id <= 611; id++) {
        auto& info = CStreaming::GetInfo(id);
        if (info.m_LoadState != LOADSTATE_NOT_LOADED) continue;
        if (info.GetCdSize() == 0) continue; // not in any IMG

        CBaseModelInfo* mi = CModelInfo::GetModelInfo(id);
        if (!mi) continue;

        int16 txdIdx = mi->m_nTxdIndex;
        if (txdIdx < 0) continue;

        // Check if TXD is loaded
        auto* txdDef = CTxdStore::ms_pTxdPool->GetAt(txdIdx);
        if (txdDef && txdDef->m_pRwDictionary) {
            return id; // TXD is loaded, model is not — good candidate
        }
    }
    // Try ped models 0-312
    for (int32 id = 0; id <= 312; id++) {
        auto& info = CStreaming::GetInfo(id);
        if (info.m_LoadState != LOADSTATE_NOT_LOADED) continue;
        if (info.GetCdSize() == 0) continue;

        CBaseModelInfo* mi = CModelInfo::GetModelInfo(id);
        if (!mi) continue;

        int16 txdIdx = mi->m_nTxdIndex;
        if (txdIdx < 0) continue;

        auto* txdDef = CTxdStore::ms_pTxdPool->GetAt(txdIdx);
        if (txdDef && txdDef->m_pRwDictionary) {
            return id;
        }
    }
    return -1;
}

// Find any unloaded DFF model (TXD may or may not be loaded)
static int32 FindAnyUnloadedDffModel() {
    for (int32 id = 400; id <= 611; id++) {
        auto& info = CStreaming::GetInfo(id);
        if (info.m_LoadState != LOADSTATE_NOT_LOADED) continue;
        if (info.GetCdSize() == 0) continue;
        CBaseModelInfo* mi = CModelInfo::GetModelInfo(id);
        if (mi) return id;
    }
    return -1;
}

GAME_TEST(CStreamingLoad, Diag_FindCandidates) {
    DiagLog("=== CStreamingLoad Diag: FindCandidates ===");

    // Count loaded models
    int loadedVeh = 0, unloadedVeh = 0, loadedPed = 0;
    for (int32 id = 400; id <= 611; id++) {
        if (CStreaming::IsModelLoaded(id)) loadedVeh++;
        else {
            auto& info = CStreaming::GetInfo(id);
            if (info.GetCdSize() > 0) unloadedVeh++;
        }
    }
    DiagLog("Vehicles: %d loaded, %d unloaded (with CD data)", loadedVeh, unloadedVeh);

    for (int32 id = 0; id <= 312; id++) {
        if (CStreaming::IsModelLoaded(id)) loadedPed++;
    }
    DiagLog("Peds: %d loaded", loadedPed);

    int32 bestCandidate = FindUnloadedModelWithLoadedTxd();
    if (bestCandidate >= 0) {
        CBaseModelInfo* mi = CModelInfo::GetModelInfo(bestCandidate);
        auto& info = CStreaming::GetInfo(bestCandidate);
        DiagLog("Best candidate (TXD loaded): model %d, txd=%d, cdSize=%u sectors",
            bestCandidate, (int)mi->m_nTxdIndex, (unsigned)info.GetCdSize());
    } else {
        DiagLog("No unloaded model with loaded TXD found");
    }

    int32 anyCandidate = FindAnyUnloadedDffModel();
    if (anyCandidate >= 0) {
        CBaseModelInfo* mi = CModelInfo::GetModelInfo(anyCandidate);
        auto& info = CStreaming::GetInfo(anyCandidate);
        int16 txdIdx = mi->m_nTxdIndex;
        bool txdLoaded = false;
        if (txdIdx >= 0) {
            auto* txd = CTxdStore::ms_pTxdPool->GetAt(txdIdx);
            txdLoaded = (txd && txd->m_pRwDictionary);
        }
        DiagLog("Any unloaded DFF: model %d, txd=%d (loaded=%d), cdSize=%u",
            anyCandidate, (int)txdIdx, (int)txdLoaded, (unsigned)info.GetCdSize());
    } else {
        DiagLog("No unloaded DFF models at all!");
    }

    EXPECT_TRUE(anyCandidate >= 0); // We need at least one to test with
    DiagLog("--- FindCandidates DONE ---");
}

// ---------------------------------------------------------------------------
// Test 3: RequestModel only (no load) — safe, just queues the request
// ---------------------------------------------------------------------------
GAME_TEST(CStreamingLoad, Diag_RequestOnly) {
    DiagLog("=== CStreamingLoad Diag: RequestOnly ===");

    int32 modelId = FindAnyUnloadedDffModel();
    if (modelId < 0) {
        DiagLog("SKIP: no unloaded model");
        EXPECT_TRUE(false); // flag as failure
        return;
    }

    DiagLog("Requesting model %d ...", modelId);
    auto& info = CStreaming::GetInfo(modelId);
    auto stateBefore = info.m_LoadState;
    DiagLog("  state before: %d", (int)stateBefore);

    CStreaming::RequestModel(modelId, STREAMING_KEEP_IN_MEMORY);

    DiagLog("  state after: %d", (int)info.m_LoadState);
    DiagLog("  IsRequestListEmpty: %d", (int)CStreaming::IsRequestListEmpty());
    DiagLog("  ms_numModelsRequested: %d", CStreaming::ms_numModelsRequested);

    EXPECT_EQ((int)info.m_LoadState, (int)LOADSTATE_REQUESTED);

    // Clean up: remove request without loading
    CStreaming::RemoveModel(modelId);
    DiagLog("  state after remove: %d", (int)info.m_LoadState);
    EXPECT_EQ((int)info.m_LoadState, (int)LOADSTATE_NOT_LOADED);

    DiagLog("--- RequestOnly DONE ---");
}

// ---------------------------------------------------------------------------
// Test 4: FlushChannels on idle channels — should be a no-op
// ---------------------------------------------------------------------------
GAME_TEST(CStreamingLoad, Diag_FlushIdle) {
    DiagLog("=== CStreamingLoad Diag: FlushIdle ===");
    DiagLog("Calling FlushChannels with idle channels...");
    CStreaming::FlushChannels();
    DiagLog("FlushChannels returned OK");
    EXPECT_TRUE(true);
    DiagLog("--- FlushIdle DONE ---");
}

// ---------------------------------------------------------------------------
// Test 5: CdStreamSync on idle channel — should return immediately
// ---------------------------------------------------------------------------
GAME_TEST(CStreamingLoad, Diag_CdStreamSyncIdle) {
    DiagLog("=== CStreamingLoad Diag: CdStreamSyncIdle ===");
    // Channel 0 should be idle at state 9
    auto status = CdStreamGetStatus(0);
    DiagLog("CdStreamGetStatus(0) = %d", (int)status);
    EXPECT_EQ((int)status, (int)eCdStreamStatus::READING_SUCCESS);
    DiagLog("--- CdStreamSyncIdle DONE ---");
}

// ---------------------------------------------------------------------------
// Test 6: MakeSpaceFor with 0 bytes — should be no-op if headroom exists
// ---------------------------------------------------------------------------
GAME_TEST(CStreamingLoad, Diag_MakeSpaceZero) {
    DiagLog("=== CStreamingLoad Diag: MakeSpaceZero ===");
    DiagLog("ms_memoryUsedBytes=%u, ms_memoryAvailable=%u",
        (unsigned)CStreaming::ms_memoryUsedBytes,
        (unsigned)CStreaming::ms_memoryAvailable);
    if (CStreaming::ms_memoryUsedBytes < CStreaming::ms_memoryAvailable) {
        DiagLog("Calling MakeSpaceFor(0)...");
        CStreaming::MakeSpaceFor(0);
        DiagLog("MakeSpaceFor(0) returned OK");
        EXPECT_TRUE(true);
    } else {
        DiagLog("SKIP: no memory headroom — MakeSpaceFor(0) would trigger cleanup");
    }
    DiagLog("--- MakeSpaceZero DONE ---");
}

// ---------------------------------------------------------------------------
// Test 7: MakeSpaceFor with a small realistic size
// ---------------------------------------------------------------------------
GAME_TEST(CStreamingLoad, Diag_MakeSpaceSmall) {
    DiagLog("=== CStreamingLoad Diag: MakeSpaceSmall ===");
    // A typical vehicle DFF is ~50-200KB
    size_t needed = 200 * 1024; // 200KB
    int64 headroom = (int64)CStreaming::ms_memoryAvailable - (int64)CStreaming::ms_memoryUsedBytes;
    DiagLog("headroom=%lld, needed=%u", headroom, (unsigned)needed);
    if (headroom > (int64)needed) {
        DiagLog("Calling MakeSpaceFor(%u)...", (unsigned)needed);
        CStreaming::MakeSpaceFor(needed);
        DiagLog("MakeSpaceFor returned OK");
        EXPECT_TRUE(true);
    } else {
        DiagLog("headroom too small — MakeSpaceFor would trigger cleanup chain");
        DiagLog("This is the likely crash path: MakeSpaceFor -> RemoveLeastUsedModel -> DeleteRwObjectsBehindCamera");
        // Try anyway to see what happens
        DiagLog("Attempting MakeSpaceFor(%u) despite low headroom...", (unsigned)needed);
        CStreaming::MakeSpaceFor(needed);
        DiagLog("MakeSpaceFor survived!");
        EXPECT_TRUE(true);
    }
    DiagLog("--- MakeSpaceSmall DONE ---");
}

// ---------------------------------------------------------------------------
// Test 8: LoadAllRequestedModels — needs CdStream thread alive!
// The test runner suspends all threads (SuspendOtherThreads), so we must
// resume them before calling any streaming I/O function.
// ---------------------------------------------------------------------------
GAME_TEST(CStreamingLoad, Diag_LoadEmpty) {
    DiagLog("=== CStreamingLoad Diag: LoadEmpty ===");

    // First drain any pending requests so we test with a clean slate
    DiagLog("Request list empty: %d, channels idle: %d/%d",
        (int)CStreaming::IsRequestListEmpty(),
        (int)CStreaming::ms_channel[0].IsIdle(),
        (int)CStreaming::ms_channel[1].IsIdle());

    // Resume threads so CdStream worker can complete pending I/O
    {
        ResumeOtherThreads resumeGuard;
        DiagLog("Threads resumed. Calling FlushChannels to drain pending work...");
        CStreaming::FlushChannels();
        DiagLog("FlushChannels done. Calling LoadAllRequestedModels(false)...");
        CStreaming::LoadAllRequestedModels(false);
        DiagLog("LoadAllRequestedModels returned OK");
    }
    // Threads re-suspended here

    DiagLog("Request list empty after: %d", (int)CStreaming::IsRequestListEmpty());
    EXPECT_TRUE(true);
    DiagLog("--- LoadEmpty DONE ---");
}

// ---------------------------------------------------------------------------
// Test 9: THE BIG ONE — Request + Load a single model
// ---------------------------------------------------------------------------
GAME_TEST(CStreamingLoad, Diag_RequestAndLoad) {
    DiagLog("=== CStreamingLoad Diag: RequestAndLoad ===");

    // Prefer a model whose TXD is already loaded
    int32 modelId = FindUnloadedModelWithLoadedTxd();
    if (modelId < 0) {
        modelId = FindAnyUnloadedDffModel();
        DiagLog("No model with loaded TXD — falling back to model %d (TXD will be loaded too)", modelId);
    }
    if (modelId < 0) {
        DiagLog("SKIP: no unloaded models");
        EXPECT_TRUE(false);
        return;
    }

    CBaseModelInfo* mi = CModelInfo::GetModelInfo(modelId);
    auto& info = CStreaming::GetInfo(modelId);
    DiagLog("Target: model %d, txd=%d, cdSize=%u, animFile=%d",
        modelId, (int)mi->m_nTxdIndex, (unsigned)info.GetCdSize(), mi->GetAnimFileIndex());

    auto memBefore = CStreaming::ms_memoryUsedBytes;
    DiagLog("Memory before: used=%u, available=%u", (unsigned)memBefore, (unsigned)CStreaming::ms_memoryAvailable);

    // Must resume threads — CdStream worker needed for I/O
    ResumeOtherThreads resumeGuard;

    // First drain any pending work from init
    DiagLog("Draining pending requests with FlushChannels...");
    CStreaming::FlushChannels();
    DiagLog("FlushChannels done");

    // Step A: Request
    DiagLog("Step A: RequestModel(%d, KEEP_IN_MEMORY)...", modelId);
    CStreaming::RequestModel(modelId, STREAMING_KEEP_IN_MEMORY);
    DiagLog("  loadState=%d, requestListEmpty=%d, numRequested=%d",
        (int)info.m_LoadState, (int)CStreaming::IsRequestListEmpty(), CStreaming::ms_numModelsRequested);

    // Step B: Load
    DiagLog("Step B: LoadAllRequestedModels(false)...");
    CStreaming::LoadAllRequestedModels(false);
    DiagLog("Step B returned!");

    DiagLog("  loadState=%d", (int)info.m_LoadState);
    DiagLog("  Memory after: used=%u", (unsigned)CStreaming::ms_memoryUsedBytes);

    bool loaded = CStreaming::IsModelLoaded(modelId);
    DiagLog("  IsModelLoaded(%d) = %d", modelId, (int)loaded);
    EXPECT_TRUE(loaded);

    if (loaded) {
        DiagLog("SUCCESS: Model %d loaded in headless mode!", modelId);
        DiagLog("  mi->m_pRwObject = %p", mi->m_pRwObject);

        // Clean up
        CStreaming::RemoveModel(modelId);
        DiagLog("  Cleaned up model %d", modelId);
    }

    DiagLog("--- RequestAndLoad DONE ---");
}

// ---------------------------------------------------------------------------
// Test 10: Request + Load a TXD explicitly (simpler than DFF)
// ---------------------------------------------------------------------------
GAME_TEST(CStreamingLoad, Diag_LoadTxd) {
    DiagLog("=== CStreamingLoad Diag: LoadTxd ===");

    // Find an unloaded TXD
    int32 txdModelId = -1;
    for (int32 id = 400; id <= 611; id++) {
        CBaseModelInfo* mi = CModelInfo::GetModelInfo(id);
        if (!mi) continue;
        int16 txdIdx = mi->m_nTxdIndex;
        if (txdIdx < 0) continue;
        auto* txdDef = CTxdStore::ms_pTxdPool->GetAt(txdIdx);
        if (!txdDef || txdDef->m_pRwDictionary) continue; // already loaded or missing
        // Found unloaded TXD
        int32 candidate = TXDToModelId(txdIdx);
        auto& tInfo = CStreaming::GetInfo(candidate);
        if (tInfo.GetCdSize() > 0) {
            txdModelId = candidate;
            DiagLog("Found unloaded TXD: modelId=%d (txdIdx=%d, for vehicle %d), cdSize=%u",
                txdModelId, (int)txdIdx, id, (unsigned)tInfo.GetCdSize());
            break;
        }
    }

    if (txdModelId < 0) {
        DiagLog("All vehicle TXDs already loaded — SKIP");
        EXPECT_TRUE(true);
        return;
    }

    // Must resume threads for CdStream I/O
    ResumeOtherThreads resumeGuard;

    // Drain pending work first
    CStreaming::FlushChannels();

    DiagLog("Requesting TXD model %d...", txdModelId);
    CStreaming::RequestModel(txdModelId, STREAMING_KEEP_IN_MEMORY);

    DiagLog("LoadAllRequestedModels...");
    CStreaming::LoadAllRequestedModels(false);
    DiagLog("Returned from LoadAllRequestedModels");

    bool loaded = CStreaming::IsModelLoaded(txdModelId);
    DiagLog("TXD loaded = %d", (int)loaded);
    EXPECT_TRUE(loaded);

    if (loaded) {
        CStreaming::RemoveModel(txdModelId);
        DiagLog("Cleaned up TXD");
    }

    DiagLog("--- LoadTxd DONE ---");
}
