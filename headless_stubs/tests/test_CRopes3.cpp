// test_CRopes3.cpp — Additional differential + behavior tests for CRopes.
// Hook paths: Global/CRopes/<fn>
//
// Extends test_CRopes2.cpp with:
// - CreateRopeForSwatPed (behavior only — calls RegisterRope which is plugin::Call)
// - FindPickupHeight with player ped entity
// - IsCarriedByRope with spawned vehicle attached via different rope slots

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Ropes.h"

// Save/restore rope state
struct RopeStateSaver3 {
    std::array<CRope, CRopes::MAX_NUM_ROPES> savedRopes;
    eControlledCrane savedCrane;
    uint32 savedCounter;

    RopeStateSaver3() {
        memcpy(&savedRopes, &CRopes::aRopes, sizeof(savedRopes));
        savedCrane = CRopes::PlayerControlsCrane;
        savedCounter = CRopes::m_nRopeIdCreationCounter;
    }
    ~RopeStateSaver3() {
        memcpy(&CRopes::aRopes, &savedRopes, sizeof(savedRopes));
        CRopes::PlayerControlsCrane = savedCrane;
        CRopes::m_nRopeIdCreationCounter = savedCounter;
    }
};

// --- CreateRopeForSwatPed tests ---
// Note: CreateRopeForSwatPed calls RegisterRope which is { .reversed = false }
// (always original), so differential testing isn't meaningful. Behavior tests only.

GAME_TEST(CRopes3, CreateRopeForSwatPed_ReturnsId) {
    RopeStateSaver3 saver;
    CRopes::Init();

    uint32 counterBefore = CRopes::m_nRopeIdCreationCounter;
    CVector pos(2535.0f, -1665.0f, 13.5f);
    int32 ropeId = CRopes::CreateRopeForSwatPed(pos);

    // CreateRopeForSwatPed returns newRopeId = counter + 100,
    // or -1 if RegisterRope fails.
    if (ropeId != -1) {
        EXPECT_EQ(ropeId, (int32)(counterBefore + 100));
        EXPECT_EQ(CRopes::m_nRopeIdCreationCounter, counterBefore + 1);
    }
    // If -1, RegisterRope failed (no free slot or other issue) — still valid
}

GAME_TEST(CRopes3, CreateRopeForSwatPed_IncrementsCounter) {
    RopeStateSaver3 saver;
    CRopes::Init();

    CRopes::m_nRopeIdCreationCounter = 50;
    CVector pos(2540.0f, -1665.0f, 13.5f);
    int32 ropeId = CRopes::CreateRopeForSwatPed(pos);

    if (ropeId != -1) {
        EXPECT_EQ(ropeId, 150); // 50 + 100
        EXPECT_EQ(CRopes::m_nRopeIdCreationCounter, (uint32)51);
    }
}

GAME_TEST(CRopes3, CreateRopeForSwatPed_FindableAfter) {
    RopeStateSaver3 saver;
    CRopes::Init();

    CVector pos(2545.0f, -1665.0f, 13.5f);
    int32 ropeId = CRopes::CreateRopeForSwatPed(pos);

    if (ropeId != -1) {
        int32 idx = CRopes::FindRope((uint32)ropeId);
        EXPECT_TRUE(idx >= 0);
        if (idx >= 0) {
            EXPECT_EQ((int)CRopes::aRopes[idx].m_nType, (int)eRopeType::SWAT);
        }
    }
}

// --- FindPickupHeight with player ped ---

GAME_DIFF_TEST(CRopes3, FindPickupHeight_PlayerPed) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;

    float rev = CRopes::FindPickupHeight(ped);
    float orig;
    { HookDisableGuard guard("Global/CRopes/FindPickupHeight");
      orig = CRopes::FindPickupHeight(ped); }
    EXPECT_NEAR(rev, orig, 1e-5f);
}

GAME_TEST(CRopes3, FindPickupHeight_PlayerPed_Positive) {
    auto* ped = FindPlayerPed(0);
    if (!ped) return;

    float height = CRopes::FindPickupHeight(ped);
    // Pickup height should be positive (based on model bounding box max Z)
    EXPECT_TRUE(height > 0.0f);
}

// --- IsCarriedByRope with spawned vehicle across multiple slots ---

GAME_DIFF_TEST(CRopes3, IsCarriedByRope_VehicleSlot0) {
    RopeStateSaver3 saver;
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2530.0f, -1665.0f, 13.5f));
    if (!veh) return;

    CRopes::Init();
    CRopes::aRopes[0].m_nType = eRopeType::CRANE_MAGNET1;
    CRopes::aRopes[0].m_pAttachedEntity = veh.AsVehicle();

    bool rev = CRopes::IsCarriedByRope(veh.AsVehicle());
    // Reset and original
    CRopes::aRopes[0].m_pAttachedEntity = veh.AsVehicle();
    CRopes::aRopes[0].m_nType = eRopeType::CRANE_MAGNET1;
    bool orig;
    { HookDisableGuard guard("Global/CRopes/IsCarriedByRope");
      orig = CRopes::IsCarriedByRope(veh.AsVehicle()); }
    EXPECT_EQ(rev, orig);
    EXPECT_TRUE(rev);
}

GAME_DIFF_TEST(CRopes3, IsCarriedByRope_VehicleLastSlot) {
    RopeStateSaver3 saver;
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2535.0f, -1665.0f, 13.5f));
    if (!veh) return;

    CRopes::Init();
    // Put in last slot (index 7)
    CRopes::aRopes[7].m_nType = eRopeType::SWAT;
    CRopes::aRopes[7].m_pAttachedEntity = veh.AsVehicle();

    bool rev = CRopes::IsCarriedByRope(veh.AsVehicle());
    // Reset and original
    CRopes::aRopes[7].m_pAttachedEntity = veh.AsVehicle();
    CRopes::aRopes[7].m_nType = eRopeType::SWAT;
    bool orig;
    { HookDisableGuard guard("Global/CRopes/IsCarriedByRope");
      orig = CRopes::IsCarriedByRope(veh.AsVehicle()); }
    EXPECT_EQ(rev, orig);
    EXPECT_TRUE(rev);
}

GAME_DIFF_TEST(CRopes3, IsCarriedByRope_VehicleTypeNone) {
    RopeStateSaver3 saver;
    int32 vehModel = Scenario::FindLoadedVehicleModel();
    if (vehModel < 0) return;
    Scenario::TestVehicle veh(vehModel, CVector(2540.0f, -1665.0f, 13.5f));
    if (!veh) return;

    // Rope slot has entity but type is NONE — should not count
    CRopes::Init();
    CRopes::aRopes[0].m_nType = eRopeType::NONE;
    CRopes::aRopes[0].m_pAttachedEntity = veh.AsVehicle();

    bool rev = CRopes::IsCarriedByRope(veh.AsVehicle());
    CRopes::aRopes[0].m_nType = eRopeType::NONE;
    CRopes::aRopes[0].m_pAttachedEntity = veh.AsVehicle();
    bool orig;
    { HookDisableGuard guard("Global/CRopes/IsCarriedByRope");
      orig = CRopes::IsCarriedByRope(veh.AsVehicle()); }
    EXPECT_EQ(rev, orig);
    EXPECT_FALSE(rev);
}
