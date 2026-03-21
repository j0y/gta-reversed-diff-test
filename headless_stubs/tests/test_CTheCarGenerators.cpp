// test_CTheCarGenerators.cpp — Differential + behavior tests for CTheCarGenerators.
// Hook paths: Global/CTheCarGenerators/<fn>
//
// Manages 500 vehicle spawn slots. Tests cover Init, CreateCarGenerator,
// RemoveCarGenerators, and Get accessor. Process/Load/Save depend on
// game state or file I/O so are tested as behavior only.

#include "StdInc.h"
#include "TestFramework.h"
#include "TheCarGenerators.h"
#include "CarGenerator.h"

// --- Differential tests ---

GAME_DIFF_TEST(CTheCarGenerators, Get) {
    // Get(index) returns &CarGeneratorArray[index]. Compare pointers.
    for (uint16 i = 0; i < 10; i++) {
        auto* rev = CTheCarGenerators::Get(i);
        CCarGenerator* orig;
        { HookDisableGuard guard("Global/CTheCarGenerators/Get");
          orig = CTheCarGenerators::Get(i); }
        EXPECT_EQ((uintptr_t)rev, (uintptr_t)orig);
    }
}

GAME_DIFF_TEST(CTheCarGenerators, Init) {
    // Save full state
    uint32 savedNum = CTheCarGenerators::NumOfCarGenerators;
    uint8 savedProcess = CTheCarGenerators::ProcessCounter;
    uint8 savedGenClose = CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter;

    // Save first few generators' used state
    bool savedUsed[10];
    for (int i = 0; i < 10; i++) {
        savedUsed[i] = CTheCarGenerators::CarGeneratorArray[i].m_bIsUsed;
    }

    // Reversed Init
    CTheCarGenerators::Init();
    uint32 revNum = CTheCarGenerators::NumOfCarGenerators;
    uint8 revProcess = CTheCarGenerators::ProcessCounter;
    uint8 revGenClose = CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter;
    bool revUsed0 = CTheCarGenerators::CarGeneratorArray[0].m_bIsUsed;

    // Restore and run original
    CTheCarGenerators::NumOfCarGenerators = savedNum;
    CTheCarGenerators::ProcessCounter = savedProcess;
    CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter = savedGenClose;
    for (int i = 0; i < 10; i++) {
        CTheCarGenerators::CarGeneratorArray[i].m_bIsUsed = savedUsed[i];
    }

    { HookDisableGuard guard("Global/CTheCarGenerators/Init");
      CTheCarGenerators::Init(); }
    EXPECT_EQ(revNum, CTheCarGenerators::NumOfCarGenerators);
    EXPECT_EQ(revProcess, CTheCarGenerators::ProcessCounter);
    EXPECT_EQ(revGenClose, CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter);
    EXPECT_EQ(revUsed0, CTheCarGenerators::CarGeneratorArray[0].m_bIsUsed);

    // Restore original state
    CTheCarGenerators::NumOfCarGenerators = savedNum;
    CTheCarGenerators::ProcessCounter = savedProcess;
    CTheCarGenerators::GenerateEvenIfPlayerIsCloseCounter = savedGenClose;
    for (int i = 0; i < 10; i++) {
        CTheCarGenerators::CarGeneratorArray[i].m_bIsUsed = savedUsed[i];
    }
}

GAME_DIFF_TEST(CTheCarGenerators, CreateCarGenerator) {
    // Save state
    uint32 savedNum = CTheCarGenerators::NumOfCarGenerators;

    // Find a free slot by temporarily clearing one at the end
    int testSlot = -1;
    for (int i = NUM_CAR_GENERATORS - 1; i >= 0; i--) {
        if (!CTheCarGenerators::CarGeneratorArray[i].m_bIsUsed) {
            testSlot = i;
            break;
        }
    }
    if (testSlot < 0) return; // no free slot

    CVector pos(100.0f, 200.0f, 10.0f);

    // Reversed
    int32 revIdx = CTheCarGenerators::CreateCarGenerator(
        pos, 90.0f, 400, -1, -1, 0, 50, 30, 1000, 5000, 0, false);

    int16 revModel = -1;
    if (revIdx >= 0) {
        revModel = CTheCarGenerators::CarGeneratorArray[revIdx].m_nModelId;
        // Clean up
        CTheCarGenerators::CarGeneratorArray[revIdx].m_bIsUsed = false;
        CTheCarGenerators::NumOfCarGenerators--;
    }

    // Original
    { HookDisableGuard guard("Global/CTheCarGenerators/CreateCarGenerator");
      int32 origIdx = CTheCarGenerators::CreateCarGenerator(
          pos, 90.0f, 400, -1, -1, 0, 50, 30, 1000, 5000, 0, false);
      EXPECT_EQ(revIdx, origIdx);
      if (origIdx >= 0) {
          EXPECT_EQ(revModel, CTheCarGenerators::CarGeneratorArray[origIdx].m_nModelId);
          CTheCarGenerators::CarGeneratorArray[origIdx].m_bIsUsed = false;
          CTheCarGenerators::NumOfCarGenerators--;
      }
    }

    CTheCarGenerators::NumOfCarGenerators = savedNum;
}

GAME_DIFF_TEST(CTheCarGenerators, CreateCarGenerator_InvalidModel) {
    // Model ID outside valid range should return -1
    uint32 savedNum = CTheCarGenerators::NumOfCarGenerators;

    int32 rev = CTheCarGenerators::CreateCarGenerator(
        CVector(0, 0, 0), 0.0f, 99999, -1, -1, 0, 0, 0, 0, 0, 0, false);
    int32 orig;
    { HookDisableGuard guard("Global/CTheCarGenerators/CreateCarGenerator");
      orig = CTheCarGenerators::CreateCarGenerator(
          CVector(0, 0, 0), 0.0f, 99999, -1, -1, 0, 0, 0, 0, 0, 0, false); }
    EXPECT_EQ(rev, orig);
    EXPECT_EQ(rev, -1);

    CTheCarGenerators::NumOfCarGenerators = savedNum;
}

GAME_DIFF_TEST(CTheCarGenerators, RemoveCarGenerators) {
    // Set up a test generator with a known IplId, then remove by IplId
    uint32 savedNum = CTheCarGenerators::NumOfCarGenerators;

    // Find unused slot
    int testSlot = -1;
    for (int i = NUM_CAR_GENERATORS - 1; i >= 0; i--) {
        if (!CTheCarGenerators::CarGeneratorArray[i].m_bIsUsed) {
            testSlot = i;
            break;
        }
    }
    if (testSlot < 0) return;

    uint8 testIplId = 255; // unlikely to collide with real IPLs

    // Reversed: add then remove
    auto& slot = CTheCarGenerators::CarGeneratorArray[testSlot];
    slot.m_bIsUsed = true;
    slot.m_nIplId = testIplId;
    CTheCarGenerators::NumOfCarGenerators++;

    CTheCarGenerators::RemoveCarGenerators(testIplId);
    bool revUsed = slot.m_bIsUsed;
    uint32 revNum = CTheCarGenerators::NumOfCarGenerators;

    // Original: add then remove
    slot.m_bIsUsed = true;
    slot.m_nIplId = testIplId;
    CTheCarGenerators::NumOfCarGenerators = revNum + 1; // re-add

    { HookDisableGuard guard("Global/CTheCarGenerators/RemoveCarGenerators");
      CTheCarGenerators::RemoveCarGenerators(testIplId); }
    EXPECT_EQ(revUsed, slot.m_bIsUsed);
    EXPECT_EQ(revNum, CTheCarGenerators::NumOfCarGenerators);

    CTheCarGenerators::NumOfCarGenerators = savedNum;
}

// --- Behavior tests ---

GAME_TEST(CTheCarGenerators, ArrayHasGenerators) {
    // At state 9, some car generators should be active
    EXPECT_TRUE(CTheCarGenerators::NumOfCarGenerators > 0);
}

GAME_TEST(CTheCarGenerators, GetReturnsValidPointers) {
    auto* first = CTheCarGenerators::Get(0);
    auto* last = CTheCarGenerators::Get(NUM_CAR_GENERATORS - 1);
    EXPECT_TRUE(first != nullptr);
    EXPECT_TRUE(last != nullptr);
    EXPECT_TRUE(last > first);
}

GAME_TEST(CTheCarGenerators, GetIndexRoundtrip) {
    auto* gen = CTheCarGenerators::Get(42);
    int32 idx = CTheCarGenerators::GetIndex(gen);
    EXPECT_EQ(idx, 42);
}
