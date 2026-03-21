// test_CVehicle2.cpp — Extended differential tests for CVehicle.
// Hook paths: Vehicle/CVehicle/<fn>
//
// Tests safe query/setter methods not covered in test_CVehicle.cpp.
// Uses the same spawned vehicle approach via ScenarioHelpers.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"

// Shared vehicle — same pattern as test_CVehicle.cpp
static Scenario::TestVehicle* s_veh2 = nullptr;
static bool s_tried2 = false;

static CVehicle* GetVeh() {
    if (!s_tried2) {
        s_tried2 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0) {
            s_veh2 = new Scenario::TestVehicle(model, CVector(2480.0f, -1665.0f, 13.5f));
        }
    }
    return s_veh2 ? s_veh2->AsVehicle() : nullptr;
}

#define VEH_GUARD(fn) if (!GetVeh()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }

// --- Simple bool queries ---

#define DIFF_VEH2_BOOL(FnName) \
    GAME_DIFF_TEST(CVehicle2, FnName) { \
        VEH_GUARD(FnName) \
        auto* v = GetVeh(); \
        bool rev = v->FnName(); \
        bool orig; \
        { HookDisableGuard guard("Vehicle/CVehicle/" #FnName); orig = v->FnName(); } \
        EXPECT_EQ(rev, orig); \
    }

DIFF_VEH2_BOOL(CanBeDeleted)

GAME_DIFF_TEST(CVehicle2, CanVehicleBeDamaged) {
    VEH_GUARD(CanVehicleBeDamaged)
    auto* v = GetVeh();
    eWeaponType weapons[] = { WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_M4 };
    for (auto wep : weapons) {
        bool revFlag = false, origFlag = false;
        bool rev = v->CanVehicleBeDamaged(nullptr, wep, revFlag);
        bool orig;
        { HookDisableGuard guard("Vehicle/CVehicle/CanVehicleBeDamaged");
          orig = v->CanVehicleBeDamaged(nullptr, wep, origFlag); }
        EXPECT_EQ(rev, orig);
        EXPECT_EQ(revFlag, origFlag);
    }
}

GAME_DIFF_TEST(CVehicle2, GetRemapIndex) {
    VEH_GUARD(GetRemapIndex)
    auto* v = GetVeh();
    int32 rev = v->GetRemapIndex();
    int32 orig;
    { HookDisableGuard guard("Vehicle/CVehicle/GetRemapIndex");
      orig = v->GetRemapIndex(); }
    EXPECT_EQ(rev, orig);
}

GAME_DIFF_TEST(CVehicle2, IsPassenger_Ped) {
    VEH_GUARD(IsPassenger_Ped)
    auto* v = GetVeh();
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    // No passengers in spawned vehicle — should return false
    bool rev = v->IsPassenger(ped);
    bool orig;
    { HookDisableGuard guard("Vehicle/CVehicle/IsPassenger-Ped");
      orig = v->IsPassenger(ped); }
    EXPECT_EQ(rev, orig);
}

GAME_DIFF_TEST(CVehicle2, IsPassenger_ModelID) {
    VEH_GUARD(IsPassenger_ModelID)
    auto* v = GetVeh();
    // Test with player model and a random model
    int32 models[] = { 0, 7, 105 };
    for (int32 m : models) {
        bool rev = v->IsPassenger(m);
        bool orig;
        { HookDisableGuard guard("Vehicle/CVehicle/IsPassenger-ModelID");
          orig = v->IsPassenger(m); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(CVehicle2, FindTyreNearestPoint) {
    // BUG CONFIRMED: Reversed code uses GetForward() for left/right and GetRight()
    // for front/rear — the original at 0x6D7BC0 uses different axis mapping or
    // wheel enum mapping. The reversed developers noted this in TODO comments.
    VEH_GUARD(FindTyreNearestPoint)
    auto* v = GetVeh();
    CVector vehPos = v->GetPosition();
    // Test 8 points around the vehicle to cover all quadrants
    CVector2D testPoints[] = {
        { vehPos.x + 2.0f, vehPos.y },
        { vehPos.x - 2.0f, vehPos.y },
        { vehPos.x, vehPos.y + 2.0f },
        { vehPos.x, vehPos.y - 2.0f },
        { vehPos.x + 2.0f, vehPos.y + 2.0f },
        { vehPos.x - 2.0f, vehPos.y - 2.0f },
        { vehPos.x + 2.0f, vehPos.y - 2.0f },
        { vehPos.x - 2.0f, vehPos.y + 2.0f },
    };
    for (auto& pt : testPoints) {
        auto rev = v->FindTyreNearestPoint(pt);
        eCarWheel orig;
        { HookDisableGuard guard("Vehicle/CVehicle/FindTyreNearestPoint");
          orig = v->FindTyreNearestPoint(pt); }
        EXPECT_EQ((int)rev, (int)orig);
    }
}

// --- Flag setters/clearers (test round-trip: set then clear) ---

GAME_DIFF_TEST(CVehicle2, GettingInFlags) {
    VEH_GUARD(GettingInFlags)
    auto* v = GetVeh();

    // Save original state
    auto savedFlags = v->m_nGettingInFlags;

    // Test SetGettingInFlags
    v->m_nGettingInFlags = 0;
    v->SetGettingInFlags(0x03);
    uint8 revAfterSet = v->m_nGettingInFlags;

    v->m_nGettingInFlags = 0;
    { HookDisableGuard guard("Vehicle/CVehicle/SetGettingInFlags");
      v->SetGettingInFlags(0x03); }
    EXPECT_EQ(revAfterSet, v->m_nGettingInFlags);

    // Test ClearGettingInFlags
    v->m_nGettingInFlags = 0xFF;
    v->ClearGettingInFlags(0x0F);
    uint8 revAfterClear = v->m_nGettingInFlags;

    v->m_nGettingInFlags = 0xFF;
    { HookDisableGuard guard("Vehicle/CVehicle/ClearGettingInFlags");
      v->ClearGettingInFlags(0x0F); }
    EXPECT_EQ(revAfterClear, v->m_nGettingInFlags);

    v->m_nGettingInFlags = savedFlags;
}

GAME_DIFF_TEST(CVehicle2, GettingOutFlags) {
    VEH_GUARD(GettingOutFlags)
    auto* v = GetVeh();

    auto savedFlags = v->m_nGettingOutFlags;

    v->m_nGettingOutFlags = 0;
    v->SetGettingOutFlags(0x05);
    uint8 revAfterSet = v->m_nGettingOutFlags;

    v->m_nGettingOutFlags = 0;
    { HookDisableGuard guard("Vehicle/CVehicle/SetGettingOutFlags");
      v->SetGettingOutFlags(0x05); }
    EXPECT_EQ(revAfterSet, v->m_nGettingOutFlags);

    v->m_nGettingOutFlags = 0xFF;
    v->ClearGettingOutFlags(0x0A);
    uint8 revAfterClear = v->m_nGettingOutFlags;

    v->m_nGettingOutFlags = 0xFF;
    { HookDisableGuard guard("Vehicle/CVehicle/ClearGettingOutFlags");
      v->ClearGettingOutFlags(0x0A); }
    EXPECT_EQ(revAfterClear, v->m_nGettingOutFlags);

    v->m_nGettingOutFlags = savedFlags;
}

// --- Handling data queries via spawned vehicle ---

GAME_TEST(CVehicle2, HandlingData_NonNull) {
    VEH_GUARD(HandlingData_NonNull)
    auto* v = GetVeh();
    EXPECT_TRUE(v->m_pHandlingData != nullptr);
    if (v->m_pHandlingData) {
        EXPECT_TRUE(v->m_pHandlingData->m_fMass > 0.0f);
    }
}

GAME_TEST(CVehicle2, ModelInfo_Consistent) {
    VEH_GUARD(ModelInfo_Consistent)
    auto* v = GetVeh();
    auto* mi = v->GetModelInfo();
    EXPECT_TRUE(mi != nullptr);
    // Model type should be vehicle
    if (mi) {
        EXPECT_EQ((int)mi->GetModelType(), (int)MODEL_INFO_VEHICLE);
    }
}

GAME_TEST(CVehicle2, HealthPositive) {
    VEH_GUARD(HealthPositive)
    auto* v = GetVeh();
    EXPECT_TRUE(v->m_fHealth > 0.0f);
    EXPECT_TRUE(v->m_fHealth <= 1000.0f);
}

GAME_TEST(CVehicle2, NoOccupants) {
    VEH_GUARD(NoOccupants)
    auto* v = GetVeh();
    // Freshly spawned vehicle should have no driver or passengers
    EXPECT_TRUE(v->m_pDriver == nullptr);
    EXPECT_EQ(v->m_nNumPassengers, (uint8)0);
}

GAME_TEST(CVehicle2, VehicleType) {
    VEH_GUARD(VehicleType)
    auto* v = GetVeh();
    // Should be a car type (model 596 is police car = VEHICLE_TYPE_AUTOMOBILE)
    int type = (int)v->m_nVehicleType;
    EXPECT_TRUE(type >= 0 && type <= 10); // valid vehicle type range
}

GAME_TEST(CVehicle2, ColModel_NonNull) {
    VEH_GUARD(ColModel_NonNull)
    auto* v = GetVeh();
    auto* col = v->GetColModel();
    EXPECT_TRUE(col != nullptr);
    if (col) {
        // Collision model should have some bounding sphere radius
        EXPECT_TRUE(col->GetBoundRadius() > 0.0f);
    }
}
