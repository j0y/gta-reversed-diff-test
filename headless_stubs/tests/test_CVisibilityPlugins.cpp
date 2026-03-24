// test_CVisibilityPlugins.cpp — Differential tests for CVisibilityPlugins.
// Hook paths: Global/CVisibilityPlugins/<fn>
// 84 hooks total — test the safe query functions.

#include "StdInc.h"
#include "TestFramework.h"
#include "VisibilityPlugins.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

// --- GetClumpModelInfo on player ---

GAME_DIFF_TEST(CVisibilityPlugins, GetClumpModelInfo_Player) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->m_pRwClump) return;
    CBaseModelInfo* orig;
    CBaseModelInfo* rev;
    { HookDisableGuard guard("Global/CVisibilityPlugins/GetClumpModelInfo");
      orig = CVisibilityPlugins::GetClumpModelInfo(player->m_pRwClump); }
    rev = CVisibilityPlugins::GetClumpModelInfo(player->m_pRwClump);
    EXPECT_EQ(orig, rev);
}

// --- GetClumpAlpha on player ---

GAME_DIFF_TEST(CVisibilityPlugins, GetClumpAlpha_Player) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->m_pRwClump) return;
    int32 orig, rev;
    { HookDisableGuard guard("Global/CVisibilityPlugins/GetClumpAlpha");
      orig = CVisibilityPlugins::GetClumpAlpha(player->m_pRwClump); }
    rev = CVisibilityPlugins::GetClumpAlpha(player->m_pRwClump);
    EXPECT_EQ(orig, rev);
}

// --- On spawned vehicle ---

static Scenario::TestVehicle* s_vpVeh = nullptr;
static bool s_triedVpVeh = false;

static CVehicle* GetVpVeh() {
    if (!s_triedVpVeh) {
        s_triedVpVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_vpVeh = new Scenario::TestVehicle(model, CVector(2376.f, -1648.f, 13.5f));
    }
    return s_vpVeh ? s_vpVeh->AsVehicle() : nullptr;
}

GAME_DIFF_TEST(CVisibilityPlugins, GetClumpModelInfo_Vehicle) {
    auto* veh = GetVpVeh();
    if (!veh || !veh->m_pRwClump) return;
    CBaseModelInfo* orig;
    CBaseModelInfo* rev;
    { HookDisableGuard guard("Global/CVisibilityPlugins/GetClumpModelInfo");
      orig = CVisibilityPlugins::GetClumpModelInfo(veh->m_pRwClump); }
    rev = CVisibilityPlugins::GetClumpModelInfo(veh->m_pRwClump);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CVisibilityPlugins, GetClumpAlpha_Vehicle) {
    auto* veh = GetVpVeh();
    if (!veh || !veh->m_pRwClump) return;
    int32 orig, rev;
    { HookDisableGuard guard("Global/CVisibilityPlugins/GetClumpAlpha");
      orig = CVisibilityPlugins::GetClumpAlpha(veh->m_pRwClump); }
    rev = CVisibilityPlugins::GetClumpAlpha(veh->m_pRwClump);
    EXPECT_EQ(orig, rev);
}

// --- IsClumpVisible on player ---

GAME_DIFF_TEST(CVisibilityPlugins, IsClumpVisible_Player) {
    auto* player = FindPlayerPed(0);
    if (!player || !player->m_pRwClump) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CVisibilityPlugins/IsClumpVisible");
      orig = CVisibilityPlugins::IsClumpVisible(player->m_pRwClump); }
    rev = CVisibilityPlugins::IsClumpVisible(player->m_pRwClump);
    EXPECT_EQ(orig, rev);
}

// --- IsClumpVisible on vehicle ---

GAME_DIFF_TEST(CVisibilityPlugins, IsClumpVisible_Vehicle) {
    auto* veh = GetVpVeh();
    if (!veh || !veh->m_pRwClump) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CVisibilityPlugins/IsClumpVisible");
      orig = CVisibilityPlugins::IsClumpVisible(veh->m_pRwClump); }
    rev = CVisibilityPlugins::IsClumpVisible(veh->m_pRwClump);
    EXPECT_EQ(orig, rev);
}

// --- SetClumpModelInfo / GetClumpModelInfo round-trip ---

GAME_DIFF_TEST(CVisibilityPlugins, SetGetClumpModelInfo_RoundTrip) {
    auto* veh = GetVpVeh();
    if (!veh || !veh->m_pRwClump) return;
    auto* saved = CVisibilityPlugins::GetClumpModelInfo(veh->m_pRwClump);
    if (!saved) return;

    { HookDisableGuard guard("Global/CVisibilityPlugins/SetClumpModelInfo");
      CVisibilityPlugins::SetClumpModelInfo(veh->m_pRwClump, saved); }
    auto* origVal = CVisibilityPlugins::GetClumpModelInfo(veh->m_pRwClump);

    CVisibilityPlugins::SetClumpModelInfo(veh->m_pRwClump, saved);
    auto* revVal = CVisibilityPlugins::GetClumpModelInfo(veh->m_pRwClump);

    EXPECT_EQ(origVal, revVal);
    EXPECT_EQ(revVal, saved);
}
