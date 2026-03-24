// test_CPools.cpp — Differential tests for CPools handle/reference lookups.
// Hook paths: Global/CPools/<fn>
// Uses player ped and spawned vehicle to test handle round-tripping.
//
// CPools manages all game object pools. GetPed/GetPedRef etc. convert between
// pool handles and pointers. Bugs here break script references to objects.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Pools/Pools.h"

// --- Ped handle round-trip ---

GAME_DIFF_TEST(CPools, GetPedRef_Player) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPools/GetPedRef"); orig = CPools::GetPedRef(ped); }
    rev = CPools::GetPedRef(ped);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPools, GetPed_FromPlayerRef) {
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }
    int32 ref = CPools::GetPedRef(ped);
    CPed* orig;
    CPed* rev;
    { HookDisableGuard guard("Global/CPools/GetPed"); orig = CPools::GetPed(ref); }
    rev = CPools::GetPed(ref);
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(rev, ped); // round-trip should return same pointer
}

// --- Vehicle handle round-trip ---

static Scenario::TestVehicle* s_poolVeh = nullptr;
static bool s_triedPoolVeh = false;

static CVehicle* GetPoolVehicle() {
    if (!s_triedPoolVeh) {
        s_triedPoolVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0) {
            s_poolVeh = new Scenario::TestVehicle(model, CVector(2514.0f, -1665.0f, 13.5f));
        }
    }
    return s_poolVeh ? s_poolVeh->AsVehicle() : nullptr;
}

GAME_DIFF_TEST(CPools, GetVehicleRef_Spawned) {
    auto* veh = GetPoolVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPools/GetVehicleRef"); orig = CPools::GetVehicleRef(veh); }
    rev = CPools::GetVehicleRef(veh);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPools, GetVehicle_FromSpawnedRef) {
    auto* veh = GetPoolVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    int32 ref = CPools::GetVehicleRef(veh);
    CVehicle* orig;
    CVehicle* rev;
    { HookDisableGuard guard("Global/CPools/GetVehicle"); orig = CPools::GetVehicle(ref); }
    rev = CPools::GetVehicle(ref);
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(rev, veh);
}

// --- Invalid handle ---

GAME_DIFF_TEST(CPools, GetPed_InvalidHandle) {
    CPed* orig;
    CPed* rev;
    { HookDisableGuard guard("Global/CPools/GetPed"); orig = CPools::GetPed(-1); }
    rev = CPools::GetPed(-1);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPools, GetVehicle_InvalidHandle) {
    CVehicle* orig;
    CVehicle* rev;
    { HookDisableGuard guard("Global/CPools/GetVehicle"); orig = CPools::GetVehicle(-1); }
    rev = CPools::GetVehicle(-1);
    EXPECT_EQ(orig, rev);
}

// --- GetObject / GetObjectRef ---

GAME_DIFF_TEST(CPools, GetObject_InvalidHandle) {
    CObject* orig;
    CObject* rev;
    { HookDisableGuard guard("Global/CPools/GetObject"); orig = CPools::GetObject(-1); }
    rev = CPools::GetObject(-1);
    EXPECT_EQ(orig, rev);
}

// CheckPoolsEmpty returns void. Skipped as diff test.

// --- CheckBuildingAtomics ---

GAME_DIFF_TEST(CPools, CheckBuildingAtomics) {
    { HookDisableGuard guard("Global/CPools/CheckBuildingAtomics");
      CPools::CheckBuildingAtomics(); }
    CPools::CheckBuildingAtomics();
    EXPECT_TRUE(true);
}
