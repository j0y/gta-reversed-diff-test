// test_CWanted_PursuitPointers.cpp — Tests for m_pCopsInPursuit pointer validity.
//
// Reported bug: during pursuit, m_pCopsInPursuit entries become dangling pointers
// (0xCDCDCDCD pattern = freed MSVC debug heap memory), causing crashes when
// IsClosestCop/ComputePursuitCopToDisplace dereference them.
//
// Also tests CanCopJoinPursuit (method) which copies the array to a local but
// passes m_nCopsInPursuit by reference — potential count/array desync.

#include "StdInc.h"
#include "TestFramework.h"
#include "Wanted.h"
#include "Entity/Ped/CopPed.h"
#include "Pools/Pools.h"
#include "World.h"
#include "Streaming.h"

// Check if a CCopPed* looks like a valid, live ped in the pool.
static bool IsCopPtrValid(CCopPed* cop) {
    if (!cop) return true; // nullptr is valid (empty slot)

    // Check for common MSVC debug heap fill patterns (dangling pointer)
    auto addr = reinterpret_cast<uintptr_t>(cop);
    if ((addr & 0xFFFFFF00) == 0xCDCDCD00) return false; // uninitialized heap
    if ((addr & 0xFFFFFF00) == 0xDDDDDD00) return false; // freed heap
    if ((addr & 0xFFFFFF00) == 0xFEEEFE00) return false; // freed heap (HeapFree)
    if ((addr & 0xFFFFFF00) == 0xBAADF000) return false; // LocalAlloc uninitialized

    // Check against ped pool
    auto* pool = GetPedPool();
    return pool->IsObjectValid(reinterpret_cast<CPed*>(cop));
}

// --- Validate current pursuit array ---

GAME_TEST(CWanted_Pursuit, PointersAreValid) {
    auto* wanted = FindPlayerWanted();
    if (!wanted) return;

    int invalidCount = 0;
    for (uint32 i = 0; i < CWanted::MAX_COPS_IN_PURSUIT; i++) {
        if (!IsCopPtrValid(wanted->m_pCopsInPursuit[i])) {
            invalidCount++;
        }
    }

    if (invalidCount > 0) {
        char msg[128];
        _snprintf(msg, sizeof(msg),
            "m_pCopsInPursuit has %d dangling pointer(s) out of %u slots",
            invalidCount, CWanted::MAX_COPS_IN_PURSUIT);
        GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
    }
    EXPECT_EQ(invalidCount, 0);
}

GAME_TEST(CWanted_Pursuit, CountMatchesArray) {
    auto* wanted = FindPlayerWanted();
    if (!wanted) return;

    int actualCount = 0;
    for (uint32 i = 0; i < CWanted::MAX_COPS_IN_PURSUIT; i++) {
        if (wanted->m_pCopsInPursuit[i] != nullptr) {
            actualCount++;
        }
    }

    EXPECT_EQ((int)wanted->m_nCopsInPursuit, actualCount);
}

// --- Synthetic: add cop to pursuit, delete cop, check for stale pointer ---

GAME_TEST(CWanted_Pursuit, DanglingAfterDelete) {
    auto* wanted = FindPlayerWanted();
    if (!wanted) return;

    // Save pursuit state
    uint8 savedCount = wanted->m_nCopsInPursuit;
    uint8 savedMax = wanted->m_nMaxCopsInPursuit;
    CCopPed* savedCops[CWanted::MAX_COPS_IN_PURSUIT];
    memcpy(savedCops, wanted->m_pCopsInPursuit, sizeof(savedCops));

    // Clear pursuit for test
    wanted->m_nCopsInPursuit = 0;
    wanted->m_nMaxCopsInPursuit = 3;
    memset(wanted->m_pCopsInPursuit, 0, sizeof(wanted->m_pCopsInPursuit));

    // Create a cop ped
    auto copModel = CStreaming::GetDefaultCopModel();
    if (!CStreaming::IsModelLoaded((int32)copModel)) {
        // Restore and skip
        wanted->m_nCopsInPursuit = savedCount;
        wanted->m_nMaxCopsInPursuit = savedMax;
        memcpy(wanted->m_pCopsInPursuit, savedCops, sizeof(savedCops));
        return;
    }

    auto* cop = new CCopPed(COP_TYPE_CITYCOP);
    if (!cop) {
        wanted->m_nCopsInPursuit = savedCount;
        wanted->m_nMaxCopsInPursuit = savedMax;
        memcpy(wanted->m_pCopsInPursuit, savedCops, sizeof(savedCops));
        return;
    }

    CVector playerPos = FindPlayerPed()->GetPosition();
    cop->SetPosn(playerPos + CVector(5.0f, 0.0f, 0.0f));
    CWorld::Add(cop);

    // Manually add to pursuit array
    wanted->m_pCopsInPursuit[0] = cop;
    wanted->m_nCopsInPursuit = 1;

    // Verify it's there
    EXPECT_TRUE(wanted->IsInPursuit(cop));
    EXPECT_TRUE(IsCopPtrValid(wanted->m_pCopsInPursuit[0]));

    // Now delete the cop WITHOUT calling RemovePursuitCop
    CWorld::Remove(cop);
    delete cop;

    // The pointer in the array is now dangling
    // This is the bug: m_pCopsInPursuit[0] is non-null but points to freed memory
    bool stillValid = IsCopPtrValid(wanted->m_pCopsInPursuit[0]);

    if (wanted->m_pCopsInPursuit[0] != nullptr && !stillValid) {
        GetTestContext().RecordFailure(__FILE__, __LINE__,
            "m_pCopsInPursuit[0] is dangling after cop was deleted without RemovePursuitCop");
    }

    // Clean up: clear the stale pointer
    wanted->m_pCopsInPursuit[0] = nullptr;
    wanted->m_nCopsInPursuit = 0;

    // Restore original pursuit state
    wanted->m_nCopsInPursuit = savedCount;
    wanted->m_nMaxCopsInPursuit = savedMax;
    memcpy(wanted->m_pCopsInPursuit, savedCops, sizeof(savedCops));
}

// --- CanCopJoinPursuit method: count/array desync ---
// The method copies m_pCopsInPursuit to a local array but passes
// m_nCopsInPursuit by reference. RemovePursuitCop inside may decrement
// the real counter while only clearing the local copy.

GAME_DIFF_TEST(CWanted_Pursuit, CanCopJoinPursuit_CountSync) {
    auto* wanted = FindPlayerWanted();
    if (!wanted) return;

    // Save state
    uint8 savedCount = wanted->m_nCopsInPursuit;
    uint8 savedMax = wanted->m_nMaxCopsInPursuit;
    CCopPed* savedCops[CWanted::MAX_COPS_IN_PURSUIT];
    memcpy(savedCops, wanted->m_pCopsInPursuit, sizeof(savedCops));

    auto copModel = CStreaming::GetDefaultCopModel();
    if (!CStreaming::IsModelLoaded((int32)copModel)) {
        wanted->m_nCopsInPursuit = savedCount;
        wanted->m_nMaxCopsInPursuit = savedMax;
        memcpy(wanted->m_pCopsInPursuit, savedCops, sizeof(savedCops));
        return;
    }

    // Create 2 cops and fill pursuit to max=2
    auto* cop1 = new CCopPed(COP_TYPE_CITYCOP);
    auto* cop2 = new CCopPed(COP_TYPE_CITYCOP);
    if (!cop1 || !cop2) {
        if (cop1) delete cop1;
        if (cop2) delete cop2;
        wanted->m_nCopsInPursuit = savedCount;
        wanted->m_nMaxCopsInPursuit = savedMax;
        memcpy(wanted->m_pCopsInPursuit, savedCops, sizeof(savedCops));
        return;
    }

    CVector playerPos = FindPlayerPed()->GetPosition();
    cop1->SetPosn(playerPos + CVector(5.0f, 0.0f, 0.0f));
    cop2->SetPosn(playerPos + CVector(10.0f, 0.0f, 0.0f));
    CWorld::Add(cop1);
    CWorld::Add(cop2);

    // Fill pursuit: 2 cops, max 2
    memset(wanted->m_pCopsInPursuit, 0, sizeof(wanted->m_pCopsInPursuit));
    wanted->m_pCopsInPursuit[0] = cop1;
    wanted->m_pCopsInPursuit[1] = cop2;
    wanted->m_nCopsInPursuit = 2;
    wanted->m_nMaxCopsInPursuit = 2;

    // Save count before CanCopJoinPursuit
    uint8 countBefore = wanted->m_nCopsInPursuit;

    // Call CanCopJoinPursuit(nullptr) — should try to displace
    // Original code at 0x562FB0 vs reversed
    bool origResult, revResult;
    uint8 origCountAfter, revCountAfter;

    // Test reversed
    wanted->m_nCopsInPursuit = countBefore;
    revResult = wanted->CanCopJoinPursuit(nullptr);
    revCountAfter = wanted->m_nCopsInPursuit;

    // Restore and test original
    wanted->m_nCopsInPursuit = countBefore;
    { HookDisableGuard guard("Global/CWanted/CanCopJoinPursuit-method");
      origResult = wanted->CanCopJoinPursuit(nullptr); }
    origCountAfter = wanted->m_nCopsInPursuit;

    EXPECT_EQ(origResult, revResult);
    EXPECT_EQ(origCountAfter, revCountAfter);

    // Check that count still matches actual array contents
    int actualCount = 0;
    for (uint32 i = 0; i < CWanted::MAX_COPS_IN_PURSUIT; i++) {
        if (wanted->m_pCopsInPursuit[i] != nullptr) actualCount++;
    }

    if ((int)wanted->m_nCopsInPursuit != actualCount) {
        char msg[128];
        _snprintf(msg, sizeof(msg),
            "CanCopJoinPursuit desynced: m_nCopsInPursuit=%d but array has %d non-null entries",
            (int)wanted->m_nCopsInPursuit, actualCount);
        GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
    }

    // Cleanup
    CWorld::Remove(cop1);
    CWorld::Remove(cop2);
    delete cop1;
    delete cop2;

    wanted->m_nCopsInPursuit = savedCount;
    wanted->m_nMaxCopsInPursuit = savedMax;
    memcpy(wanted->m_pCopsInPursuit, savedCops, sizeof(savedCops));
}
