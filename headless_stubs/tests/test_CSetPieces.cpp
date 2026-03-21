// test_CSetPieces.cpp — Differential + behavior tests for CSetPieces and CSetPiece.
// Hook paths: Global/CSetPieces/<fn>, Global/CSetPiece/<fn>
//
// CSetPieces manages 210 set piece slots for scripted cop encounters.
// CSetPiece handles individual piece logic (cop/car spawning).
// TryToGenerateCopPed/TryToGenerateCopCar spawn entities — too risky headlessly.
// We test Init, AddOne, fixed-point coordinate round-trips, and Update early-returns.

#include "StdInc.h"
#include "TestFramework.h"
#include "SetPieces.h"
#include "SetPiece.h"

// --- CSetPieces differential tests ---

GAME_DIFF_TEST(CSetPieces, Init) {
    uint32 savedNum = CSetPieces::NumSetPieces;
    bool savedDebug = CSetPieces::bDebug;

    CSetPieces::NumSetPieces = 42;
    CSetPieces::bDebug = true;
    CSetPieces::Init();
    uint32 revNum = CSetPieces::NumSetPieces;
    bool revDebug = CSetPieces::bDebug;

    CSetPieces::NumSetPieces = 42;
    CSetPieces::bDebug = true;
    { HookDisableGuard guard("Global/CSetPieces/Init");
      CSetPieces::Init(); }
    EXPECT_EQ(revNum, CSetPieces::NumSetPieces);
    EXPECT_EQ(revDebug, CSetPieces::bDebug);

    CSetPieces::NumSetPieces = savedNum;
    CSetPieces::bDebug = savedDebug;
}

GAME_DIFF_TEST(CSetPieces, AddOne) {
    uint32 savedNum = CSetPieces::NumSetPieces;
    // Save the slot we'll write to
    CSetPiece savedPiece{};
    if (savedNum < MAX_SET_PIECES) {
        savedPiece = CSetPieces::aSetPieces[savedNum];
    }

    CVector2D cornerA(100.0f, 200.0f);
    CVector2D cornerB(300.0f, 400.0f);
    CVector2D spawn1(150.0f, 250.0f);
    CVector2D target1(250.0f, 350.0f);
    CVector2D spawn2(120.0f, 220.0f);
    CVector2D target2(280.0f, 380.0f);

    // Reversed AddOne
    CSetPieces::NumSetPieces = savedNum;
    CSetPieces::AddOne(SETPIECE_2PEDS, cornerA, cornerB, spawn1, target1, spawn2, target2);
    uint32 revNum = CSetPieces::NumSetPieces;
    auto& revPiece = CSetPieces::aSetPieces[savedNum];
    eSetPieceType revType = revPiece.m_nType;
    float revCornerAX = revPiece.GetCornerAX();
    float revCornerAY = revPiece.GetCornerAY();
    float revCornerBX = revPiece.GetCornerBX();
    float revCornerBY = revPiece.GetCornerBY();
    float revSpawn1X = revPiece.GetSpawnCoord1X();
    float revSpawn1Y = revPiece.GetSpawnCoord1Y();
    float revTarget1X = revPiece.GetTargetCoord1X();
    float revTarget1Y = revPiece.GetTargetCoord1Y();

    // Reset and run original
    CSetPieces::NumSetPieces = savedNum;
    { HookDisableGuard guard("Global/CSetPieces/AddOne");
      CSetPieces::AddOne(SETPIECE_2PEDS, cornerA, cornerB, spawn1, target1, spawn2, target2); }
    auto& origPiece = CSetPieces::aSetPieces[savedNum];

    EXPECT_EQ(revNum, CSetPieces::NumSetPieces);
    EXPECT_EQ((int)revType, (int)origPiece.m_nType);
    // Fixed-point coords have quantization — use tolerance of 4.0 (the FixedRect scale)
    EXPECT_NEAR(revCornerAX, origPiece.GetCornerAX(), 4.1f);
    EXPECT_NEAR(revCornerAY, origPiece.GetCornerAY(), 4.1f);
    EXPECT_NEAR(revCornerBX, origPiece.GetCornerBX(), 4.1f);
    EXPECT_NEAR(revCornerBY, origPiece.GetCornerBY(), 4.1f);
    EXPECT_NEAR(revSpawn1X, origPiece.GetSpawnCoord1X(), 4.1f);
    EXPECT_NEAR(revSpawn1Y, origPiece.GetSpawnCoord1Y(), 4.1f);
    EXPECT_NEAR(revTarget1X, origPiece.GetTargetCoord1X(), 4.1f);
    EXPECT_NEAR(revTarget1Y, origPiece.GetTargetCoord1Y(), 4.1f);

    // Restore
    CSetPieces::aSetPieces[savedNum] = savedPiece;
    CSetPieces::NumSetPieces = savedNum;
}

GAME_DIFF_TEST(CSetPieces, AddOne_Full) {
    // When array is full, AddOne should be a no-op
    uint32 savedNum = CSetPieces::NumSetPieces;

    CSetPieces::NumSetPieces = MAX_SET_PIECES;
    CSetPieces::AddOne(SETPIECE_1PED, {0,0}, {1,1}, {0,0}, {1,1}, {0,0}, {0,0});
    uint32 revNum = CSetPieces::NumSetPieces;

    CSetPieces::NumSetPieces = MAX_SET_PIECES;
    { HookDisableGuard guard("Global/CSetPieces/AddOne");
      CSetPieces::AddOne(SETPIECE_1PED, {0,0}, {1,1}, {0,0}, {1,1}, {0,0}, {0,0}); }
    EXPECT_EQ(revNum, CSetPieces::NumSetPieces);
    EXPECT_EQ(revNum, (uint32)MAX_SET_PIECES); // should not increment

    CSetPieces::NumSetPieces = savedNum;
}

// --- CSetPiece differential tests ---

GAME_DIFF_TEST(CSetPiece, Update_PlayerOutside) {
    // When player is outside the area rect, Update should early-return (no spawning).
    // Create a set piece far from the player.
    CSetPiece testPiece{};
    testPiece.m_nType = SETPIECE_1PED;
    testPiece.m_nLastGenerationTime = 0;
    // Place area rect far from player
    testPiece.SetCornerA(CVector2D(-9999.0f, -9999.0f));
    testPiece.SetCornerB(CVector2D(-9998.0f, -9998.0f));
    testPiece.SetSpawnCoord1(CVector2D(-9999.0f, -9999.0f));
    testPiece.SetTargetCoord1(CVector2D(-9998.0f, -9998.0f));

    int32 savedGenTime = testPiece.m_nLastGenerationTime;

    // Reversed Update — should early-return, not modify generation time
    testPiece.Update();
    int32 revGenTime = testPiece.m_nLastGenerationTime;

    // Reset and run original
    testPiece.m_nLastGenerationTime = savedGenTime;
    { HookDisableGuard guard("Global/CSetPiece/Update");
      testPiece.Update(); }
    EXPECT_EQ(revGenTime, testPiece.m_nLastGenerationTime);
}

GAME_DIFF_TEST(CSetPiece, Update_RecentlyGenerated) {
    // When m_nLastGenerationTime is recent (within 4000ms), Update should early-return.
    CSetPiece testPiece{};
    testPiece.m_nType = SETPIECE_1PED;
    testPiece.m_nLastGenerationTime = CTimer::GetTimeInMS(); // just now
    // Place area rect at player position so it would trigger if not for the time check
    CVector playerPos = FindPlayerCoors();
    testPiece.SetCornerA(CVector2D(playerPos.x - 100.0f, playerPos.y - 100.0f));
    testPiece.SetCornerB(CVector2D(playerPos.x + 100.0f, playerPos.y + 100.0f));

    testPiece.Update();
    int32 revGenTime = testPiece.m_nLastGenerationTime;

    testPiece.m_nLastGenerationTime = CTimer::GetTimeInMS();
    { HookDisableGuard guard("Global/CSetPiece/Update");
      testPiece.Update(); }
    EXPECT_EQ(revGenTime, testPiece.m_nLastGenerationTime);
}

// --- Behavior tests ---

GAME_TEST(CSetPieces, NumSetPiecesValid) {
    // NumSetPieces should be within array bounds (may be 0 at default spawn)
    EXPECT_TRUE(CSetPieces::NumSetPieces <= MAX_SET_PIECES);
}

GAME_TEST(CSetPieces, AllTypesValid) {
    for (uint32 i = 0; i < CSetPieces::NumSetPieces; i++) {
        auto type = (int)CSetPieces::aSetPieces[i].m_nType;
        EXPECT_TRUE(type >= 0 && type <= 8);
    }
}

GAME_TEST(CSetPiece, FixedPointRoundTrip) {
    // Test that set/get coordinate round-trip preserves values within quantization
    CSetPiece sp{};
    sp.SetCornerA(CVector2D(123.4f, -567.8f));
    sp.SetCornerB(CVector2D(901.2f, -345.6f));
    sp.SetSpawnCoord1(CVector2D(200.0f, 300.0f));
    sp.SetTargetCoord1(CVector2D(400.0f, 500.0f));

    // FixedRect/FixedVector use int16 with scale 4.0 — quantization step is 4.0
    EXPECT_NEAR(sp.GetCornerAX(), 123.4f, 4.1f);
    EXPECT_NEAR(sp.GetCornerAY(), -567.8f, 4.1f);
    EXPECT_NEAR(sp.GetCornerBX(), 901.2f, 4.1f);
    EXPECT_NEAR(sp.GetCornerBY(), -345.6f, 4.1f);
    EXPECT_NEAR(sp.GetSpawnCoord1X(), 200.0f, 4.1f);
    EXPECT_NEAR(sp.GetSpawnCoord1Y(), 300.0f, 4.1f);
    EXPECT_NEAR(sp.GetTargetCoord1X(), 400.0f, 4.1f);
    EXPECT_NEAR(sp.GetTargetCoord1Y(), 500.0f, 4.1f);
}

GAME_TEST(CSetPiece, SizeIs32Bytes) {
    EXPECT_EQ(sizeof(CSetPiece), (size_t)0x20);
}
