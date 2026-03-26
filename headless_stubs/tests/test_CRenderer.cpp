// test_CRenderer.cpp — Differential tests for CRenderer.
// Hook paths: Global/CRenderer/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Renderer.h"
#include "ScenarioHelpers.h"

// --- ShouldModelBeStreamed ---
// Takes a CEntity*, CVector&, float. Test with player entity.

GAME_DIFF_TEST(CRenderer, ShouldModelBeStreamed_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos = player->GetPosition();

    bool orig, rev;
    { HookDisableGuard guard("Global/CRenderer/ShouldModelBeStreamed");
      orig = CRenderer::ShouldModelBeStreamed(player, pos, 300.0f); }
    rev = CRenderer::ShouldModelBeStreamed(player, pos, 300.0f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CRenderer, ShouldModelBeStreamed_Vehicle) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;

    Scenario::TestVehicle veh(model, CVector(2488.f, -1666.f, 13.f));
    if (!veh) return;

    CVector pos = veh->GetPosition();

    bool orig, rev;
    { HookDisableGuard guard("Global/CRenderer/ShouldModelBeStreamed");
      orig = CRenderer::ShouldModelBeStreamed(veh.AsVehicle(), pos, 300.0f); }
    rev = CRenderer::ShouldModelBeStreamed(veh.AsVehicle(), pos, 300.0f);
    EXPECT_EQ(orig, rev);

    // Test far away — should not be streamed
    CVector farPos(9999.f, 9999.f, 0.f);
    { HookDisableGuard guard("Global/CRenderer/ShouldModelBeStreamed");
      orig = CRenderer::ShouldModelBeStreamed(veh.AsVehicle(), farPos, 300.0f); }
    rev = CRenderer::ShouldModelBeStreamed(veh.AsVehicle(), farPos, 300.0f);
    EXPECT_EQ(orig, rev);
}
