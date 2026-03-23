// test_CPhysical2.cpp — Direct-call differential tests for CPhysical::GetSpeed.
// GetSpeed returns CVector (12 bytes) — may have sret issues with HookDisableGuard.
// Uses direct asm call to the original at the game address.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Physical.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// Helper: call original CVector-returning thiscall at game address
// CVector __thiscall GetSpeed(CVector* sret, CPhysical* this, CVector point)
// In MSVC x86 thiscall: ecx = this, sret = hidden first stack arg
static CVector CallOriginalGetSpeed(uint32 addr, CPhysical* obj, CVector point) {
    CVector result = {-999.0f, -999.0f, -999.0f};
    CVector* pOut = &result;
    CVector* pPoint = &point;
    void* pThis = obj;
    __asm {
        // Push CVector point (12 bytes, passed by value on stack)
        mov eax, pPoint
        push [eax+8]   // point.z
        push [eax+4]   // point.y
        push [eax]     // point.x
        // Push sret pointer
        mov eax, pOut
        push eax
        // Set this in ecx
        mov ecx, pThis
        // Call
        mov eax, addr
        call eax
        // thiscall: callee cleans up sret + point (16 bytes)
    }
    return result;
}

GAME_TEST(CPhysical2, GetSpeed_Player_DirectCall) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    CVector point(0.0f, 0.0f, 0.0f);

    // Direct call to original at 0x542CE0 (CPhysical::GetSpeed)
    CVector orig = CallOriginalGetSpeed(0x542CE0, player, point);
    CVector rev = player->GetSpeed(point);

    EXPECT_NEAR(orig.x, rev.x, 1e-3f);
    EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    EXPECT_NEAR(orig.z, rev.z, 1e-3f);
}

GAME_TEST(CPhysical2, GetSpeed_Vehicle_DirectCall) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    static Scenario::TestVehicle veh(model, CVector(2412.0f, -1668.0f, 13.5f));
    if (!veh) return;

    CVector point(1.0f, 0.0f, 0.0f);
    CVector orig = CallOriginalGetSpeed(0x542CE0, veh.ptr, point);
    CVector rev = veh.ptr->GetSpeed(point);

    EXPECT_NEAR(orig.x, rev.x, 1e-3f);
    EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    EXPECT_NEAR(orig.z, rev.z, 1e-3f);
}
