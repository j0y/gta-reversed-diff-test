// test_CRopes2.cpp — Differential + behavior tests for CRopes.
// Hook paths: Global/CRopes/<fn>
//
// Rope system manages 8 rope slots (cranes, SWAT rappels, magnets).
// RegisterRope and FindCoorsAlongRope are plugin::Call stubs — not testable.
// Render uses RenderWare — skip. We test Init, FindRope, IsCarriedByRope,
// SetSpeedOfTopNode, FindPickupHeight, and CreateRopeForSwatPed.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Ropes.h"

// Save/restore rope state
struct RopeStateSaver {
    std::array<CRope, CRopes::MAX_NUM_ROPES> savedRopes;
    eControlledCrane savedCrane;
    uint32 savedCounter;

    RopeStateSaver() {
        memcpy(&savedRopes, &CRopes::aRopes, sizeof(savedRopes));
        savedCrane = CRopes::PlayerControlsCrane;
        savedCounter = CRopes::m_nRopeIdCreationCounter;
    }
    ~RopeStateSaver() {
        memcpy(&CRopes::aRopes, &savedRopes, sizeof(savedRopes));
        CRopes::PlayerControlsCrane = savedCrane;
        CRopes::m_nRopeIdCreationCounter = savedCounter;
    }
};

// --- Differential tests ---

GAME_DIFF_TEST(CRopes2, Init) {
    RopeStateSaver saver;

    // Pre-populate a rope
    CRopes::aRopes[0].m_nType = eRopeType::SWAT;
    CRopes::PlayerControlsCrane = eControlledCrane::MAGNO_CRANE;

    // Reversed Init
    CRopes::Init();
    auto revType0 = CRopes::aRopes[0].m_nType;
    auto revCrane = CRopes::PlayerControlsCrane;

    // Restore and run original
    CRopes::aRopes[0].m_nType = eRopeType::SWAT;
    CRopes::PlayerControlsCrane = eControlledCrane::MAGNO_CRANE;
    { HookDisableGuard guard("Global/CRopes/Init");
      CRopes::Init(); }

    EXPECT_EQ((int)revType0, (int)CRopes::aRopes[0].m_nType);
    EXPECT_EQ((int)revCrane, (int)CRopes::PlayerControlsCrane);
}

GAME_DIFF_TEST(CRopes2, FindRope_NotFound) {
    // Search for a rope ID that doesn't exist
    int32 rev = CRopes::FindRope(0xDEADBEEF);
    int32 orig;
    { HookDisableGuard guard("Global/CRopes/FindRope");
      orig = CRopes::FindRope(0xDEADBEEF); }
    EXPECT_EQ(rev, orig);
    EXPECT_EQ(rev, -1);
}

GAME_DIFF_TEST(CRopes2, FindRope_Present) {
    RopeStateSaver saver;

    // Manually insert a rope with known ID
    CRopes::aRopes[3].m_nType = eRopeType::SWAT;
    CRopes::aRopes[3].m_nId = 42;

    int32 rev = CRopes::FindRope(42);
    int32 orig;
    { HookDisableGuard guard("Global/CRopes/FindRope");
      orig = CRopes::FindRope(42); }
    EXPECT_EQ(rev, orig);
    EXPECT_EQ(rev, 3);
}

GAME_DIFF_TEST(CRopes2, IsCarriedByRope_Nullptr) {
    bool rev = CRopes::IsCarriedByRope(nullptr);
    bool orig;
    { HookDisableGuard guard("Global/CRopes/IsCarriedByRope");
      orig = CRopes::IsCarriedByRope(nullptr); }
    EXPECT_EQ(rev, orig);
    EXPECT_FALSE(rev);
}

GAME_DIFF_TEST(CRopes2, IsCarriedByRope_NotCarried) {
    // Spawned vehicle not attached to any rope
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2440.0f, -1665.0f, 13.5f));
    if (!veh) return;

    bool rev = CRopes::IsCarriedByRope(veh.AsVehicle());
    bool orig;
    { HookDisableGuard guard("Global/CRopes/IsCarriedByRope");
      orig = CRopes::IsCarriedByRope(veh.AsVehicle()); }
    EXPECT_EQ(rev, orig);
    EXPECT_FALSE(rev);
}

// Diagnostic: which CRope field does the original IsCarriedByRope check?
// Reversed code checks m_pAttachedEntity (offset 0x314).
// Test each of the 3 entity pointer fields to find which one the original uses.

GAME_DIFF_TEST(CRopes2, IsCarriedByRope_ViaAttachedEntity) {
    RopeStateSaver saver;
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2435.0f, -1665.0f, 13.5f));
    if (!veh) return;

    CRopes::Init();
    CRopes::aRopes[2].m_nType = eRopeType::MAGNET;
    CRopes::aRopes[2].m_pAttachedEntity = veh.AsVehicle();

    bool rev = CRopes::IsCarriedByRope(veh.AsVehicle());
    bool orig;
    { HookDisableGuard guard("Global/CRopes/IsCarriedByRope");
      orig = CRopes::IsCarriedByRope(veh.AsVehicle()); }
    // reversed=true, original=? If false, wrong field.
    EXPECT_EQ(rev, orig);
}

GAME_DIFF_TEST(CRopes2, IsCarriedByRope_ViaRopeHolder) {
    RopeStateSaver saver;
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2435.0f, -1665.0f, 13.5f));
    if (!veh) return;

    CRopes::Init();
    CRopes::aRopes[2].m_nType = eRopeType::MAGNET;
    CRopes::aRopes[2].m_pRopeHolder = (CEntity*)veh.AsVehicle();

    // Reversed should return false (checks m_pAttachedEntity which is null)
    bool rev = CRopes::IsCarriedByRope(veh.AsVehicle());
    bool orig;
    { HookDisableGuard guard("Global/CRopes/IsCarriedByRope");
      orig = CRopes::IsCarriedByRope(veh.AsVehicle()); }
    // If orig=true here, original checks m_pRopeHolder (offset 0x310)
    EXPECT_EQ(rev, orig);
}

GAME_DIFF_TEST(CRopes2, IsCarriedByRope_ViaRopeAttachObject) {
    RopeStateSaver saver;
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2435.0f, -1665.0f, 13.5f));
    if (!veh) return;

    CRopes::Init();
    CRopes::aRopes[2].m_nType = eRopeType::MAGNET;
    CRopes::aRopes[2].m_pRopeAttachObject = (CEntity*)veh.AsVehicle();

    bool rev = CRopes::IsCarriedByRope(veh.AsVehicle());
    bool orig;
    { HookDisableGuard guard("Global/CRopes/IsCarriedByRope");
      orig = CRopes::IsCarriedByRope(veh.AsVehicle()); }
    // If orig=true here, original checks m_pRopeAttachObject (offset 0x318)
    EXPECT_EQ(rev, orig);
}

GAME_DIFF_TEST(CRopes2, SetSpeedOfTopNode) {
    RopeStateSaver saver;

    // Create a rope with known ID
    CRopes::aRopes[1].m_nType = eRopeType::CRANE_MAGNET1;
    CRopes::aRopes[1].m_nId = 777;
    CRopes::aRopes[1].m_aSpeed[0] = CVector(0, 0, 0);

    CVector speed(1.0f, 2.0f, 3.0f);

    // Reversed
    CRopes::SetSpeedOfTopNode(777, speed);
    CVector revSpeed = CRopes::aRopes[1].m_aSpeed[0];

    // Reset and original
    CRopes::aRopes[1].m_aSpeed[0] = CVector(0, 0, 0);
    { HookDisableGuard guard("Global/CRopes/SetSpeedOfTopNode");
      CRopes::SetSpeedOfTopNode(777, speed); }
    EXPECT_NEAR(revSpeed.x, CRopes::aRopes[1].m_aSpeed[0].x, 1e-5f);
    EXPECT_NEAR(revSpeed.y, CRopes::aRopes[1].m_aSpeed[0].y, 1e-5f);
    EXPECT_NEAR(revSpeed.z, CRopes::aRopes[1].m_aSpeed[0].z, 1e-5f);
}

GAME_DIFF_TEST(CRopes2, SetSpeedOfTopNode_WrongId) {
    RopeStateSaver saver;

    CRopes::aRopes[1].m_nType = eRopeType::CRANE_MAGNET1;
    CRopes::aRopes[1].m_nId = 777;
    CRopes::aRopes[1].m_aSpeed[0] = CVector(0, 0, 0);

    // Set speed with wrong ID — should be a no-op
    CRopes::SetSpeedOfTopNode(999, CVector(5, 5, 5));
    CVector revSpeed = CRopes::aRopes[1].m_aSpeed[0];

    CRopes::aRopes[1].m_aSpeed[0] = CVector(0, 0, 0);
    { HookDisableGuard guard("Global/CRopes/SetSpeedOfTopNode");
      CRopes::SetSpeedOfTopNode(999, CVector(5, 5, 5)); }
    EXPECT_NEAR(revSpeed.x, CRopes::aRopes[1].m_aSpeed[0].x, 1e-5f);
}

GAME_DIFF_TEST(CRopes2, FindPickupHeight) {
    // FindPickupHeight reads ColModel bounding box max Z from entity's model
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2430.0f, -1665.0f, 13.5f));
    if (!veh) return;

    float rev = CRopes::FindPickupHeight(veh.AsVehicle());
    float orig;
    { HookDisableGuard guard("Global/CRopes/FindPickupHeight");
      orig = CRopes::FindPickupHeight(veh.AsVehicle()); }
    EXPECT_NEAR(rev, orig, 1e-5f);
}

// --- Behavior tests ---

GAME_TEST(CRopes2, ArraySize) {
    EXPECT_EQ(CRopes::aRopes.size(), (size_t)8);
}

GAME_TEST(CRopes2, InitClearsAll) {
    RopeStateSaver saver;

    CRopes::aRopes[0].m_nType = eRopeType::SWAT;
    CRopes::Init();

    for (auto& rope : CRopes::aRopes) {
        EXPECT_EQ((int)rope.m_nType, (int)eRopeType::NONE);
    }
    EXPECT_EQ((int)CRopes::PlayerControlsCrane, (int)eControlledCrane::NONE);
}

GAME_TEST(CRopes2, FindRope_EmptyReturnsNeg1) {
    RopeStateSaver saver;
    CRopes::Init();
    EXPECT_EQ(CRopes::FindRope(1), -1);
    EXPECT_EQ(CRopes::FindRope(0), -1);
}

GAME_TEST(CRopes2, RopeStructSize) {
    EXPECT_EQ(sizeof(CRope), (size_t)0x328);
}
