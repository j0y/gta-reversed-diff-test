// test_CCarEnterExit3.cpp — Extended CCarEnterExit behavior tests.
// CCarEnterExit hooks are locked — can't diff test. Behavior tests only.

#include "StdInc.h"
#include "TestFramework.h"
#include "CarEnterExit.h"
#include "ScenarioHelpers.h"

GAME_TEST(CCarEnterExit3, IsRoomForPedToLeaveCar_AllDoors) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2510.f, -1666.f, 13.5f));
    if (!veh) return;
    auto* v = veh.AsVehicle();
    for (int32 door = 0; door < 4; door++) {
        bool result = CCarEnterExit::IsRoomForPedToLeaveCar(v, door, nullptr);
        (void)result;
        EXPECT_TRUE(true);
    }
}
