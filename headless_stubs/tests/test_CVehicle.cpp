// test_CVehicle.cpp — Differential tests for CVehicle member functions.
// Hook paths: Vehicle/CVehicle/<fn>
// Uses ScenarioHelpers to create a real vehicle from a loaded model.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"

// Shared vehicle for all tests — created once, reused
static Scenario::TestVehicle* s_veh = nullptr;
static bool s_triedCreate = false;

static CVehicle* GetTestVehicle() {
    if (!s_triedCreate) {
        s_triedCreate = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0) {
            // Place near player spawn at Grove Street
            s_veh = new Scenario::TestVehicle(model, CVector(2495.0f, -1665.0f, 13.5f));
        }
    }
    return s_veh ? s_veh->AsVehicle() : nullptr;
}

#define DIFF_VEH_BOOL(FnName) \
    GAME_DIFF_TEST(CVehicle, FnName) { \
        auto* veh = GetTestVehicle(); \
        if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle — no loaded model"); return; } \
        bool orig, rev; \
        { HookDisableGuard guard("Vehicle/CVehicle/" #FnName); orig = veh->FnName(); } \
        rev = veh->FnName(); \
        EXPECT_EQ(orig, rev); \
    }

DIFF_VEH_BOOL(IsLawEnforcementVehicle)
DIFF_VEH_BOOL(IsUpsideDown)
DIFF_VEH_BOOL(IsOnItsSide)
DIFF_VEH_BOOL(CanBeDriven)
DIFF_VEH_BOOL(UsesSiren)
DIFF_VEH_BOOL(CanPedEnterCar)

GAME_DIFF_TEST(CVehicle, CanPedLeanOut) {
    auto* veh = GetTestVehicle();
    if (!veh) return;
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedLeanOut"); orig = veh->CanPedLeanOut(ped); }
    rev = veh->CanPedLeanOut(ped);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CVehicle, GetVehicleAppearance) {
    auto* veh = GetTestVehicle();
    if (!veh) return;
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleAppearance"); orig = (int)veh->GetVehicleAppearance(); }
    rev = (int)veh->GetVehicleAppearance();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CVehicle, GetVehicleLightsStatus) {
    auto* veh = GetTestVehicle();
    if (!veh) return;
    int orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetVehicleLightsStatus"); orig = veh->GetVehicleLightsStatus(); }
    rev = veh->GetVehicleLightsStatus();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CVehicle, CanPedOpenLocks) {
    auto* veh = GetTestVehicle();
    if (!veh) return;
    auto* ped = FindPlayerPed(0);
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/CanPedOpenLocks"); orig = veh->CanPedOpenLocks(ped); }
    rev = veh->CanPedOpenLocks(ped);
    EXPECT_EQ(orig, rev);
}

// Entity-level queries on the spawned vehicle
GAME_DIFF_TEST(CVehicle, Entity_IsVisible) {
    auto* veh = GetTestVehicle();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/IsVisible"); orig = veh->IsVisible(); }
    rev = veh->IsVisible();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CVehicle, Entity_GetColModel) {
    auto* veh = GetTestVehicle();
    if (!veh) return;
    CColModel* orig;
    CColModel* rev;
    { HookDisableGuard guard("Entity/CEntity/GetColModel"); orig = veh->GetColModel(); }
    rev = veh->GetColModel();
    EXPECT_EQ(orig, rev);
}

// --- Additional queries ---

DIFF_VEH_BOOL(CanDoorsBeDamaged)
DIFF_VEH_BOOL(IsVehicleNormal)
DIFF_VEH_BOOL(CarHasRoof)

GAME_DIFF_TEST(CVehicle, GetFiringRateMultiplier) {
    auto* veh = GetTestVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle — no loaded model"); return; }
    float orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetFiringRateMultiplier"); orig = veh->GetFiringRateMultiplier(); }
    rev = veh->GetFiringRateMultiplier();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CVehicle, GetPlaneGunsAutoAimAngle) {
    auto* veh = GetTestVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle — no loaded model"); return; }
    float orig, rev;
    { HookDisableGuard guard("Vehicle/CVehicle/GetPlaneGunsAutoAimAngle"); orig = veh->GetPlaneGunsAutoAimAngle(); }
    rev = veh->GetPlaneGunsAutoAimAngle();
    EXPECT_NEAR(orig, rev, 1e-5f);
}

GAME_DIFF_TEST(CVehicle, IsSphereTouchingVehicle) {
    auto* veh = GetTestVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle — no loaded model"); return; }
    CVector vehPos = veh->GetPosition();
    // Sphere at vehicle position — should touch
    bool origTouch, revTouch;
    { HookDisableGuard guard("Vehicle/CVehicle/IsSphereTouchingVehicle"); origTouch = veh->IsSphereTouchingVehicle(vehPos, 5.0f); }
    revTouch = veh->IsSphereTouchingVehicle(vehPos, 5.0f);
    EXPECT_EQ(origTouch, revTouch);
    // Sphere far away — should not touch
    CVector farPos(0.0f, 0.0f, 0.0f);
    bool origFar, revFar;
    { HookDisableGuard guard("Vehicle/CVehicle/IsSphereTouchingVehicle"); origFar = veh->IsSphereTouchingVehicle(farPos, 1.0f); }
    revFar = veh->IsSphereTouchingVehicle(farPos, 1.0f);
    EXPECT_EQ(origFar, revFar);
}

// Diagnostic
GAME_TEST(CVehicle, SpawnDiagnostic) {
    auto* veh = GetTestVehicle();
    char buf[256];
    if (veh) {
        snprintf(buf, sizeof(buf), "[DIAG] Spawned vehicle: model=%d type=%d health=%.0f pos=(%.1f,%.1f,%.1f)",
            veh->GetModelIndex(), (int)veh->m_nVehicleType, veh->m_fHealth,
            veh->GetPosition().x, veh->GetPosition().y, veh->GetPosition().z);
    } else {
        int32 model = Scenario::FindLoadedVehicleModel();
        snprintf(buf, sizeof(buf), "[DIAG] Failed to spawn vehicle. FindLoadedVehicleModel=%d", model);
    }
    { FILE* f = fopen("C:\\veh_diag.log", "w"); if(f) { fputs(buf, f); fputc('\n', f); fclose(f); } }
    EXPECT_TRUE(veh != nullptr);
}
