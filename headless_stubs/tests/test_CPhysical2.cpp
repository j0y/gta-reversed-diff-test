// test_CPhysical2.cpp — Direct-call differential tests for CPhysical::GetSpeed.
// GetSpeed returns CVector (12 bytes) — uses sret workaround via helper.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Physical.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// GetSpeed: CVector __thiscall(CPhysical* this, CVector point)
// Address: 0x542CE0
// CVector point is 12 bytes passed by value — needs custom asm (not covered by helpers).
static CVector CallOriginalGetSpeed(uint32 addr, CPhysical* obj, CVector point) {
    CVector result = {-999.0f, -999.0f, -999.0f};
    CVector* pOut = &result;
    CVector* pPoint = &point;
    void* pThis = obj;
    __asm {
        mov eax, pPoint
        push [eax+8]   // point.z
        push [eax+4]   // point.y
        push [eax]     // point.x
        mov eax, pOut
        push eax
        mov ecx, pThis
        mov eax, addr
        call eax
    }
    return result;
}

GAME_TEST(CPhysical2, GetSpeed_Player_DirectCall) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector points[] = { {0,0,0}, {1,0,0}, {0,0,1}, {0.5f,0.5f,0.5f} };
    for (auto& pt : points) {
        CVector orig = CallOriginalGetSpeed(0x542CE0, player, pt);
        CVector rev = player->GetSpeed(pt);
        EXPECT_NEAR(orig.x, rev.x, 1e-5f);
        EXPECT_NEAR(orig.y, rev.y, 1e-5f);
        EXPECT_NEAR(orig.z, rev.z, 1e-5f);
    }
}

static Scenario::TestVehicle* s_physDcVeh = nullptr;
static bool s_triedPhysDcVeh = false;

static CPhysical* GetPhysDcVehicle() {
    if (!s_triedPhysDcVeh) {
        s_triedPhysDcVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_physDcVeh = new Scenario::TestVehicle(model, CVector(2502.0f, -1665.0f, 13.5f));
    }
    return s_physDcVeh ? static_cast<CPhysical*>(s_physDcVeh->AsVehicle()) : nullptr;
}

GAME_TEST(CPhysical2, GetSpeed_Vehicle_DirectCall) {
    auto* phys = GetPhysDcVehicle();
    if (!phys) return;
    CVector points[] = { {0,0,0}, {1,0,0}, {0,1,0} };
    for (auto& pt : points) {
        CVector orig = CallOriginalGetSpeed(0x542CE0, phys, pt);
        CVector rev = phys->GetSpeed(pt);
        EXPECT_NEAR(orig.x, rev.x, 1e-5f);
        EXPECT_NEAR(orig.y, rev.y, 1e-5f);
        EXPECT_NEAR(orig.z, rev.z, 1e-5f);
    }
}
