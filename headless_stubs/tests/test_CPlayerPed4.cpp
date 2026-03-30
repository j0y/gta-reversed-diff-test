// test_CPlayerPed4.cpp — Differential test for CPlayerPed::ProcessControl.
// Hook path: Entity/Ped/CPlayerPed/ProcessControl
//
// Tests the look-direction block inside ProcessControl which was broken by
// RadiansToDegrees/DegreesToRadians confusion (bug #60).
// The bug made `angle > RadiansToDegrees(30)` always false (comparing
// a radian angle ~0-6.28 against ~1718 degrees), killing the entire
// look-direction feature in 3rd-person follow camera mode.
//
// We test by running ProcessControl and comparing look-related state.

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CPlayerPed4, Diff_ProcessControl_LookState) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* pad = CPad::GetPad(0);
    if (!pad) return;

    // The look-direction block requires MODE_FOLLOWPED and the camera not looking
    // in a specific direction. Force these conditions.
    auto& cam = TheCamera.m_aCams[TheCamera.m_nActiveCam];
    auto savedCamMode = cam.m_nMode;
    auto savedCamLooking = cam.m_nDirectionWasLooking;
    cam.m_nMode = MODE_FOLLOWPED;
    cam.m_nDirectionWasLooking = 0;

    // Set player rotation different from camera direction to trigger the look block
    // Camera front is cam.m_vecFront, player rotation is m_fCurrentRotation
    auto savedRotation = player->m_fCurrentRotation;
    // Rotate player 90° away from camera to ensure angle > DegreesToRadians(30)
    float camDir = CGeneral::LimitRadianAngle(atan2(-cam.m_vecFront.x, cam.m_vecFront.y));
    player->m_fCurrentRotation = CGeneral::LimitRadianAngle(camDir + DegreesToRadians(90.0f));

    // Save state
    auto savedLookTime = player->m_nLookTime;
    auto savedLookDir = player->m_fLookDirection;
    auto savedPedState = player->m_nPedState;
    CVector savedPos = player->GetPosition();
    CVector savedMoveSpeed = player->m_vecMoveSpeed;

    // Run original ProcessControl
    player->m_nLookTime = 0;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/ProcessControl");
      player->ProcessControl(); }
    auto origLookTime = player->m_nLookTime;

    // Restore state for reversed call
    player->m_nLookTime = 0;
    player->m_fLookDirection = savedLookDir;
    player->m_nPedState = savedPedState;
    player->m_fCurrentRotation = CGeneral::LimitRadianAngle(camDir + DegreesToRadians(90.0f));
    player->SetPosn(savedPos);
    player->m_vecMoveSpeed = savedMoveSpeed;

    // Run reversed ProcessControl
    player->ProcessControl();
    auto revLookTime = player->m_nLookTime;

    // With the bug (RadiansToDegrees), the look block is dead code:
    // origLookTime gets set by original code, revLookTime stays 0.
    // With the fix, both should match.
    EXPECT_EQ(origLookTime, revLookTime);

    // Restore everything
    cam.m_nMode = savedCamMode;
    cam.m_nDirectionWasLooking = savedCamLooking;
    player->m_nLookTime = savedLookTime;
    player->m_fLookDirection = savedLookDir;
    player->m_nPedState = savedPedState;
    player->m_fCurrentRotation = savedRotation;
    player->SetPosn(savedPos);
    player->m_vecMoveSpeed = savedMoveSpeed;
}
