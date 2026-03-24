// test_CVehicle_DirectCall.cpp — Direct-call differential tests for CVehicle
// struct-returning member functions.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_dcVeh = nullptr;
static bool s_triedDcVeh = false;

static CVehicle* GetDcVehicle() {
    if (!s_triedDcVeh) {
        s_triedDcVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_dcVeh = new Scenario::TestVehicle(model, CVector(2398.f, -1668.f, 13.5f));
    }
    return s_dcVeh ? s_dcVeh->AsVehicle() : nullptr;
}

// --- GetPlaneGunsPosition (0x6D4290, thiscall, sret CVector) ---

GAME_TEST(CVehicle_DirectCall, GetPlaneGunsPosition_AllGuns) {
    auto* veh = GetDcVehicle();
    if (!veh) return;
    for (int32 gunId = 0; gunId < 4; gunId++) {
        CVector orig = CallOriginal_CVector_Thiscall(0x6D4290, veh, gunId);
        CVector rev = veh->GetPlaneGunsPosition(gunId);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
        EXPECT_NEAR(orig.z, rev.z, 1e-3f);
    }
}

// --- GetPlaneOrdnancePosition (0x6D46E0, thiscall, sret CVector) ---

GAME_TEST(CVehicle_DirectCall, GetPlaneOrdnancePosition_AllTypes) {
    auto* veh = GetDcVehicle();
    if (!veh) return;
    for (int32 ordType = 0; ordType < 4; ordType++) {
        CVector orig = CallOriginal_CVector_Thiscall(0x6D46E0, veh, ordType);
        CVector rev = veh->GetPlaneOrdnancePosition(static_cast<eOrdnanceType>(ordType));
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
        EXPECT_NEAR(orig.z, rev.z, 1e-3f);
    }
}
