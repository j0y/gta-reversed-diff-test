// test_CTaskComplexDie.cpp — Tests for CTaskComplexDie.
// Hook paths: Tasks/TaskTypes/CTaskComplexDie/<fn>
//
// CTaskComplexDie handles the death sequence for peds — animation selection,
// stealth kills, fall-to-death, and death audio samples. Tests verify
// construction, cloning, and type/flag correctness.

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexDie.h"
#include "Entity/Ped/Ped.h"

// --- Construction & type ---

GAME_TEST(CTaskComplexDie, GetTaskType) {
    CTaskComplexDie task(WEAPON_UNARMED, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_DIE);
}

GAME_TEST(CTaskComplexDie, IsComplex) {
    CTaskComplexDie task(WEAPON_UNARMED, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0);
    EXPECT_FALSE(task.IsSimple());
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexDie, Constructor_Defaults) {
    CTaskComplexDie task(WEAPON_PISTOL, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0);
    EXPECT_EQ((int32)task.m_nWeaponType, (int32)WEAPON_PISTOL);
    EXPECT_EQ((int32)task.m_nAnimGroup, (int32)ANIM_GROUP_DEFAULT);
    EXPECT_EQ((int32)task.m_nAnimID, (int32)ANIM_ID_KO_SHOT_FRONT_0);
    EXPECT_NEAR(task.m_fBlendDelta, 4.0f, 1e-5f);
    EXPECT_NEAR(task.m_fAnimSpeed, 0.0f, 1e-5f);
    EXPECT_FALSE(task.m_bBeingKilledByStealth);
    EXPECT_FALSE(task.m_bFallingToDeath);
    EXPECT_FALSE(task.m_bFallToDeathOverRailing);
}

GAME_TEST(CTaskComplexDie, Constructor_StealthKill) {
    CTaskComplexDie task(WEAPON_KNIFE, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0,
                         8.0f, 1.0f, true, false, eDirection::FORWARD, false);
    EXPECT_EQ((int32)task.m_nWeaponType, (int32)WEAPON_KNIFE);
    EXPECT_NEAR(task.m_fBlendDelta, 8.0f, 1e-5f);
    EXPECT_NEAR(task.m_fAnimSpeed, 1.0f, 1e-5f);
    EXPECT_TRUE(task.m_bBeingKilledByStealth);
    EXPECT_FALSE(task.m_bFallingToDeath);
}

GAME_TEST(CTaskComplexDie, Constructor_FallToDeath) {
    CTaskComplexDie task(WEAPON_UNARMED, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0,
                         4.0f, 0.0f, false, true, eDirection::LEFT, true);
    EXPECT_TRUE(task.m_bFallingToDeath);
    EXPECT_TRUE(task.m_bFallToDeathOverRailing);
    EXPECT_EQ((int32)task.m_nFallToDeathDir, (int32)eDirection::LEFT);
}

// --- Clone ---

GAME_TEST(CTaskComplexDie, Clone_PreservesAllFields) {
    CTaskComplexDie original(WEAPON_AK47, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0,
                             6.0f, 2.0f, true, true, eDirection::RIGHT, true);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_DIE);
    EXPECT_TRUE(cloned->IsComplex());

    auto* c = static_cast<CTaskComplexDie*>(cloned);
    EXPECT_EQ((int32)c->m_nWeaponType, (int32)WEAPON_AK47);
    EXPECT_EQ((int32)c->m_nAnimGroup, (int32)ANIM_GROUP_DEFAULT);
    EXPECT_EQ((int32)c->m_nAnimID, (int32)ANIM_ID_KO_SHOT_FRONT_0);
    EXPECT_NEAR(c->m_fBlendDelta, 6.0f, 1e-5f);
    EXPECT_NEAR(c->m_fAnimSpeed, 2.0f, 1e-5f);
    EXPECT_TRUE(c->m_bBeingKilledByStealth);
    EXPECT_TRUE(c->m_bFallingToDeath);
    EXPECT_EQ((int32)c->m_nFallToDeathDir, (int32)eDirection::RIGHT);
    EXPECT_TRUE(c->m_bFallToDeathOverRailing);

    delete cloned;
}

GAME_TEST(CTaskComplexDie, Clone_IsSeparateObject) {
    CTaskComplexDie original(WEAPON_UNARMED, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}

// MakeAbortable delegates to m_pSubTask which is null on a freshly constructed
// CTaskComplexDie — calling it would crash. Skipping differential test for
// MakeAbortable since it requires an active subtask (created by CreateFirstSubTask
// during actual ped processing).

// --- Weapon type coverage ---

GAME_TEST(CTaskComplexDie, DifferentWeaponTypes) {
    eWeaponType weapons[] = {
        WEAPON_UNARMED, WEAPON_KNIFE, WEAPON_PISTOL, WEAPON_SHOTGUN,
        WEAPON_AK47, WEAPON_SNIPERRIFLE, WEAPON_ROCKET, WEAPON_FLAMETHROWER
    };
    for (auto wt : weapons) {
        CTaskComplexDie task(wt, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0);
        EXPECT_EQ((int32)task.m_nWeaponType, (int32)wt);
        EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_DIE);
    }
}
